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
        mSocket->close();
        T3D_SAFE_DELETE(mSocket);
    }

    //--------------------------------------------------------------------------

    TResult NetworkManager::startup(const String &addr, uint16_t port)
    {
        TResult ret = T3D_OK;

        do
        {
            mSocket = new Socket();
            if (!mSocket->create(Socket::Protocol::kTCP))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Create socket failed ! ERROR [%u]", mSocket->getErrorCode());
                break;
            }
            
            if (!mSocket->setNonBlocking())
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Set socket non-blocking failed ! ERROR [%u]", mSocket->getErrorCode());
                break;
            }

            if (!mSocket->connect(addr, port))
            {
                T3D_LOG_ERROR(LOG_TAG_EDITOR, "Connect failed ! ERROR [%u]", mSocket->getErrorCode());
                break;
            }
            
            mSocket->setConnectedCallback([this](Socket *socket, bool isOK){ onConnected(socket, isOK); });
            mSocket->setRecvCallback([this](Socket *socket) { return onRecv(socket); });
            mSocket->setSendCallback([this](Socket *socket) { return onSend(socket); });
            mSocket->setExceptionCallback([this](Socket *socket) { return onException(socket); });
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void NetworkManager::poll()
    {
        Socket::pollEvents(100);
    }

    //--------------------------------------------------------------------------

    void NetworkManager::shutdown()
    {
        if (mSocket != nullptr)
        {
            mSocket->close();
        }

        T3D_SAFE_DELETE(mSocket);
    }

    //--------------------------------------------------------------------------

    void NetworkManager::onConnected(Socket *socket, bool isOK)
    {
        
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

    NS_END
}

