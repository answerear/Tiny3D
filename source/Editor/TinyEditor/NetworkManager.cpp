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

#include "../TinyLauncher/LauncherPrerequisites.h"


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
                    [](TCPConnection *connection, TResult result)
                    {
                        if (T3D_SUCCEEDED(result))
                        {
                            T3D_LOG_INFO(LOG_TAG_EDITOR, "Connected TinyLauncher !");
                        }
                        else
                        {
                            T3D_LOG_INFO(LOG_TAG_EDITOR, "Connected error !");
                        }
                    });

                mConnection->setOnRecvCallback(
                    [](TCPConnection *connection, uint32_t seq, const void *data, int32_t dataSize)
                    {
                        return T3D_OK;
                    });

                mConnection->setOnSendCallback(
                    [](TCPConnection *connection, uint32_t seq, const void *data, int32_t dataSize)
                    {
                        return T3D_OK;
                    });

                mConnection->setOnExceptionCallback(
                    [](TCPConnection *connection, TResult result)
                    {});

                mConnection->setOnDisconnectedCallback(
                    [](TCPConnection *connection)
                    {
                        T3D_LOG_INFO(LOG_TAG_EDITOR, "Disconnected TinyLauncher !");
                    });
            }
            
            ret = mConnection->connect(addr, port);
            if (T3D_FAILED(ret))
            {
                break;
            }
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


    //--------------------------------------------------------------------------

    NS_END
}

