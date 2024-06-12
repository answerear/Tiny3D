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

#ifndef __T3D_SOCKET_TYPE_H__
#define __T3D_SOCKET_TYPE_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DMacro.h"


#if defined (T3D_OS_WINDOWS)
    #include <WinSock.h>
        //    #include <WinSock2.h>

    namespace Tiny3D
    {
        #define T3DGetError()               WSAGetLastError()
        #define T3DCloseSocket(s)           closesocket(s)
        #define T3DIOCtlSocket(s, c, a)     ioctlsocket(s, c, a)

        #define T3D_EAGAIN                  WSAEWOULDBLOCK      // Resource temporarily unavailable. Nonblocking socket
        #define T3D_EINPROGRESS             WSAEINPROGRESS      // Operation now in progress. Blocking socket
        #define T3D_ENOBUFS                 WSAENOBUFS          // No buffer space available.
        #define T3D_EINTR                   WSAEINTR            // Interrupted function call.   
        #define T3D_ETIMEDOUT               WSAETIMEDOUT        // Connection timed out.
        #define T3D_EHOSTNOTFOUND           WSAHOST_NOT_FOUND   // 
        // #define INVALID_SOCKET           INVALID_SOCKET
        // #define SOCKET_ERROR         SOCKET_ERROR
        
        typedef int socklen_t; 
    }
    
#else
    #include <sys/time.h>
    #include <stddef.h>
    #include <unistd.h>
    #include <stdlib.h>
    #include <sys/wait.h>
    #include <sys/Socket.h>
    #include <netinet/in.h>
    #include <sys/ioctl.h>
    #include <netdb.h>
    #include <sys/errno.h>
    #include <arpa/inet.h>

    typedef int             SOCKET;
    typedef sockaddr_in     SOCKADDR_IN;
    typedef sockaddr        SOCKADDR;

    #define T3DGetError()           errno
        
    #define T3DCloseSocket(s)       close(s)
    #define T3DIOCtlSocket(s, c, a) ioctl(s, c, a)

    #define T3D_EAGAIN              EAGAIN
    #define T3D_EINPROGRESS         EINPROGRESS
    #define T3D_ENOBUFS             ENOBUFS
    #define T3D_EINTR               EINTR
    #define T3D_ETIMEDOUT           ETIMEDOUT
    #define T3D_EHOSTNOTFOUND       133
    #define INVALID_SOCKET      (-1)
    #define SOCKET_ERROR            (-1)
#endif
 

#endif  /*__T3D_SOCKET_TYPE_H__*/