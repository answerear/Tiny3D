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


namespace Tiny3D
{
    NS_BEGIN(Editor)

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
        }
        else
        {
            T3D_LOG_INFO(LOG_TAG_EDITOR, "Connect failed ! ERROR [%d]", result);

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
        T3D_ASSERT(mTimerID == T3D_INVALID_TIMER_ID, "Monitor in network must be not set !");
        mTimerID = T3D_TIMER_MGR.startTimer(1000, true,
            [this](ID timerID, uint32_t dt)
            {
                if (timerID == mTimerID)
                {
                    if (Process::isProcessRunning("TinyLauncher.exe"))
                    {
                        // 进程已经启动了
                        stopMonitorTimer();
                        TResult ret = mConnection->connect(mRemoteAddr, mRemotePort);
                        T3D_LOG_INFO(LOG_TAG_EDITOR, "TinyLauncher is running, start connecting ... ERROR [%d]", ret);
                    }
                }
            });
    }

    //--------------------------------------------------------------------------

    void NetworkManager::stopMonitorTimer()
    {
        if (mTimerID != T3D_INVALID_TIMER_ID)
        {
            T3D_TIMER_MGR.stopTimer(mTimerID);
            mTimerID = T3D_INVALID_TIMER_ID;
        }
    }

    //--------------------------------------------------------------------------

    NS_END
}

