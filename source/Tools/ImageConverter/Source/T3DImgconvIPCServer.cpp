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


#include "T3DImgconvIPCServer.h"
#include "T3DImageConverterApp.h"
#include "T3DImageConverterCommand.h"
#include "ImageConverter.pb.h"
#include <chrono>
#include <thread>
#include <vector>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ImgconvIPCServer::ImgconvIPCServer(ImageConverterApp *app, uint16_t port, const String &token)
        : mApp(app)
        , mPort(port)
        , mToken(token)
    {
    }

    //--------------------------------------------------------------------------

    ImgconvIPCServer::~ImgconvIPCServer() = default;

    //--------------------------------------------------------------------------

    int32_t ImgconvIPCServer::run()
    {
        TCPConnection *conn = T3D_NEW TCPConnection();
        conn->setSendBufferCapacity(128 * 1024);
        conn->setRecvBufferCapacity(128 * 1024);

        bool connected = false;
        bool finished = false;
        bool shutdownRequested = false;
        bool hasError = false;
        std::vector<uint8_t> sendScratch(64 * 1024);

        auto sendRsp = [&](uint32_t seq, const Tools::ImgconvResponseBody &rsp)
        {
            if (conn == nullptr || !conn->isConnected())
            {
                return;
            }
            int32_t byteSize = static_cast<int32_t>(rsp.ByteSizeLong());
            if (static_cast<size_t>(byteSize) > sendScratch.size())
            {
                sendScratch.resize(static_cast<size_t>(byteSize));
            }
            if (rsp.SerializeToArray(sendScratch.data(), byteSize))
            {
                conn->send(seq, sendScratch.data(), byteSize, false);
            }
        };

        conn->setOnConnectedCallbak(
            [&](TCPConnection *c, TResult result)
            {
                if (T3D_FAILED(result))
                {
                    hasError = true;
                    finished = true;
                    return;
                }
                connected = true;

                Tools::ImgconvResponseBody rsp;
                rsp.set_message_id(Tools::IMGCONV_MSGID_HELLO);
                auto *hello = rsp.mutable_hello();
                hello->set_token(mToken);
                hello->set_version(IMGCONV_VERSION_STR);
                hello->set_pid(0);
                sendRsp(0, rsp);
            });

        conn->setOnDisconnectedCallback(
            [&](TCPConnection *c, TResult result)
            {
                if (!shutdownRequested)
                {
                    hasError = true;
                }
                finished = true;
            });

        conn->setOnExceptionCallback(
            [&](TCPConnection *c, TResult result)
            {
                hasError = true;
                finished = true;
            });

        conn->setOnRecvCallback(
            [&](TCPConnection *c, uint32_t seq, const void *data, int32_t dataSize)
            {
                Tools::ImgconvRequestBody req;
                if (!req.ParseFromArray(data, dataSize))
                {
                    IMGCONV_LOG_ERROR("Failed to parse ImgconvRequestBody (seq=%u, size=%d)", seq, dataSize);
                    return;
                }

                switch (req.message_id())
                {
                case Tools::IMGCONV_MSGID_IMPORT:
                {
                    const auto &imp = req.import_req();
                    StringArray args;
                    args.reserve(static_cast<size_t>(imp.args_size()));
                    for (int i = 0; i < imp.args_size(); ++i)
                    {
                        args.push_back(imp.args(i));
                    }

                    ImageConverterOptions options;
                    ImageConverterCommand cmd;
                    String parseError;
                    if (!cmd.parse(args, options, &parseError))
                    {
                        Tools::ImgconvResponseBody logRsp;
                        logRsp.set_message_id(Tools::IMGCONV_MSGID_LOG);
                        logRsp.mutable_log()->set_level("error");
                        logRsp.mutable_log()->set_message(parseError);
                        sendRsp(seq, logRsp);

                        Tools::ImgconvResponseBody doneRsp;
                        doneRsp.set_message_id(Tools::IMGCONV_MSGID_DONE);
                        auto *done = doneRsp.mutable_done();
                        done->set_result(T3D_ERR_INVALID_PARAM);
                        done->set_total(0);
                        done->set_succeeded(0);
                        done->set_skipped(0);
                        done->set_failed(1);
                        sendRsp(seq, doneRsp);
                        return;
                    }

                    ImageBatchCallbacks cbs;
                    cbs.onProgress = [&](uint32_t current, uint32_t total, const String &stage, const String &currentFile)
                    {
                        Tools::ImgconvResponseBody progRsp;
                        progRsp.set_message_id(Tools::IMGCONV_MSGID_PROGRESS);
                        auto *prog = progRsp.mutable_progress();
                        prog->set_index(current);
                        prog->set_total(total);
                        prog->set_stage(stage);
                        prog->set_path(currentFile);
                        sendRsp(seq, progRsp);
                    };

                    cbs.onLog = [&](const String &level, const String &text)
                    {
                        Tools::ImgconvResponseBody logRsp;
                        logRsp.set_message_id(Tools::IMGCONV_MSGID_LOG);
                        logRsp.mutable_log()->set_level(level);
                        logRsp.mutable_log()->set_message(text);
                        sendRsp(seq, logRsp);
                    };

                    cbs.onItemResult = [&](const ImportItemRecord &rec)
                    {
                        Tools::ImgconvResponseBody resRsp;
                        resRsp.set_message_id(Tools::IMGCONV_MSGID_RESULT);
                        auto *res = resRsp.mutable_result();
                        if (!rec.success)
                        {
                            res->set_status("failed");
                        }
                        else if (rec.skipped)
                        {
                            res->set_status("skipped");
                        }
                        else
                        {
                            res->set_status("ok");
                        }
                        res->set_input(rec.srcPath);
                        res->set_output(rec.dstPath);
                        if (options.isGeneratingMeta)
                        {
                            res->set_meta(rec.dstPath + ".meta");
                        }
                        res->set_uuid(rec.uuid);
                        res->set_width(rec.width);
                        res->set_height(rec.height);
                        res->set_mip_levels(rec.mipLevels);
                        res->set_premultiplied(rec.premultipliedAlpha);
                        res->set_error(rec.errorMessage);
                        sendRsp(seq, resRsp);
                    };

                    std::vector<ImportItemRecord> records;
                    uint64_t totalElapsedMs = 0;
                    bool allOk = mApp->processBatch(options, records, totalElapsedMs, &cbs);

                    uint32_t okCount = 0;
                    uint32_t skipCount = 0;
                    uint32_t failCount = 0;
                    for (const auto &r : records)
                    {
                        if (!r.success)
                        {
                            ++failCount;
                        }
                        else if (r.skipped)
                        {
                            ++skipCount;
                        }
                        else
                        {
                            ++okCount;
                        }
                    }

                    Tools::ImgconvResponseBody doneRsp;
                    doneRsp.set_message_id(Tools::IMGCONV_MSGID_DONE);
                    auto *done = doneRsp.mutable_done();
                    done->set_result(allOk ? T3D_OK : T3D_ERR_FAIL);
                    done->set_total(static_cast<uint32_t>(records.size()));
                    done->set_succeeded(okCount);
                    done->set_skipped(skipCount);
                    done->set_failed(failCount);
                    sendRsp(seq, doneRsp);
                }
                break;

                case Tools::IMGCONV_MSGID_SHUTDOWN:
                    shutdownRequested = true;
                    break;

                default:
                    break;
                }
            });

        if (T3D_FAILED(conn->connect("127.0.0.1", mPort)))
        {
            IMGCONV_LOG_ERROR("Failed to initiate TCP connection to 127.0.0.1:%u", static_cast<uint32_t>(mPort));
            TCPConnection::destroyConnection(conn);
            TCPConnection::destroyAll();
            return 2;
        }

        while (!finished)
        {
            TCPConnection::poll();

            if (shutdownRequested)
            {
                // 确保所有待发送响应帧已写入 socket 后再关闭退出
                while (conn->isConnected() && conn->hasPendingSendData())
                {
                    TCPConnection::poll();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
                conn->disconnect();
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }

        TCPConnection::destroyConnection(conn);
        TCPConnection::destroyAll();
        return hasError ? 2 : 0;
    }
}
