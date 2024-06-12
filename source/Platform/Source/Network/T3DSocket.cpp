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
    
    bool Socket::create(Protocol eProtocol /* = Protocol::kTCP */)
    {
        if (0 == msCount++)
        {
            if (!startup(1, 1))
                return false;
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

        bool ret = (INVALID_SOCKET != mSocket);
        if (ret)
        {
            enqueue(this);
        }

        return ret;
    }

    //--------------------------------------------------------------------------
    
    bool Socket::close()
    {
        if (INVALID_SOCKET == mSocket)
        {
            return false;
        }

        dequeue(this);

        ::shutdown(mSocket, 2);

        T3DCloseSocket(mSocket);
        mSocket = INVALID_SOCKET;
        mState = State::kDisconnected;
        
        if (0 == --msCount)
        {
            cleanup();
        }

        return true;
    }

    //--------------------------------------------------------------------------
    
    bool Socket::attach(SOCKET socket)
    {
        mSocket = socket;
        enqueue(this);
        return true;
    }

    //--------------------------------------------------------------------------
    
    SOCKET Socket::detach()
    {
        dequeue(this);
        SOCKET socket = mSocket;
        mSocket = INVALID_SOCKET;
        return socket;
    }

    //--------------------------------------------------------------------------
    
    bool Socket::connect(const String &strHostAddress, uint16_t unHostPort)
    {
        if (strHostAddress.empty() || strHostAddress == "")
        {
            return false;
        }

        SOCKADDR_IN addrRemote;
        hostent *lpszHost = 0;

        memset(&addrRemote, 0, sizeof(addrRemote));
        addrRemote.sin_family = AF_INET;
        addrRemote.sin_port = ::htons(unHostPort);
        addrRemote.sin_addr.s_addr = inet_addr(strHostAddress.c_str());

        if (INADDR_NONE == addrRemote.sin_addr.s_addr)
        {
            lpszHost = ::gethostbyname(strHostAddress.c_str());
            if (NULL == lpszHost)
                return false;

            memcpy(&addrRemote.sin_addr, lpszHost->h_addr_list[0], lpszHost->h_length);
        }

        if (::connect(mSocket, (SOCKADDR *)&addrRemote, sizeof(addrRemote)) == SOCKET_ERROR
            && T3DGetError() != T3D_EAGAIN)
        {
            return false;
        }

        mState = State::kConnecting;

        return true;
    }

    //--------------------------------------------------------------------------
    
    bool Socket::connect(unsigned long ulHostIP, uint16_t unHostPort)
    {
        if (ulHostIP == 0)
        {
            return false;
        }

        SOCKADDR_IN addrRemote;

        memset(&addrRemote, 0, sizeof(addrRemote));
        addrRemote.sin_family = AF_INET;
        addrRemote.sin_port = ::htons(unHostPort);
        addrRemote.sin_addr.s_addr = ulHostIP;

        if (::connect(mSocket, (SOCKADDR *)&addrRemote, sizeof(addrRemote)) == SOCKET_ERROR)
        {
            return false;
        }

        mState = State::kConnecting;
        
        return true;
    }

    //--------------------------------------------------------------------------
    
    bool Socket::listen(int32_t nConnectionBacklog /* = 5 */)
    {
        if (::listen(mSocket, nConnectionBacklog) == SOCKET_ERROR)
        {
            return false;
        }

        mState = State::kListening;

        return true;
    }

    //--------------------------------------------------------------------------
    
    bool Socket::bind(uint16_t unSocketPort, const String &strSocketAddress /* = "" */)
    {
        if (INVALID_SOCKET == mSocket)
        {
            return false;
        }

        SOCKADDR_IN addrLocal;
        addrLocal.sin_family = AF_INET;
        addrLocal.sin_port = ::htons(unSocketPort);

        if (strSocketAddress.empty() || strSocketAddress == "")
        {
            addrLocal.sin_addr.s_addr = inet_addr(strSocketAddress.c_str());
        }
        else
        {
            addrLocal.sin_addr.s_addr = ::htonl(INADDR_ANY);
        }

        if (::bind(mSocket, (SOCKADDR *)&addrLocal, sizeof(addrLocal)) == SOCKET_ERROR)
        {
            return false;
        }

        return true;
    }

    //--------------------------------------------------------------------------
    
    bool Socket::accept(Socket &sockClient)
    {
        SOCKADDR_IN addr;
        int32_t len = sizeof(addr);
        SOCKET socket;

        socket = ::accept(mSocket, (SOCKADDR *)&addr, (socklen_t *)&len);
        if (INVALID_SOCKET == socket || 0 == socket)
        {
            return false;
        }

        ++msCount;

        sockClient.attach(socket);
        sockClient.mState = State::kConnected;
        sockClient.callOnConnected(true);
        
        callOnAccepted(&sockClient);

        return true;
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

    // bool Socket::canWrite() const
    // {
    //     fd_set writefds;
    //     struct timeval timeout;
    //
    //     timeout.tv_sec=0;
    //     timeout.tv_usec=0;
    //     FD_ZERO(&writefds);
    //     FD_SET(mSocket, &writefds);
    //
    //     int ret = ::select(FD_SETSIZE, NULL, &writefds, NULL, &timeout);
    //     if(ret > 0 && FD_ISSET(mSocket, &writefds))
    //         return true;
    //     
    //     return false;
    // }
    //
    // bool Socket::canRead() const
    // {
    //     fd_set readfds;
    //     struct timeval timeout;
    //
    //     timeout.tv_sec = 0;
    //     timeout.tv_usec = 0;
    //     FD_ZERO(&readfds);
    //     FD_SET(mSocket, &readfds);
    //
    //     int32_t ret = ::select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);
    //     if (ret > 0 && FD_ISSET(mSocket, &readfds))
    //         return true;
    //     
    //     return false;
    // }
    //
    // bool Socket::hasExcept() const
    // {
    //     fd_set exceptfds;
    //     struct timeval timeout;
    //
    //     timeout.tv_sec=0;
    //     timeout.tv_usec=0;
    //     FD_ZERO(&exceptfds);
    //     FD_SET(mSocket, &exceptfds);
    //
    //     int ret = select(FD_SETSIZE, NULL, NULL, &exceptfds, &timeout);
    //     if(ret > 0 && FD_ISSET(mSocket, &exceptfds))
    //         return true;
    //     
    //     return false;
    // }

    //--------------------------------------------------------------------------
    
    bool Socket::setNonBlocking()
    {
        /* set to nonblocking mode */
        unsigned long arg;
        arg = 1;

        if (T3DIOCtlSocket(mSocket, FIONBIO, &arg) == SOCKET_ERROR)
        {
            return false;
        }
        
        return true;
    }

    //--------------------------------------------------------------------------
    
    void Socket::reset()
    {
        mState = State::kDisconnected;
        mSocket = INVALID_SOCKET;
    }

    //--------------------------------------------------------------------------
    
    bool Socket::setSendBufferSize(int32_t nSize)
    {
        int32_t ret;
        ret = ::setsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, (char*)&nSize, sizeof(nSize));
        if (ret == SOCKET_ERROR) 
            return false;

        return true;
    }

    //--------------------------------------------------------------------------
    
    bool Socket::setRecvBufferSize(int32_t nSize)
    {
        int32_t ret;
        ret = ::setsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, (char*)&nSize, sizeof(nSize));
        if (ret == SOCKET_ERROR)
            return false;

        return true;
    }

    //--------------------------------------------------------------------------
    
    bool Socket::setReuseAddress(bool bReuse)
    {
#ifndef T3D_OS_WINDOWS
        /* only useful in linux */
        int32_t opt = 0;
        uint32_t len = sizeof(opt);

        if (bReuse) 
            opt = 1;

        if (::setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, len) == SOCKET_ERROR)
        {
            return false;
        }
        
        return true;
#else
        return true;
#endif
    }

    //--------------------------------------------------------------------------
    
    bool Socket::getLocalName(String &rSockName, uint16_t &rSockPort) const
    {
        SOCKADDR_IN addrLocal;
        socklen_t len = sizeof(addrLocal);
        if (::getsockname(mSocket, (SOCKADDR*)&addrLocal, &len) == SOCKET_ERROR)
            return false;

        char *tmp = ::inet_ntoa(addrLocal.sin_addr);
        if (!tmp) 
            return false;
        
        rSockName = tmp;
        rSockPort = ::ntohs(addrLocal.sin_port);

        return true;
    }

    //--------------------------------------------------------------------------
    
    bool Socket::getPeerName(String &rPeerName, uint16_t &rPeerPort) const
    {
        sockaddr_in addrRemote;
        int len = sizeof(addrRemote);
        if (::getpeername(mSocket, (sockaddr *)&addrRemote, (socklen_t *)&len) == SOCKET_ERROR)
            return false;

        char *tmp = ::inet_ntoa(addrRemote.sin_addr);
        if (!tmp) 
            return false;
        
        rPeerName = tmp;
        rPeerPort = ::ntohs(addrRemote.sin_port);

        return true;
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
    
    bool Socket::startup(int32_t nVersionH, int32_t nVersionL)
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
            return false;
        }

        /* version error */
        if (LOBYTE(wsaData.wVersion)!= nVersionL || HIBYTE(wsaData.wVersion)!= nVersionH ) 
        {
            ::WSACleanup();
            return false;
        }
#endif
        return true;
    }

    //--------------------------------------------------------------------------
    
    bool Socket::cleanup()
    {
#ifdef T3D_OS_WINDOWS   
        ::WSACleanup();
#endif
        return true;
    }
    
    //--------------------------------------------------------------------------

    void Socket::enqueue(Socket *socket)
    {
        msSockets.emplace_back(socket);
    }

    //--------------------------------------------------------------------------

    void Socket::dequeue(Socket *socket)
    {
        auto it = std::find_if(
            msSockets.begin(),
            msSockets.end(),
            [socket](Socket *sock)
            {
                if (sock->mSocket == socket->mSocket)
                {
                    return true;
                }
                return false;
            });
        msSockets.erase(it);
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
                callOnConnected(false);
            }
            else if (FD_ISSET(mSocket, &writefds))
            {
                if (getSocketError() != 0)
                {
                    ret = T3D_ERR_SOCKET_ERROR;
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
 
