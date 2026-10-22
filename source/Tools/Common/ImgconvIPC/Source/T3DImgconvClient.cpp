/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#include "T3DImgconvClient.h"
#include <IO/T3DDir.h>
#include <T3DLog.h>
#include <chrono>
#include <thread>


namespace Tiny3D
{
    #define LOG_TAG_IMGCONV_IPC "ImgconvIPC"

    //--------------------------------------------------------------------------

    ImgconvClient::ImgconvClient()
    {
        mSendScratch.resize(64 * 1024);
    }

    //--------------------------------------------------------------------------

    ImgconvClient::~ImgconvClient()
    {
        shutdown();
    }

    //--------------------------------------------------------------------------

    String ImgconvClient::resolveDefaultExePath()
    {
        String appDir = Dir::getAppPath();
        if (!appDir.empty())
        {
            char last = appDir.back();
            if (last != '/' && last != '\\')
            {
                appDir += Dir::getNativeSeparator();
            }
        }
#if defined(T3D_OS_WINDOWS)
        return appDir + "imgconv.exe";
#else
        return appDir + "imgconv";
#endif
    }

    //--------------------------------------------------------------------------

    TResult ImgconvClient::ensureStarted(const String &exePath, uint32_t handshakeTimeoutMs)
    {
        if (isReady())
        {
            return T3D_OK;
        }

        // 清理可能残留的旧会话
        shutdown();

        mExePath = exePath.empty() ? resolveDefaultExePath() : exePath;
        if (!Dir::exists(mExePath))
        {
            T3D_LOG_ERROR(LOG_TAG_IMGCONV_IPC, "imgconv executable not found: %s", mExePath.c_str());
            return T3D_ERR_FILE_NOT_EXIST;
        }

        mListener = T3D_NEW TCPListener();
        mListener->setOnAcceptedCallback(
            [this](TCPListener *listener, TCPConnection *connection)
            {
                onAccepted(listener, connection);
            });

        // 绑定 127.0.0.1，端口 0 由 OS 自动分配空闲端口
        TResult ret = mListener->listen(0, "127.0.0.1", 1);
        if (T3D_FAILED(ret))
        {
            T3D_LOG_ERROR(LOG_TAG_IMGCONV_IPC, "Failed to listen on 127.0.0.1:0 ! ret=0x%08X", ret);
            shutdown();
            return ret;
        }

        uint16_t port = mListener->getListenPort();
        if (port == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_IMGCONV_IPC, "Failed to get listening port !");
            shutdown();
            return T3D_ERR_FAIL;
        }

        mToken = UUID::generate().toString();
        mHandshaked = false;
        mDisconnected = false;

        mProcess = T3D_NEW Process();
        String cmdLine = "--ipc-port " + std::to_string(port) + " --ipc-token " + mToken;
        ret = mProcess->start(mExePath, cmdLine);
        if (T3D_FAILED(ret))
        {
            T3D_LOG_ERROR(LOG_TAG_IMGCONV_IPC, "Failed to start process: %s %s", mExePath.c_str(), cmdLine.c_str());
            shutdown();
            return ret;
        }

        auto startTime = std::chrono::steady_clock::now();
        while (!mHandshaked && !mDisconnected)
        {
            poll();
            if (mHandshaked)
            {
                break;
            }

            // 检查子进程是否已异常退出
            if (mProcess != nullptr && mProcess->wait(0) == T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_IMGCONV_IPC, "imgconv exited prematurely during handshake (exitCode=%u)",
                    mProcess->getExitCode());
                shutdown();
                return T3D_ERR_FAIL;
            }

            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - startTime).count();
            if (elapsed >= static_cast<int64_t>(handshakeTimeoutMs))
            {
                T3D_LOG_ERROR(LOG_TAG_IMGCONV_IPC, "Handshake with imgconv timed out after %u ms", handshakeTimeoutMs);
                shutdown();
                return T3D_ERR_TIMEOUT;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        if (!mHandshaked)
        {
            T3D_LOG_ERROR(LOG_TAG_IMGCONV_IPC, "Handshake with imgconv failed !");
            shutdown();
            return T3D_ERR_FAIL;
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void ImgconvClient::onAccepted(TCPListener *listener, TCPConnection *connection)
    {
        if (mConnection != nullptr)
        {
            // 只允许一个认证连接，额外连接直接断开
            connection->disconnect();
            TCPConnection::destroyConnection(connection);
            return;
        }

        mConnection = connection;
        mConnection->setSendBufferCapacity(64 * 1024);
        mConnection->setRecvBufferCapacity(64 * 1024);

        mConnection->setOnRecvCallback(
            [this](TCPConnection *conn, uint32_t seq, const void *data, int32_t dataSize)
            {
                onRecv(conn, seq, data, dataSize);
            });

        mConnection->setOnDisconnectedCallback(
            [this](TCPConnection *conn, TResult result)
            {
                onDisconnected(conn, result);
            });
    }

    //--------------------------------------------------------------------------

    void ImgconvClient::onRecv(TCPConnection *connection, uint32_t seq, const void *data, int32_t dataSize)
    {
        Tools::ImgconvResponseBody rsp;
        if (!rsp.ParseFromArray(data, dataSize))
        {
            T3D_LOG_ERROR(LOG_TAG_IMGCONV_IPC, "Failed to parse ImgconvResponseBody (seq=%u, size=%d)", seq, dataSize);
            return;
        }

        switch (rsp.message_id())
        {
        case Tools::IMGCONV_MSGID_HELLO:
            {
                const auto &hello = rsp.hello();
                if (hello.token() != mToken)
                {
                    T3D_LOG_ERROR(LOG_TAG_IMGCONV_IPC, "Invalid token in MsgHello ! expected=%s actual=%s",
                        mToken.c_str(), hello.token().c_str());
                    connection->disconnect();
                    mDisconnected = true;
                    return;
                }
                mHandshaked = true;
                T3D_LOG_INFO(LOG_TAG_IMGCONV_IPC, "Connected to imgconv (version=%s, pid=%llu)",
                    hello.version().c_str(), static_cast<unsigned long long>(hello.pid()));
            }
            break;

        case Tools::IMGCONV_MSGID_PROGRESS:
            {
                auto it = mPendingRequests.find(seq);
                if (it != mPendingRequests.end() && it->second.onProgress)
                {
                    it->second.onProgress(rsp.progress());
                }
            }
            break;

        case Tools::IMGCONV_MSGID_LOG:
            {
                auto it = mPendingRequests.find(seq);
                if (it != mPendingRequests.end())
                {
                    it->second.report.logs.push_back(rsp.log());
                    if (it->second.onLog)
                    {
                        it->second.onLog(rsp.log());
                    }
                }
            }
            break;

        case Tools::IMGCONV_MSGID_RESULT:
            {
                auto it = mPendingRequests.find(seq);
                if (it != mPendingRequests.end())
                {
                    it->second.report.results.push_back(rsp.result());
                    if (it->second.onResult)
                    {
                        it->second.onResult(rsp.result());
                    }
                }
            }
            break;

        case Tools::IMGCONV_MSGID_DONE:
            {
                auto it = mPendingRequests.find(seq);
                if (it != mPendingRequests.end())
                {
                    it->second.report.done = rsp.done();
                    it->second.completed = true;
                    if (it->second.onDone)
                    {
                        it->second.onDone(it->second.report);
                    }
                }
            }
            break;

        default:
            break;
        }
    }

    //--------------------------------------------------------------------------

    void ImgconvClient::onDisconnected(TCPConnection *connection, TResult result)
    {
        if (connection == mConnection)
        {
            mDisconnected = true;
            mHandshaked = false;
            TCPConnection::destroyConnection(mConnection);
            mConnection = nullptr;
        }
        else
        {
            TCPConnection::destroyConnection(connection);
        }
    }

    //--------------------------------------------------------------------------

    TResult ImgconvClient::sendMessage(uint32_t seq, const Tools::ImgconvRequestBody &req)
    {
        if (mConnection == nullptr || !mConnection->isConnected())
        {
            return T3D_ERR_FAIL;
        }

        int32_t byteSize = static_cast<int32_t>(req.ByteSizeLong());
        if (static_cast<size_t>(byteSize) > mSendScratch.size())
        {
            mSendScratch.resize(static_cast<size_t>(byteSize));
        }

        if (!req.SerializeToArray(mSendScratch.data(), byteSize))
        {
            return T3D_ERR_FAIL;
        }

        return mConnection->send(seq, mSendScratch.data(), byteSize, false);
    }

    //--------------------------------------------------------------------------

    TResult ImgconvClient::importAsync(
        const ImgconvArgs &args,
        const OnImgconvDone &onDone,
        const OnImgconvProgress &onProgress,
        const OnImgconvLog &onLog,
        const OnImgconvResult &onResult)
    {
        TResult ret = ensureStarted(mExePath);
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        uint32_t seq = mNextSeq++;
        PendingRequest pending;
        pending.seq = seq;
        pending.onProgress = onProgress;
        pending.onLog = onLog;
        pending.onResult = onResult;
        pending.onDone = onDone;
        mPendingRequests[seq] = pending;

        Tools::ImgconvRequestBody req;
        req.set_message_id(Tools::IMGCONV_MSGID_IMPORT);
        auto *imp = req.mutable_import_req();
        for (const auto &a : args.args)
        {
            imp->add_args(a);
        }

        ret = sendMessage(seq, req);
        if (T3D_FAILED(ret))
        {
            mPendingRequests.erase(seq);
            return ret;
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ImgconvClient::importSync(
        const ImgconvArgs &args,
        ImgconvBatchReport &outReport,
        const OnImgconvProgress &onProgress,
        const OnImgconvLog &onLog,
        uint32_t timeoutMs)
    {
        TResult ret = ensureStarted(mExePath);
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        uint32_t seq = mNextSeq++;
        PendingRequest pending;
        pending.seq = seq;
        pending.onProgress = onProgress;
        pending.onLog = onLog;
        mPendingRequests[seq] = pending;

        Tools::ImgconvRequestBody req;
        req.set_message_id(Tools::IMGCONV_MSGID_IMPORT);
        auto *imp = req.mutable_import_req();
        for (const auto &a : args.args)
        {
            imp->add_args(a);
        }

        ret = sendMessage(seq, req);
        if (T3D_FAILED(ret))
        {
            mPendingRequests.erase(seq);
            return ret;
        }

        auto startTime = std::chrono::steady_clock::now();
        while (true)
        {
            poll();

            auto it = mPendingRequests.find(seq);
            if (it != mPendingRequests.end() && it->second.completed)
            {
                outReport = std::move(it->second.report);
                mPendingRequests.erase(it);
                return static_cast<TResult>(outReport.done.result());
            }

            if (mDisconnected || mConnection == nullptr)
            {
                mPendingRequests.erase(seq);
                T3D_LOG_ERROR(LOG_TAG_IMGCONV_IPC, "Connection closed before MsgDone received (seq=%u)", seq);
                return T3D_ERR_FAIL;
            }

            if (mProcess != nullptr && mProcess->wait(0) == T3D_OK)
            {
                // 再跑一次 poll 以防最后的数据包刚到达 socket 缓冲
                poll();
                auto itFinal = mPendingRequests.find(seq);
                if (itFinal != mPendingRequests.end() && itFinal->second.completed)
                {
                    outReport = std::move(itFinal->second.report);
                    mPendingRequests.erase(itFinal);
                    return static_cast<TResult>(outReport.done.result());
                }
                mPendingRequests.erase(seq);
                T3D_LOG_ERROR(LOG_TAG_IMGCONV_IPC, "imgconv process exited unexpectedly (exitCode=%u)",
                    mProcess->getExitCode());
                return T3D_ERR_FAIL;
            }

            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - startTime).count();
            if (elapsed >= static_cast<int64_t>(timeoutMs))
            {
                mPendingRequests.erase(seq);
                T3D_LOG_ERROR(LOG_TAG_IMGCONV_IPC, "importSync timed out after %u ms (seq=%u)", timeoutMs, seq);
                return T3D_ERR_TIMEOUT;
            }
        }
    }

    //--------------------------------------------------------------------------

    void ImgconvClient::poll()
    {
        TCPListener::poll();
        TCPConnection::poll();

        // 清理已完成的异步请求
        for (auto it = mPendingRequests.begin(); it != mPendingRequests.end(); )
        {
            if (it->second.completed && it->second.onDone != nullptr)
            {
                it = mPendingRequests.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    //--------------------------------------------------------------------------

    void ImgconvClient::shutdown()
    {
        if (mConnection != nullptr && mConnection->isConnected() && mHandshaked)
        {
            Tools::ImgconvRequestBody req;
            req.set_message_id(Tools::IMGCONV_MSGID_SHUTDOWN);
            req.mutable_shutdown();
            sendMessage(mNextSeq++, req);

            // 给 socket 发送缓冲一点时间把 Shutdown 发出去
            for (int i = 0; i < 10 && mConnection != nullptr && mConnection->hasPendingSendData(); ++i)
            {
                TCPConnection::poll();
            }
        }

        if (mConnection != nullptr)
        {
            mConnection->disconnect();
            TCPConnection::destroyConnection(mConnection);
            mConnection = nullptr;
        }
        TCPConnection::destroyAll();

        if (mListener != nullptr)
        {
            mListener->stop();
            T3D_SAFE_DELETE(mListener);
        }

        if (mProcess != nullptr)
        {
            if (mProcess->wait(2000) != T3D_OK)
            {
                mProcess->terminate(0);
            }
            T3D_SAFE_DELETE(mProcess);
        }

        mHandshaked = false;
        mDisconnected = false;
        mPendingRequests.clear();
    }
}
