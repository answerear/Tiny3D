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

#include "../TinyEditor/EditorPrerequisites.h"
#include "proto/NetPackage.pb.h"


namespace Tiny3D
{
    NS_BEGIN(Launcher)
    
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

    TResult NetworkManager::startup()
    {
        TResult ret = T3D_OK;

        do
        {
            if (mTCPListener == nullptr)
            {
                mTCPListener = new TCPListener();

                mTCPListener->setOnAcceptedCallback(
                    [this](TCPListener *listener, TCPConnection *connection)
                    {
                        connection->setOnConnectedCallbak(
                            [this](TCPConnection *connection, TResult result)
                            {
                                onConnected(connection, result);
                            });

                        connection->setOnRecvCallback(
                            [this](TCPConnection *connection, uint32_t seq, const void *data, int32_t dataSize)
                            {
                                return onRecv(connection, seq, data, dataSize);
                            });

                        connection->setOnSendCallback(
                            [this](TCPConnection *connection, uint32_t seq, const void *data, int32_t dataSize)
                            {
                                return onSend(connection, seq, data, dataSize);
                            });

                        connection->setOnExceptionCallback(
                            [this](TCPConnection *connection, TResult result)
                            {
                                return onException(connection, result);
                            });

                        connection->setOnDisconnectedCallback(
                            [this](TCPConnection *connection, TResult result)
                            {
                                onDisconnected(connection, result);
                            });
                        
                        enqueue(connection);
                    });
            }

            uint16_t port = 5327;
            ret = mTCPListener->listen(port);
            if (T3D_FAILED(ret))
            {
                break;
            }

            T3D_LOG_INFO(LOG_TAG_LAUNCHER, "Started listening socket (port:%d) ...", port);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void NetworkManager::poll()
    {
        TCPListener::poll();
        TCPConnection::poll();
    }

    //--------------------------------------------------------------------------

    void NetworkManager::shutdown()
    {
        closeAllConnections();
        
        if (mTCPListener != nullptr)
        {
            T3D_LOG_INFO(LOG_TAG_LAUNCHER, "Stop listening socket (port:5327)");
            mTCPListener->stop();
            delete mTCPListener;
            mTCPListener = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    void NetworkManager::enqueue(TCPConnection *connection)
    {
        auto itr = std::find_if(
            mTCPConnections.begin(),
            mTCPConnections.end(),
            [connection](TCPConnection *conn)
            {
                return (connection == conn);
            });

        if (itr != mTCPConnections.end())
        {
            return;
        }

        mTCPConnections.emplace_back(connection);
    }

    //--------------------------------------------------------------------------

    void NetworkManager::enqueue(TCPConnection *connection, const String &path, const String &name)
    {
        EditorInstance editorInst = {path, name};
        mClientConnections.emplace(connection, editorInst);
        mEditorInstances.emplace(editorInst, connection);
    }

    //--------------------------------------------------------------------------

    void NetworkManager::dequeue(TCPConnection *connection)
    {
        auto itr = std::find_if(
            mTCPConnections.begin(),
            mTCPConnections.end(),
            [connection](TCPConnection *conn)
            {
                return (connection == conn);
            });

        if (itr == mTCPConnections.end())
        {
            return;
        }

        auto itSock = mClientConnections.find(connection);
        if (itSock == mClientConnections.end())
        {
            return;
        }

        EditorInstance editorInst = {itSock->second.path, itSock->second.name};
        mEditorInstances.erase(editorInst);
    }
    
    //--------------------------------------------------------------------------

    void NetworkManager::closeAllConnections()
    {
        for (auto connection : mTCPConnections)
        {
            connection->disconnect();
            T3D_SAFE_DELETE(connection);
        }

        mTCPConnections.clear();
        mClientConnections.clear();
        mEditorInstances.clear();
    }

    //--------------------------------------------------------------------------

    void NetworkManager::onConnected(TCPConnection *connection, TResult result)
    {
        T3D_LOG_INFO(LOG_TAG_LAUNCHER, "Remote TinyEditor [%s:%d] connected !", connection->getPeerName().c_str(), connection->getPeerPort());
    }

    //--------------------------------------------------------------------------

    void NetworkManager::onDisconnected(TCPConnection *connection, TResult result)
    {
        T3D_LOG_INFO(LOG_TAG_LAUNCHER, "Remote TinyEditor [%s:%d] disconnected !",
            connection->getPeerName().c_str(), connection->getPeerPort());
    }

    //--------------------------------------------------------------------------

    TResult NetworkManager::onRecv(TCPConnection *connection, uint32_t seq, const void *data, int32_t dataSize)
    {
        Editor::NetRequestBody req;
        
        if (req.ParseFromArray(data, dataSize))
        {
            switch (req.message_id())
            {
            case Editor::MessageID::MSGID_HELLO:
                {
                    T3D_LOG_INFO(LOG_TAG_LAUNCHER, "Recv hello seq [%u]", seq);
                
                    // 心跳
                    Editor::NetResponseBody rsp;
                    rsp.set_message_id(Editor::MessageID::MSGID_HELLO);
                    rsp.mutable_hello();
                    int32_t rspDataSize = static_cast<int32_t>(rsp.ByteSizeLong());
                    if (rsp.SerializeToArray(mSendBuffer, rspDataSize))
                    {
                        connection->send(seq, mSendBuffer, rspDataSize, false);
                    }
                    else
                    {
                        T3D_LOG_ERROR(LOG_TAG_EDITOR, "Pack hello failed !");
                    }
                }
                break;
            case Editor::MessageID::MSGID_CREATE_PROJECT:
                {
                    // 创建工程
                    const auto &msg = req.create_project();
                    enqueue(connection, msg.path(), msg.name());

                    // 回包
                    Editor::NetResponseBody rsp;
                    rsp.set_message_id(Editor::MessageID::MSGID_CREATE_PROJECT);
                    auto rspMsg = rsp.mutable_create_project();
                    rspMsg->set_result(0);
                    int32_t rspDataSize = static_cast<int32_t>(rsp.ByteSizeLong());
                    if (rsp.SerializeToArray(mSendBuffer, rspDataSize))
                    {
                        connection->send(seq, mSendBuffer, rspDataSize, false);
                    }
                    else
                    {
                        T3D_LOG_ERROR(LOG_TAG_EDITOR, "Pack create project failed !");
                    }
                }
                break;
            case Editor::MessageID::MSGID_OPEN_PROJECT:
                {
                    // 打开工程
                }
                break;
            }
        }
        
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

    NS_END
}
