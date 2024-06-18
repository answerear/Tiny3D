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

#include "Network/T3DSocket.h"
#include "T3DPlatformErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    int32_t Socket::msCount = 0;
    Socket::Sockets Socket::msSockets;

    //--------------------------------------------------------------------------
    
    String Socket::getHostByName(const String &strDomain)
    {
        if (strDomain.empty() || strDomain == "")
            return String("");

        hostent *pHostEnt = ::gethostbyname(strDomain.c_str());
        if (pHostEnt != NULL)
        {
            if (pHostEnt->h_addr_list[0] != NULL)
            {
                return String(inet_ntoa(*((struct in_addr*)pHostEnt->h_addr_list[0])));
            }
        }

        return String("");
    }

    //--------------------------------------------------------------------------
    
    ulong_t Socket::DNS(const String &strDomain)
    {
        if (strDomain.empty() || strDomain == "")
            return INADDR_NONE;

        hostent *pHostEnt = ::gethostbyname(strDomain.c_str());
        if (pHostEnt != NULL)
        {
            if (pHostEnt->h_addr != NULL)
            {
                sockaddr_in address;
                struct in_addr *pin_addr =  ((struct in_addr *)pHostEnt->h_addr);
                address.sin_addr = *(pin_addr);
                ulong_t ip = address.sin_addr.s_addr;
                return ip;
            }
        }

        return INADDR_NONE;
    }

    //--------------------------------------------------------------------------
    
    bool Socket::isDomain(const String &strDomain)
    {
        if (strDomain.empty() || strDomain == "")
            return false;

        ulong_t ulHostIP = ::inet_addr(strDomain.c_str());

        return (ulHostIP == INADDR_NONE);
    }

    //--------------------------------------------------------------------------
    
    Socket::Socket()
        : mSocket(INVALID_SOCKET)
    {
        
    }

    //--------------------------------------------------------------------------
    
    Socket::~Socket()
    {
        close();
    }

    //--------------------------------------------------------------------------
    
    TResult Socket::create(Protocol eProtocol /* = Protocol::kTCP */)
    {
        TResult ret = T3D_OK;

        do
        {
            if (0 == msCount++)
            {
                ret = startup(1, 1);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }

            if (INVALID_SOCKET != mSocket)
            {
                close();
            }

            mState = State::kDisconnected;
        
            if (Protocol::kUDP == eProtocol)
            {
                mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            }
            else
            {
                mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            }

            if (INVALID_SOCKET == mSocket)
            {
                ret = T3D_ERR_SOCKET_CREATED;
                break;
            }
            
            ret = enqueue(this);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------
    
    TResult Socket::close()
    {
        TResult ret = T3D_OK;

        do
        {
            if (INVALID_SOCKET == mSocket)
            {
                ret = T3D_ERR_SOCKET_INVALID;
                break;
            }

            if (mState == State::kConnected)
            {
                ret = dequeue(this);
                if (T3D_FAILED(ret))
                {
                    break;
                }

                ::shutdown(mSocket, 2);

                T3DCloseSocket(mSocket);
                mSocket = INVALID_SOCKET;
                mState = State::kDisconnected;
        
                if (0 == --msCount)
                {
                    cleanup();
                }
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------
    
    TResult Socket::attach(SOCKET socket)
    {
        mSocket = socket;
        TResult ret = enqueue(this);
        if (T3D_FAILED(ret))
        {
            mSocket = INVALID_SOCKET;
        }
        return ret;
    }

    //--------------------------------------------------------------------------
    
    SOCKET Socket::detach()
    {
        SOCKET socket = INVALID_SOCKET;
        do
        {
            TResult ret = dequeue(this);
            if (T3D_FAILED(ret))
            {
                break;
            }
            socket = mSocket;
            mSocket = INVALID_SOCKET;
        } while (false);
        
        return socket;
    }

    //--------------------------------------------------------------------------
    
    TResult Socket::connect(const String &strHostAddress, uint16_t unHostPort)
    {
        if (strHostAddress.empty() || strHostAddress == "")
        {
            return T3D_ERR_INVALID_PARAM;
        }

        if (INVALID_SOCKET == mSocket)
        {
            return T3D_ERR_SOCKET_INVALID;
        }

        SOCKADDR_IN addrRemote;
        memset(&addrRemote, 0, sizeof(addrRemote));
        addrRemote.sin_family = AF_INET;
        addrRemote.sin_port = ::htons(unHostPort);
        addrRemote.sin_addr.s_addr = inet_addr(strHostAddress.c_str());

        if (INADDR_NONE == addrRemote.sin_addr.s_addr)
        {
            hostent *lpszHost = ::gethostbyname(strHostAddress.c_str());
            if (nullptr == lpszHost)
                return T3D_ERR_SOCKET_DNS;

            memcpy(&addrRemote.sin_addr, lpszHost->h_addr_list[0], lpszHost->h_length);
        }

        if (::connect(mSocket, (SOCKADDR *)&addrRemote, sizeof(addrRemote)) == SOCKET_ERROR
            && T3DGetError() != T3D_EAGAIN)
        {
            return T3D_ERR_SOCKET_ERROR;
        }

        mState = State::kConnecting;

        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult Socket::connect(unsigned long ulHostIP, uint16_t unHostPort)
    {
        if (ulHostIP == 0)
        {
            return T3D_ERR_INVALID_PARAM;
        }

        if (INVALID_SOCKET == mSocket)
        {
            return T3D_ERR_SOCKET_INVALID;
        }
        
        SOCKADDR_IN addrRemote;
        memset(&addrRemote, 0, sizeof(addrRemote));
        addrRemote.sin_family = AF_INET;
        addrRemote.sin_port = ::htons(unHostPort);
        addrRemote.sin_addr.s_addr = ulHostIP;

        if (::connect(mSocket, (SOCKADDR *)&addrRemote, sizeof(addrRemote)) == SOCKET_ERROR
            && T3DGetError() != T3D_EAGAIN)
        {
            return T3D_ERR_SOCKET_ERROR;
        }

        mState = State::kConnecting;
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult Socket::listen(int32_t nConnectionBacklog /* = 5 */)
    {
        if (mSocket == INVALID_SOCKET)
        {
            return T3D_ERR_SOCKET_INVALID;
        }
        
        if (::listen(mSocket, nConnectionBacklog) == SOCKET_ERROR)
        {
            return T3D_ERR_SOCKET_ERROR;
        }

        mState = State::kListening;

        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult Socket::bind(uint16_t unSocketPort, const String &strSocketAddress /* = "" */)
    {
        if (INVALID_SOCKET == mSocket)
        {
            return T3D_ERR_SOCKET_INVALID;
        }

        SOCKADDR_IN addrLocal;
        addrLocal.sin_family = AF_INET;
        addrLocal.sin_port = ::htons(unSocketPort);

        if (!strSocketAddress.empty())
        {
            addrLocal.sin_addr.s_addr = inet_addr(strSocketAddress.c_str());
        }
        else
        {
            addrLocal.sin_addr.s_addr = ::htonl(INADDR_ANY);
        }

        if (::bind(mSocket, (SOCKADDR *)&addrLocal, sizeof(addrLocal)) == SOCKET_ERROR)
        {
            return T3D_ERR_SOCKET_ERROR;
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult Socket::accept(Socket &sockClient)
    {
        SOCKADDR_IN addr;
        int32_t len = sizeof(addr);
        SOCKET socket = ::accept(mSocket, (SOCKADDR *)&addr, (socklen_t *)&len);
        if (INVALID_SOCKET == socket || 0 == socket)
        {
            return T3D_ERR_SOCKET_INVALID;
        }

        ++msCount;

        sockClient.attach(socket);
        sockClient.mState = State::kConnected;
        callOnAccepted(&sockClient);
        
        sockClient.callOnConnected(true);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    int32_t Socket::recv(uchar_t *pBuffer, int32_t nBufLen)
    {
        return ::recv(mSocket, (char*)pBuffer, nBufLen, 0);
    }

    //--------------------------------------------------------------------------
    
    int32_t Socket::send(uchar_t *pBuffer, int32_t nBufLen)
    {
        return ::send(mSocket, (char*)pBuffer, nBufLen, 0);
    }

    //--------------------------------------------------------------------------
    
    int32_t Socket::recvFrom(uchar_t *pBuffer, int32_t nBufLen, const String &strHostAddress, uint16_t unHostPort)
    {
        if (strHostAddress.empty() || strHostAddress == "")
            return -1;

        SOCKADDR_IN addrRemote;
        memset(&addrRemote, 0, sizeof(addrRemote));
        addrRemote.sin_family = AF_INET;
        addrRemote.sin_port = ::htons(unHostPort);
        addrRemote.sin_addr.s_addr = inet_addr(strHostAddress.c_str());

        socklen_t addrLen = sizeof(addrRemote);

        return ::recvfrom(mSocket, (char*)pBuffer, nBufLen, 0, (SOCKADDR*)&addrRemote, (socklen_t*)&addrLen);
    }

    //--------------------------------------------------------------------------
    
    int32_t Socket::sendTo(uchar_t *pBuffer, int32_t nBufLen, const String &strHostAddress, uint16_t unHostPort)
    {
        if (strHostAddress.empty() || strHostAddress == "")
            return -1;

        SOCKADDR_IN addrRemote;
        memset(&addrRemote, 0, sizeof(addrRemote));
        addrRemote.sin_family = AF_INET;
        addrRemote.sin_port = ::htons(unHostPort);
        addrRemote.sin_addr.s_addr = inet_addr(strHostAddress.c_str());

        return ::sendto(mSocket, (char*)pBuffer, nBufLen, 0, (SOCKADDR *)&addrRemote, sizeof(SOCKADDR_IN));
    }

    //--------------------------------------------------------------------------
    
    TResult Socket::setNonBlocking()
    {
        /* set to nonblocking mode */
        unsigned long arg;
        arg = 1;

        if (T3DIOCtlSocket(mSocket, FIONBIO, &arg) == SOCKET_ERROR)
        {
            return T3D_ERR_SOCKET_ERROR;
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    void Socket::reset()
    {
        if (mState != State::kDisconnected)
        {
            close();

            if (mState == State::kConnected)
            {
                callOnDisconnected();
            }
        }
        
        mState = State::kDisconnected;
        mSocket = INVALID_SOCKET;
    }

    //--------------------------------------------------------------------------
    
    TResult Socket::setSendBufferSize(int32_t nSize)
    {
        if (::setsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, (char*)&nSize, sizeof(nSize)) == SOCKET_ERROR) 
            return T3D_ERR_SOCKET_ERROR;

        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult Socket::setRecvBufferSize(int32_t nSize)
    {
        if (::setsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, (char*)&nSize, sizeof(nSize)) == SOCKET_ERROR)
            return T3D_ERR_SOCKET_ERROR;

        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult Socket::setReuseAddress(bool bReuse)
    {
#ifndef T3D_OS_WINDOWS
        /* only useful in linux */
        int32_t opt = 0;
        uint32_t len = sizeof(opt);

        if (bReuse) 
            opt = 1;

        if (::setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, len) == SOCKET_ERROR)
        {
            return T3D_ERR_SOCKET_ERROR;
        }
        
        return T3D_OK;
#else
        return T3D_OK;
#endif
    }

    //--------------------------------------------------------------------------
    
    TResult Socket::getLocalName(String &rSockName, uint16_t &rSockPort) const
    {
        SOCKADDR_IN addrLocal;
        socklen_t len = sizeof(addrLocal);
        if (::getsockname(mSocket, (SOCKADDR*)&addrLocal, &len) == SOCKET_ERROR)
            return T3D_ERR_SOCKET_ERROR;

        char *tmp = ::inet_ntoa(addrLocal.sin_addr);
        if (!tmp) 
            return T3D_ERR_SOCKET_ERROR;
        
        rSockName = tmp;
        rSockPort = ::ntohs(addrLocal.sin_port);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult Socket::getPeerName(String &rPeerName, uint16_t &rPeerPort) const
    {
        sockaddr_in addrRemote;
        int len = sizeof(addrRemote);
        if (::getpeername(mSocket, (sockaddr *)&addrRemote, (socklen_t *)&len) == SOCKET_ERROR)
            return T3D_ERR_SOCKET_ERROR;

        char *tmp = ::inet_ntoa(addrRemote.sin_addr);
        if (!tmp) 
            return T3D_ERR_SOCKET_ERROR;
        
        rPeerName = tmp;
        rPeerPort = ::ntohs(addrRemote.sin_port);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    uint32_t Socket::getErrorCode() const
    {
        return T3DGetError();
    }

    //--------------------------------------------------------------------------
    
    uint32_t Socket::getSocketError() const
    {
        socklen_t iSockErr, iSockErrLen = sizeof(iSockErr);
        int32_t ret = ::getsockopt(mSocket, SOL_SOCKET, SO_ERROR, (char*)&iSockErr, &iSockErrLen);
        if (ret == SOCKET_ERROR) 
            return iSockErr;

        return 0;
    }

    //--------------------------------------------------------------------------
    
    TResult Socket::startup(int32_t nVersionH, int32_t nVersionL)
    {
#ifdef T3D_OS_WINDOWS
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;

        wVersionRequested = MAKEWORD(nVersionH, nVersionL);
        err = ::WSAStartup(wVersionRequested, &wsaData);

        /* startup failed */
        if (err != 0)
        {
            ::WSACleanup();
            return T3D_ERR_SOCKET_STARTUP;
        }

        /* version error */
        if (LOBYTE(wsaData.wVersion)!= nVersionL || HIBYTE(wsaData.wVersion)!= nVersionH ) 
        {
            ::WSACleanup();
            return T3D_ERR_SOCKET_VERSION;
        }
#endif
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult Socket::cleanup()
    {
#ifdef T3D_OS_WINDOWS   
        ::WSACleanup();
#endif
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    TResult Socket::enqueue(Socket *socket)
    {
        auto it = std::find_if(
            msSockets.begin(),
            msSockets.end(),
            [socket](Socket *sock)
            {
                return (sock->mSocket == socket->mSocket);
            });
        if (it != msSockets.end())
        {
            return T3D_ERR_SOCKET_DUPLICATED;
        }
        msSockets.emplace_back(socket);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Socket::dequeue(Socket *socket)
    {
        auto it = std::find_if(
            msSockets.begin(),
            msSockets.end(),
            [socket](Socket *sock)
            {
                return (sock->mSocket == socket->mSocket);
            });
        if (it == msSockets.end())
        {
            return T3D_ERR_SOCKET_NOT_FOUND;
        }
        msSockets.erase(it);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void Socket::callOnAccepted(Socket *sockClient)
    {
        if (mOnAccepted != nullptr)
        {
            mOnAccepted(this, sockClient);
        }
    }

    //--------------------------------------------------------------------------

    void Socket::callOnConnected(bool isOK)
    {
        if (mOnConnected != nullptr)
        {
            mOnConnected(this, isOK);
        }
    }

    //--------------------------------------------------------------------------

    TResult Socket::callOnRecv()
    {
        if (mOnRecv != nullptr)
        {
            return mOnRecv(this);
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Socket::callOnSend()
    {
        if (mOnSend != nullptr)
        {
            return mOnSend(this);
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Socket::callOnException()
    {
        if (mOnException != nullptr)
        {
            return mOnException(this);
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void Socket::callOnDisconnected()
    {
        if (mOnDisconnected != nullptr)
        {
            mOnDisconnected(this);
        }
    }

    //--------------------------------------------------------------------------

    TResult Socket::pollEvents(uint32_t timeout)
    {
        fd_set readfds, writefds, errorfds;
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&errorfds);

        int32_t maxHandle = 0;
        int32_t handle = 0;
        bool useSelect = false;

        for (auto socket : msSockets)
        {
            if ((handle = socket->prepareFdSet(readfds, writefds, errorfds)) != (int32_t)INVALID_SOCKET)
            {
                if (handle > maxHandle)
                {
                    maxHandle = handle;
                }

                useSelect = true;
            }
        }

        TResult ret = T3D_OK;

        if (useSelect)
        {
            maxHandle += 1;
            int rval = select(maxHandle, &readfds, &writefds, &errorfds, timeout);
            if (rval < 0)
            {
                return T3D_ERR_TIMEOUT;
            }

            for (auto socket : msSockets)
            {
                ret = socket->handleEvent(readfds, writefds, errorfds);
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    int32_t Socket::prepareFdSet(fd_set &readfds, fd_set &writefds, fd_set &errorfds)
    {
        if (mSocket == INVALID_SOCKET)
        {
            return (int32_t)INVALID_SOCKET;
        }

        if (State::kConnecting == mState)
        {
            FD_SET(mSocket, &writefds);
            FD_SET(mSocket, &errorfds);
        }
        else if (State::kConnected == mState)
        {
            FD_SET(mSocket, &readfds);
            FD_SET(mSocket, &writefds);
            FD_SET(mSocket, &errorfds);
        }
        else
        {
            return (int32_t)INVALID_SOCKET;
        }
        
        return (int32_t)mSocket;
    }

    //--------------------------------------------------------------------------

    int32_t Socket::select(int32_t nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, uint32_t timeout)
    {
        struct timeval to;
        to.tv_sec = 0;
        to.tv_usec = (long)timeout;
        return ::select(nfds, readfds, writefds, errorfds, &to);
    }

    //--------------------------------------------------------------------------

    TResult Socket::handleEvent(fd_set &readfds, fd_set &writefds, fd_set &errorfds)
    {
        if (mSocket == INVALID_SOCKET)
        {
            return T3D_ERR_INVALID_POINTER;
        }
        
        TResult ret = T3D_OK;

        if (State::kConnecting == mState)
        {
            if (FD_ISSET(mSocket, &errorfds) && T3DGetError() != T3D_EINPROGRESS)
            {
                // 出错了
                ret = T3D_ERR_SOCKET_ERROR;
                mState = State::kDisconnected;
                callOnConnected(false);
            }
            else if (FD_ISSET(mSocket, &writefds))
            {
                if (getSocketError() != 0)
                {
                    ret = T3D_ERR_SOCKET_ERROR;
                    mState = State::kDisconnected;
                    callOnConnected(false);
                }
                else
                {
                    // 连接上
                    setNonBlocking();
                    mState = State::kConnected;
                    callOnConnected(true);
                }
            }
        }
        else if (State::kConnected == mState)
        {
            if (FD_ISSET(mSocket, &readfds))
            {
                // 可读数据
                ret = callOnRecv();
                if (T3D_FAILED(ret))
                {
                    callOnDisconnected();
                    close();
                    mState = State::kDisconnected;
                }
            }

            if (FD_ISSET(mSocket, &writefds))
            {
                // 可写数据
                ret = callOnSend();
                if (T3D_FAILED(ret))
                {
                    callOnDisconnected();
                    close();
                    mState = State::kDisconnected;
                }
            }
        }
        return ret;
    }

    //--------------------------------------------------------------------------
}
 
