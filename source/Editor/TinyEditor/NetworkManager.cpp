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


#include "NetworkManager.h"
#include "proto/NetPackage.pb.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------

    uint32_t NetworkManager::MONITOR_INTERVAL = 1000;
    uint32_t NetworkManager::HELLO_INTERVAL = 5 * 1000;
    
    //--------------------------------------------------------------------------

    NetworkManager::NetworkManager()
    {
        
    }

    //--------------------------------------------------------------------------

    NetworkManager::~NetworkManager()
    {
        shutdown();
    }

    //--------------------------------------------------------------------------

    TResult NetworkManager::startup(const String &addr, uint16_t port)
    {
        TResult ret = T3D_OK;

        do
        {
            if (mConnection == nullptr)
            {
                mConnection = new TCPConnection();

                mConnection->setOnConnectedCallbak(
                    [this](TCPConnection *connection, TResult result)
                    {
                        onConnected(connection, result);
                    });

                mConnection->setOnRecvCallback(
                    [this](TCPConnection *connection, uint32_t seq, const void *data, int32_t dataSize)
                    {
                        return onRecv(connection, seq, data, dataSize);
                    });

                mConnection->setOnSendCallback(
                    [this](TCPConnection *connection, uint32_t seq, const void *data, int32_t dataSize)
                    {
                        return onSend(connection, seq, data, dataSize);
                    });

                mConnection->setOnExceptionCallback(
                    [this](TCPConnection *connection, TResult result)
                    {
                        return onException(connection, result);
                    });

                mConnection->setOnDisconnectedCallback(
                    [this](TCPConnection *connection, TResult result)
                    {
                        onDisconnected(connection, result);
                    });
            }
            
            ret = mConnection->connect(addr, port);
            if (T3D_FAILED(ret))
            {
                break;
            }

            mRemoteAddr = addr;
            mRemotePort = port;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void NetworkManager::poll()
    {
        TCPConnection::poll();
    }

    //--------------------------------------------------------------------------

    void NetworkManager::shutdown()
    {
        stopHelloTimer();
        
        if (mConnection != nullptr)
        {
            mConnection->disconnect();
        }

        T3D_SAFE_DELETE(mConnection);
    }

    //--------------------------------------------------------------------------

    void NetworkManager::onConnected(TCPConnection *connection, TResult result)
    {
        if (T3D_SUCCEEDED(result))
        {
            T3D_LOG_INFO(LOG_TAG_EDITOR, "Connected TinyLauncher !");

            // 启动 hello 定时器
            startHelloTimer();
        }
        else
        {
            T3D_LOG_INFO(LOG_TAG_EDITOR, "Connect failed ! ERROR [%d]", result);

            // 连接失败，说明 launcher 可能没运行，启动定时检测
            startMonitorTimer();
        }
    }

    //--------------------------------------------------------------------------

    TResult NetworkManager::onRecv(TCPConnection *connection, uint32_t seq, const void *data, int32_t dataSize)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NetworkManager::onSend(TCPConnection *connection, uint32_t seq, const void *data, int32_t dataSize)
    {
        T3D_LOG_INFO(LOG_TAG_EDITOR, "Send : seq [%u], dataSize [%d]", seq, dataSize);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NetworkManager::onException(TCPConnection *connection, TResult result)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void NetworkManager::onDisconnected(TCPConnection *connection, TResult result)
    {
        T3D_LOG_INFO(LOG_TAG_EDITOR, "Disconnected TinyLauncher ! ERROR [%d]", result);
    }

    //--------------------------------------------------------------------------

    void NetworkManager::startMonitorTimer()
    {
        T3D_ASSERT(mMonitorTimerID == T3D_INVALID_TIMER_ID, "Monitor in network must be not set !");
        mMonitorTimerID = T3D_TIMER_MGR.startTimer(MONITOR_INTERVAL, true,
            [this](ID timerID, uint32_t dt)
            {
                T3D_ASSERT(timerID == mMonitorTimerID, "Must be monitor timer !");
                
                if (Process::isProcessRunning("TinyLauncher.exe"))
                {
                    // 进程已经启动了
                    stopMonitorTimer();
                    TResult ret = mConnection->connect(mRemoteAddr, mRemotePort);
                    T3D_LOG_INFO(LOG_TAG_EDITOR, "TinyLauncher is running, start connecting ... ERROR [%d]", ret);
                }
            });
    }

    //--------------------------------------------------------------------------

    void NetworkManager::stopMonitorTimer()
    {
        if (mMonitorTimerID != T3D_INVALID_TIMER_ID)
        {
            T3D_TIMER_MGR.stopTimer(mMonitorTimerID);
            mMonitorTimerID = T3D_INVALID_TIMER_ID;
        }
    }

    //--------------------------------------------------------------------------

    void NetworkManager::startHelloTimer()
    {
        T3D_ASSERT(mHelloTimerID == T3D_INVALID_TIMER_ID, "Hello timer id must be invalid !");
        mHelloTimerID = T3D_TIMER_MGR.startTimer(HELLO_INTERVAL, true,
            [this](ID timerID, uint32_t dt)
            {
                T3D_ASSERT(timerID == mHelloTimerID, "Must be hello timer !");
                NetRequestBody req;
                req.set_message_id(MSGID_HELLO);
                req.mutable_hello();
                int32_t reqDataSize = static_cast<int32_t>(req.ByteSizeLong());
                if (req.SerializeToArray(mSendBuffer, reqDataSize))
                {
                    mConnection->send(mSeq++, mSendBuffer, reqDataSize, false);
                }
                
            });
    }

    //--------------------------------------------------------------------------

    void NetworkManager::stopHelloTimer()
    {
        if (mHelloTimerID != T3D_INVALID_TIMER_ID)
        {
            T3D_TIMER_MGR.stopTimer(mHelloTimerID);
            mHelloTimerID = T3D_INVALID_TIMER_ID;
        }
    }

    //--------------------------------------------------------------------------

    TResult NetworkManager::createProject(const String &path, const String &name)
    {
        TResult ret = T3D_OK;

        NetRequestBody req;
        req.set_message_id(MSGID_CREATE_PROJECT);
        auto pkg = req.mutable_create_project();
        pkg->set_path(path);
        pkg->set_name(name);
        pkg->set_result(0);
        int32_t reqDataSize = static_cast<int32_t>(req.ByteSizeLong());
        if (req.SerializeToArray(mSendBuffer, reqDataSize))
        {
            ret = mConnection->send(mSeq++, mSendBuffer, reqDataSize, true);
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult NetworkManager::openProject(const String &path, const String &name)
    {
        TResult ret = T3D_OK;

        NetRequestBody req;
        req.set_message_id(MSGID_OPEN_PROJECT);
        auto pkg = req.mutable_open_project();
        pkg->set_path(path);
        pkg->set_name(name);
        pkg->set_result(0);
        int32_t reqDataSize = static_cast<int32_t>(req.ByteSizeLong());
        if (req.SerializeToArray(mSendBuffer, reqDataSize))
        {
            ret = mConnection->send(mSeq++, mSendBuffer, reqDataSize, true);
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    NS_END
}

