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
    NS_BEGIN(Launcher)
    
    //--------------------------------------------------------------------------

    NetworkManager::NetworkManager()
    {
        
    }

    //--------------------------------------------------------------------------

    NetworkManager::~NetworkManager()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult NetworkManager::startup()
    {
        TResult ret = T3D_OK;

        do
        {
            mListenSocket = new Socket();

            // 创建监听 socket
            if (!mListenSocket->create(Socket::Protocol::kTCP))
            {
                T3D_LOG_ERROR(LOG_TAG_LAUNCHER, "Create listen socket failed ! ERROR [%u]", mListenSocket->getErrorCode());
                ret = T3D_ERR_FAIL;
                break;
            }

            // 设置非阻塞式 socket
            if (!mListenSocket->setNonBlocking())
            {
                T3D_LOG_ERROR(LOG_TAG_LAUNCHER, "Set socket non-blocking failed ! ERROR [%u]", mListenSocket->getErrorCode());
                ret = T3D_ERR_FAIL;
                break;
            }

            mListenSocket->setAcceptedCallback(
                [this](Socket *sockListen, Socket *sockClient)
                {
                    onAccepted(sockListen, sockClient);
                });

            // 绑定端口
            if (!mListenSocket->bind(5327, "127.0.0.1"))
            {
                T3D_LOG_ERROR(LOG_TAG_LAUNCHER, "Bind socket ip & port failed ! ERROR [%u]", mListenSocket->getErrorCode());
                ret = T3D_ERR_FAIL;
                break;
            }

            // 开始监听 editor 接入
            if (!mListenSocket->listen())
            {
                T3D_LOG_ERROR(LOG_TAG_LAUNCHER, "Start listening failed ! ERROR [%u]", mListenSocket->getErrorCode());
                ret = T3D_ERR_FAIL;
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void NetworkManager::poll()
    {
        if (mCurrentClientSock == nullptr)
        {
            mCurrentClientSock = new Socket();
        }
        
        mListenSocket->accept(*mCurrentClientSock);
        
        Socket::pollEvents(100);
    }

    //--------------------------------------------------------------------------

    void NetworkManager::shutdown()
    {
        closeAllSockets();
        
        mListenSocket->close();
        T3D_SAFE_DELETE(mListenSocket);
    }

    //--------------------------------------------------------------------------

    void NetworkManager::onAccepted(Socket *sockListen, Socket *sockClient)
    {
        do
        {
            if (!sockClient->setNonBlocking())
            {
                T3D_LOG_ERROR(LOG_TAG_LAUNCHER, "Set socket non-blocking failed when accept client socket ! ERROR [%u]", mCurrentClientSock->getErrorCode());
                break;
            }

            sockClient->setRecvCallback([this](Socket *socket) { return onRecv(socket); });
            sockClient->setSendCallback([this](Socket *socket) { return onSend(socket); });
            sockClient->setExceptionCallback([this](Socket *socket) { return onException(socket); });

            enqueue(sockClient);
        } while (false);

        mCurrentClientSock = nullptr;
    }

    //--------------------------------------------------------------------------

    TResult NetworkManager::onRecv(Socket *socket)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NetworkManager::onSend(Socket *socket)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NetworkManager::onException(Socket *socket)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void NetworkManager::enqueue(Socket *socket)
    {
        auto itr = std::find_if(
            mSockets.begin(),
            mSockets.end(),
            [socket](Socket *sock)
            {
                return (socket == sock);
            });

        if (itr != mSockets.end())
        {
            return;
        }

        mSockets.emplace_back(socket);
    }

    //--------------------------------------------------------------------------

    void NetworkManager::enqueue(Socket *socket, const String &path, const String &name)
    {
        EditorInstance editorInst = {path, name};
        mClientSockets.emplace(socket, editorInst);
        mEditorInstances.emplace(editorInst, socket);
    }

    //--------------------------------------------------------------------------

    void NetworkManager::dequeue(Socket *socket)
    {
        auto itr = std::find_if(
            mSockets.begin(),
            mSockets.end(),
            [socket](Socket *sock)
            {
                return (socket == sock);
            });

        if (itr == mSockets.end())
        {
            return;
        }

        auto itSock = mClientSockets.find(socket);
        if (itSock == mClientSockets.end())
        {
            return;
        }

        EditorInstance editorInst = {itSock->second.path, itSock->second.name};
        mEditorInstances.erase(editorInst);
    }
    
    //--------------------------------------------------------------------------

    void NetworkManager::closeAllSockets()
    {
        for (auto sock : mSockets)
        {
            sock->close();
            T3D_SAFE_DELETE(sock);
        }

        mSockets.clear();
        mClientSockets.clear();
        mEditorInstances.clear();
    }

    //--------------------------------------------------------------------------

    NS_END
}
