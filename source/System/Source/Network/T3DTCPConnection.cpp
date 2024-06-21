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


#include "Network/T3DTCPConnection.h"
#include "T3DPlatformErrorDef.h"
#include "Network/T3DSocket.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    #define T3D_NET_PACKAGE_MAGIC           12345
    #define T3D_NET_PACKAGE_HEADER_SIZE     8 
    
    struct NetPackage
    {
        uint16_t magic {T3D_NET_PACKAGE_MAGIC};
        uint16_t length {0};
        uint32_t seq {~0U};
        char data[1];
    };
    
    //--------------------------------------------------------------------------

    TCPListener::TCPListeners TCPListener::msListeners;
    
    //--------------------------------------------------------------------------

    void TCPListener::poll()
    {
        for (auto listener : msListeners)
        {
            listener->doAccepted();
        }
    }

    //--------------------------------------------------------------------------

    TCPListener::~TCPListener()
    {
        T3D_SAFE_DELETE(mSockClient);

        if (mSockListener != nullptr)
        {
            mSockListener->close();
        }
        T3D_SAFE_DELETE(mSockListener);
    }

    //--------------------------------------------------------------------------

    TResult TCPListener::listen(uint16_t port, const String &addr, int32_t backlog)
    {
        if (mIsListening)
        {
            return T3D_OK;
        }
        
        if (mSockListener == nullptr)
        {
            mSockListener = new Socket();
            mSockListener->setOnAcceptedCallback(
                [this](Socket *sockListener, Socket *sockClient)
                {
                    mSockClient = nullptr;
                    TCPConnection *connection = new TCPConnection(sockClient);
                    callOnAccepted(connection);
                });
        }

        TResult ret = T3D_OK;
        
        do
        {
            ret = mSockListener->create(Socket::Protocol::kTCP);
            if (T3D_FAILED(ret))
            {
                uint32_t error = mSockListener->getErrorCode();
                break;
            }

            ret = mSockListener->setNonBlocking();
            if (T3D_FAILED(ret))
            {
                uint32_t error = mSockListener->getErrorCode();
                break;
            }
            
            ret = mSockListener->bind(port, addr);
            if (T3D_FAILED(ret))
            {
                uint32_t error = mSockListener->getErrorCode();
                break;
            }
        
            ret = mSockListener->listen(backlog);
            if (T3D_FAILED(ret))
            {
                uint32_t error = mSockListener->getErrorCode();
                break;
            }

            mIsListening = true;
            msListeners.emplace_back(this);
            callOnStarted();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult TCPListener::stop()
    {
        T3D_ASSERT(mSockListener != nullptr, "Socket is nulltpr !");
        if (!mIsListening)
        {
            return T3D_OK;
        }
        
        TResult ret = mSockListener->close();
        if (T3D_SUCCEEDED(ret))
        {
            auto it = std::find_if(
                msListeners.begin(),
                msListeners.end(),
                [this](TCPListener *listener)
                {
                    return listener == this;
                });
            msListeners.erase(it);
            callOnStopped();
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    void TCPListener::doAccepted()
    {
        if (mSockClient == nullptr)
        {
            mSockClient = new Socket();
        }
        
        mSockListener->accept(*mSockClient);
    }

    //--------------------------------------------------------------------------

    TCPConnection::TCPConnections TCPConnection::msConnections;
    
    //--------------------------------------------------------------------------

    void TCPConnection::poll()
    {
        Socket::pollEvents(100);
    }

    //--------------------------------------------------------------------------

    void TCPConnection::closeAllConnections()
    {
        for (auto connection : msConnections)
        {
            connection->disconnect();
            T3D_SAFE_DELETE(connection);
        }

        msConnections.clear();
    }

    //--------------------------------------------------------------------------

    TResult TCPConnection::enqueue(TCPConnection *connection)
    {
        msConnections.emplace(connection);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult TCPConnection::dequeue(TCPConnection *connection)
    {
        msConnections.erase(connection);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TCPConnection::TCPConnection()
    {
        mSocket = new Socket();
        setupCallbacks();
        
        enqueue(this);
    }
    
    //--------------------------------------------------------------------------

    TCPConnection::TCPConnection(Socket *socket)
        : mSocket(socket)
    {
        setupCallbacks();
        if (mSocket->isConnected())
        {
            mSocket->getPeerName(mPeerName, mPeerPort);
        }
        enqueue(this);
    }
    
    //--------------------------------------------------------------------------

    TCPConnection::~TCPConnection()
    {
        if (mSocket != nullptr)
        {
            mSocket->close();
        }
        T3D_SAFE_DELETE(mSocket);
        dequeue(this);
    }

    //--------------------------------------------------------------------------

    void TCPConnection::setupCallbacks()
    {
        mSocket->setOnConnectedCallback(
            [this](Socket *socket, TResult result)
            {
                stopConnectingTimer();
                mSocket->getPeerName(mPeerName, mPeerPort);
                if (T3D_FAILED(result))
                {
                    disconnect();
                }
                callOnConnected(result);
            });

        mSocket->setOnDisconnectedCallback(
            [this](Socket *socket, TResult result)
            {
                callOnDisconnected(result);
            });

        mSocket->setOnRecvCallback(
            [this](Socket *socket)
            {
                return onRecv(socket);
            });

        mSocket->setOnSendCallback(
            [this](Socket *socket)
            {
                return onSend(socket);
            });

        mSocket->setOnExceptionCallback(
            [this](Socket *socket)
            {
                return T3D_OK;
            });
    }

    //--------------------------------------------------------------------------

    TResult TCPConnection::connect(const String &addr, uint16_t port, uint32_t timeout)
    {
        TResult ret = T3D_OK;

        do
        {
            ret = mSocket->create(Socket::Protocol::kTCP);
            if (T3D_FAILED(ret))
            {
                break;
            }

            ret = mSocket->setNonBlocking();
            if (T3D_FAILED(ret))
            {
                disconnect();
                break;
            }
            
            ret = mSocket->connect(addr, port);
            if (T3D_FAILED(ret))
            {
                disconnect();
                break;
            }

            startConnectingTimer(timeout);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void TCPConnection::startConnectingTimer(uint32_t timeout)
    {
        if (mConnectTimerID == T3D_INVALID_TIMER_ID)
        {
            mConnectTimerID = T3D_TIMER_MGR.startTimer(timeout, false,
                [this](uint32_t timerID, uint32_t dt)
                {
                    if (timerID == mConnectTimerID)
                    {
                        mConnectTimerID = T3D_INVALID_TIMER_ID;
                    }
                });
        }
    }

    //--------------------------------------------------------------------------

    void TCPConnection::stopConnectingTimer()
    {
        if (mConnectTimerID != T3D_INVALID_TIMER_ID)
        {
            T3D_TIMER_MGR.stopTimer(mConnectTimerID);
            mConnectTimerID = T3D_INVALID_TIMER_ID;
        }
    }

    //--------------------------------------------------------------------------
    
    TResult TCPConnection::send(uint32_t seq, const void *data, int32_t dataSize, bool cacheWhenConnecting)
    {
        TResult ret = T3D_OK;
        do
        {
            if (mSocket == nullptr)
            {
                ret = T3D_ERR_SOCKET_INVALID;
                break;
            }

            if (!mSocket->isConnected() && !mSocket->isConnecting())
            {
                // 没连接上
                ret = T3D_ERR_SOCKET_NOT_CONNECTED;
                break;
            }

            ensureSendBuffer();
            
            int32_t pkgSize = dataSize + T3D_NET_PACKAGE_HEADER_SIZE;
            NetPackage *package = (NetPackage *)(mSendBuffer + mSendBufferSize);
            
            if (mSocket->isConnecting())
            {
                // 正在连接中
                if (cacheWhenConnecting)
                {
                    // 需要缓存，等连接上再发送
                    if (mSendBufferSize + pkgSize > mSendBufferCapacity)
                    {
                        // 缓冲区满了
                        ret = T3D_ERR_SOCKET_NO_SENDBUF;
                        break;
                    }

                    package->magic = Socket::htons(T3D_NET_PACKAGE_MAGIC);
                    package->seq = Socket::htonl(seq);
                    package->length = Socket::htons(static_cast<uint16_t>(pkgSize));
                    memcpy(package->data, data, dataSize);
                    mSendBufferSize += pkgSize;
                }
                else
                {
                    // 不缓存，直接报错
                    ret = T3D_ERR_SOCKET_NOT_CONNECTED;
                }
                break;
            }
            
            // 连接上，可以正常发包

            // 先把发送缓冲区剩余的数据发送
            ret = sendRemainData();
            if (T3D_FAILED(ret))
            {
                break;
            }

            package->magic = Socket::htons(T3D_NET_PACKAGE_MAGIC);
            package->seq = Socket::htonl(seq);
            package->length = Socket::htons(static_cast<uint16_t>(pkgSize));
            memcpy(package->data, data, dataSize);
            
            if (mSendBufferSize > 0)
            {
                // 剩余的还没发完，缓存
                mSendBufferSize += pkgSize;
                break;
            }

            // 剩余的发送完了，直接发送新来的数据
            int32_t sendLen = mSocket->send(mSendBuffer, pkgSize);
            
            if (sendLen == 0)
            {
                // 一点数据都没发送
                if (mSocket->getSocketError() == T3D_ENOBUFS && mSendBufferSize + pkgSize <= mSendBufferCapacity)
                {
                    // socket 缓冲区满了，应用层缓存一下
                    mSendBufferSize += pkgSize;
                }
                else
                {
                    // errors
                    ret = T3D_ERR_SOCKET_NO_SENDBUF;
                }
                break;
            }

            if (sendLen < pkgSize)
            {
                // 发送了部分，整个包缓存起来，调整发送位置
                mSendBufferSize += pkgSize;
                mSendBufferBegin += sendLen;
                break;
            }
            
            // 发送了完整的包，回调通知发送成功
            callOnSend(seq, data, dataSize);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult TCPConnection::sendRemainData()
    {
        TResult ret = T3D_OK;

        do
        {
            if (mSendBufferSize == 0)
            {
                // 没有数据需要发送
                break;
            }

            uint8_t *buf = mSendBuffer + mSendBufferBegin;
            int32_t length = mSendBufferSize - mSendBufferBegin;
            int32_t sendLen = mSocket->send(buf, length);
            if (sendLen == 0)
            {
                // 出错了
                if (mSocket->getSocketError() == T3D_ENOBUFS && mSendBufferSize < mSendBufferCapacity)
                {
                    // 继续缓存
                }
                else
                {
                    ret = T3D_ERR_SOCKET_NO_SENDBUF;
                }
                break;
            }

            // 发送回调，告知上层发送了哪些包
            int32_t offset = 0;
            NetPackage *package = (NetPackage *)(mSendBuffer);
            int32_t pkgSize = Socket::ntohs(package->length);
            int32_t remainLen = sendLen + mSendBufferBegin;
                
            while (remainLen > 0 && remainLen >= pkgSize)
            {
                T3D_ASSERT(Socket::ntohs(package->magic) == T3D_NET_PACKAGE_MAGIC, "Invalid net package magic !");
                // 发送回调，告知上层发送了哪些包
                callOnSend(package->seq, package->data, package->length-T3D_NET_PACKAGE_HEADER_SIZE);
                offset += pkgSize;
                remainLen -= pkgSize;
                package = (NetPackage *)(mSendBuffer + offset);
                pkgSize = Socket::ntohs(package->length);
            }
            
            if (sendLen < length)
            {
                // 发送了部分
                
                T3D_ASSERT(remainLen > 0, "The length of remain data greater than zero !");
                T3D_ASSERT(Socket::ntohs(package->magic) == T3D_NET_PACKAGE_MAGIC, "Invalid net package magic !");
                
                uint8_t *buf = mSendBuffer + offset;
                memmove(mSendBuffer, buf, remainLen);
                mSendBufferBegin = remainLen;
                mSendBufferSize -= offset;
                
                break;
            }
            
            // 全部发完
            T3D_ASSERT(sendLen == length, "Send all data !");
            
            mSendBufferBegin = 0;
            mSendBufferSize = 0;
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult TCPConnection::disconnect()
    {
        stopConnectingTimer();
        
        if (mSocket != nullptr)
        {
            // 断开连接
            mSocket->close();

            // 清楚所有缓存带处理数据
            mSendBufferBegin = 0;
            mSendBufferSize = 0;
            mRecvBufferSize = 0;
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult TCPConnection::onRecv(Socket *socket)
    {
        TResult ret = T3D_OK;

        do
        {
            if (mSocket == nullptr)
            {
                ret = T3D_ERR_SOCKET_INVALID;
                break;
            }
            
            ensureRecvBuffer();

            uint8_t *buf = mRecvBuffer + mRecvBufferSize;
            int32_t bufSize = mRecvBufferCapacity - mRecvBufferSize;
            int32_t recvLen = mSocket->recv(buf, bufSize);

            if (recvLen == 0)
            {
                // error
                if (mSocket->getErrorCode() != T3D_EAGAIN)
                {
                    ret = T3D_ERR_SOCKET_ERROR;
                }
                break;
            }

            mRecvBufferSize += recvLen;
            
            NetPackage *package = (NetPackage *)mRecvBuffer;
            
            if (mRecvBufferSize < T3D_NET_PACKAGE_HEADER_SIZE)
            {
                // 长度不够包头，只能等等
                break;
            }

            int32_t pkgSize = package->length = Socket::ntohs(package->length);
            if (mRecvBufferSize < pkgSize)
            {
                // 长度不够一个完整包，只能等等
                break;
            }

            int32_t length = mRecvBufferSize;
            int32_t offset = 0;
            while (length >= pkgSize)
            {
                package->magic = Socket::ntohs(package->magic);
                T3D_ASSERT(package->magic == T3D_NET_PACKAGE_MAGIC, "Invalid net package magic !");
                package->seq = Socket::ntohl(package->seq);
                callOnRecv(package->seq, package->data, package->length-T3D_NET_PACKAGE_HEADER_SIZE);
                length -= pkgSize;
                offset += pkgSize;
                package = (NetPackage *)(mRecvBuffer + offset);
                package->length = Socket::ntohs(package->length);
                pkgSize = package->length;
            }

            buf = mRecvBuffer + offset;
            memmove(mRecvBuffer, buf, length);
            mRecvBufferSize = length;
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult TCPConnection::onSend(Socket *socket)
    {
        return sendRemainData();
    }

    //--------------------------------------------------------------------------

    void TCPConnection::ensureSendBuffer()
    {
        if (mIsSendBufDirty)
        {
            T3D_SAFE_DELETE_ARRAY(mSendBuffer);
            mSendBuffer = new uint8_t[mSendBufferCapacity];
            mSendBufferSize = 0;
            mIsSendBufDirty = false;
        }
    }

    //--------------------------------------------------------------------------

    void TCPConnection::ensureRecvBuffer()
    {
        if (mIsRecvBufDirty)
        {
            mRecvBuffer = new uint8_t[mRecvBufferCapacity];
            mRecvBufferSize = 0;
            mIsRecvBufDirty = false;
        }
    }

    //--------------------------------------------------------------------------
}