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

#ifndef __T3D_SOCKER_H__
#define __T3D_SOCKER_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DNoncopyable.h"
#include "T3DPlatformErrorDef.h"
#include "T3DSocketType.h"
#include "Memory/T3DMemory.h"


namespace Tiny3D
{
    using OnAccepted = TFunction<void(Socket *sockListen, Socket *sockClient)>;
    using OnConnected = TFunction<void(Socket*, TResult)>;
    using OnRecv = TFunction<TResult(Socket*)>;
    using OnSend = TFunction<TResult(Socket*)>;
    using OnException = TFunction<TResult(Socket*)>;
    using OnDisconnected = TFunction<void(Socket*, TResult)>;
    
    class T3D_PLATFORM_API Socket : public Allocator, public Noncopyable
    {
        public:
        /**
         * @brief 套接字协议枚举
         */
        enum class Protocol : uint32_t
        {
            /// 未知协议
            kUnknown = 0,
            /// UDP协议
            kUDP,
            /// TCP协议
            kTCP
        };

        /**
         * 套接字状态
         */
        enum class State : uint32_t
        {
            /// 套接字关闭
            kDisconnected = 0,
            /// 监听中
            kListening,
            /// 连接中
            kConnecting,
            /// 连接上
            kConnected
        };

        /**
         * 轮训 socket 事件，检查是否可读、可写、发生错误异常
         * @param [in] timeout : 超时时间，单位：微秒
         * @return 调用成功返回 T3D_OK
         */
        static TResult pollEvents(uint32_t timeout);

        /**
         * @brief Constructor for Socket.
         */
        Socket();

        /**
         * @brief Destructor for Socket.
         */
        ~Socket() override;

        /**
         * @brief 创建套接字.
         * @param [in] eProtocol : 套接字协议类型，默认是TCP.
         * @return 函数调用成功返回 T3D_OK.  
         */
        TResult create(Protocol eProtocol = Protocol::kTCP);

        /**
         * 设置接受连接回调，有远端请求连接时候回调
         * @param [in] onAccepted : 回调对象 
         */
        void setOnAcceptedCallback(const OnAccepted &onAccepted) { mOnAccepted = onAccepted; }

        /**
         * 设置连接上回调，连接上远端时候回调
         * @param [in] onConnected : 回调对象 
         */
        void setOnConnectedCallback(const OnConnected &onConnected) { mOnConnected = onConnected; }

        /**
         * 设置接收数据回调，套接字有数据可读时回调
         * @param [in] onRecv : 回调对象
         */
        void setOnRecvCallback(const OnRecv &onRecv) { mOnRecv = onRecv; }

        /**
         * 设置发送数据回调，套接字可写时回调
         * @param [in] onSend : 回调对象
         */
        void setOnSendCallback(const OnSend &onSend) { mOnSend = onSend; }

        /**
         * 设置异常回调，套接字有异常发生的时候回调
         * @param [in] onException : 回调对象
         */
        void setOnExceptionCallback(const OnException &onException) { mOnException = onException; }

        /**
         * 设置套接字关闭回调，套接字被关闭的时候回调
         * @param [in] onDisconnected : 回调对象
         */
        void setOnDisconnectedCallback(const OnDisconnected &onDisconnected) { mOnDisconnected = onDisconnected; }

        /**
         * @brief 关闭套接字.
         * @return 函数调用成功返回 T3_OK.  
         */
        TResult close();

        /**
         * @brief 附加原始套接字.
         * @param [in] socket : 原始套接字描述符
         * @return 函数调用成功返回 T3_OK.
         */
        TResult attach(SOCKET socket);

        /**
         * @brief 分离出原始套接字.
         * @return 返回原始套接字描述符
         */
        SOCKET detach();

        /**
         * @brief 连接到指定IP和端口服务器.
         * @param [in] strHostAddress : IP地址
         * @param [in] unHostPort : 端口
         * @return 函数调用成功返回 T3D_OK.
         */
        TResult connect(const String &strHostAddress, uint16_t unHostPort);

        /**
         * @brief 连接到指定IP和端口服务器.
         * @param [in] ulHostIP : IP 地址
         * @param [in] unHostPort : 端口
         * @return 函数调用成功返回 T3D_OK.
         */
        TResult connect(unsigned long ulHostIP, uint16_t unHostPort);
        
        /**
         * @brief 监听连接请求.
         * @param [in] nConnectionBacklog : 等待队列最大值，Defaults to 5.
         * @return 函数调用成功返回true，否则返回false.  
         */
        TResult listen(int32_t nConnectionBacklog = 5);

        /**
         * @brief 绑定端口和IP.
         * @param [in] unSocketPort : 端口
         * @param [in] strSocketAddress : Defaults to "".
         * @return 函数调用成功返回 T3D_OK.
         */
        TResult bind(uint16_t unSocketPort, const String &strSocketAddress = "");

        /**
         * @brief 接受新连接.
         * @param [out] sockClient : 返回新套接字对象
         * @return 函数调用成功返回 T3D_OK.
         */
        TResult accept(Socket &sockClient);

        /**
         * @brief 接收数据.
         * @param [in] pBuffer : 数据缓冲区
         * @param [in] nBufLen : 缓冲区大小
         * @return 接收到的数据大小  
         */
        int32_t recv(uchar_t *pBuffer, int32_t nBufLen);

        /**
         * @brief 发送数据.
         * @param [in] pBuffer : 数据缓冲区
         * @param [in] nBufLen : 缓冲区大小
         * @return 发送的数据大小  
         */
        int32_t send(uchar_t *pBuffer, int32_t nBufLen);

        /**
         * @brief 接收数据.
         * @param [in] pBuffer : 数据缓冲区
         * @param [in] nBufLen : 缓冲区大小
         * @param [in] strHostAddress : 接收服务器IP地址
         * @param [in] unHostPort : 接收的服务器端口
         * @return 接收到的数据大小  
         */
        int32_t recvFrom(uchar_t *pBuffer, int32_t nBufLen, const String &strHostAddress, uint16_t unHostPort);

        /**
         * @brief 发送数据.
         * @param [in] pBuffer : 数据缓冲区
         * @param [in] nBufLen : 缓冲区大小
         * @param [in] strHostAddress : 发送到的服务器IP地址
         * @param [in] unHostPort : 发送到的服务器对应端口
         * @return 发送的数据大小  
         */
        int32_t sendTo(uchar_t *pBuffer, int32_t nBufLen, const String &strHostAddress, uint16_t unHostPort);

        /**
         * 是否连接上
         */
        bool isConnected() const { return mState == State::kConnected; }

        /**
         * 是否连接中
         */
        bool isConnecting() const { return mState == State::kConnecting; }

        /**
         * @brief 设置为非阻塞套接字.
         * @return 函数调用成功返回 T3D_OK.
         */
        TResult setNonBlocking();

        /**
         * @brief 重置套接字及其状态
         */
        void reset();

        /**
         * @brief 设置系统发送缓冲区大小.
         * @param [in] nSize : 缓冲区大小
         * @return 函数调用成功返回 T3D_OK.
         */
        TResult setSendBufferSize(int32_t nSize);

        /**
         * @brief 设置系统接收缓冲区大小.
         * @param [in] nSize : 换红区大熊啊
         * @return 函数调用成功返回 T3D_OK.
         */
        TResult setRecvBufferSize(int32_t nSize);

        /**
         * @brief 设置是否可重用端口.
         * @param [in] bReuse : reuse
         * @return 函数调用成功返回 T3D_OK.
         */
        TResult setReuseAddress(bool bReuse);

        /**
         * @brief 获取本地ip和端口.
         * @param [out] rSockName : ip
         * @param [out] rSockPort : 端口
         * @return 函数调用成功返回 T3D_OK.
         */
        TResult getLocalName(String &rSockName, uint16_t &rSockPort) const;

        /**
         * @brief 获取对端ip和端口.
         * @param [in] rPeerName : ip
         * @param [in] rPeerPort : 端口
         * @return 函数调用成功返回true，否则返回false.  
         */
        TResult getPeerName(String &rPeerName, uint16_t &rPeerPort) const;

        /**
         * @brief 获取系统错误码.
         * @return 返回系统错误码  
         */
        uint32_t getErrorCode() const;

        /**
         * @brief 获取socket错误码.
         * @return 返回socket错误码
         */
        uint32_t getSocketError() const;

        /**
         * @brief 获取原始套接字描述符
         */
        SOCKET getSocketFD() const  { return mSocket; }

        /**
         * @brief DNS同步解析.
         * @param [in] strDomain : 域名
         * @return 返回字符串形式的IP地址 
         */
        static String getHostByName(const String &strDomain);

        /**
         * @brief DNS同步解析.
         * @param [in] strDomain : 域名
         * @return 返回的整型IP地址  
         */
        static ulong_t DNS(const String &strDomain);

        /**
         * @brief 判断字符串是否是域名.
         * @param [in] strDomain : 字符串
         * @return 如果是域名格式返回true
         */
        static bool isDomain(const String &strDomain);

        static uint16_t htons(uint16_t val) { return isBigEndian() ? val : swap16(val); }

        static uint32_t htonl(uint32_t val) { return isBigEndian() ? val : swap32(val); }

        static uint16_t ntohs(uint16_t val) { return isBigEndian() ? val : swap16(val); }

        static uint32_t ntohl(uint32_t val) { return isBigEndian() ? val : swap32(val); }

    protected:
        /**
         * 准备套接字描述符集，用于检测是否可读、可写、发生异常错误
         * @param [in] readfds : 可读套接字描述符集
         * @param [in] writefds : 可写套接字描述符集
         * @param [in] errorfds : 异常错误套接字描述符集
         * @return 没有错误则返回 socket 句柄，否则返回 INVALID_SOCKET
         */
        int32_t prepareFdSet(fd_set &readfds, fd_set &writefds, fd_set &errorfds);

        /**
         * 检测是否有套接字可读、可写、发生异常错误
         * @param [in] nfds : 套接字描述符数量
         * @param [in,out] readfds : 可读套接字描述符集
         * @param [in,out] writefds : 可写套接字描述符集
         * @param [in,out] errorfds : 异常错误套接字描述符集
         * @param [in] timeout : 超时时间，单位：毫秒
         * @return 调用成功返回非负数
         */
        static int32_t select(int32_t nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, uint32_t timeout);

        /**
         * 处理读写异常事件
         * @param readfds : 可读套接字描述符集
         * @param writefds : 可写套接字描述符集
         * @param errorfds : 异常错误套接字描述符集
         * @return 调用成功返回 T3D_OK
         */
        TResult handleEvent(fd_set &readfds, fd_set &writefds, fd_set &errorfds);
        
        TResult startup(int32_t nVersionH, int32_t nVersionL);
        
        TResult cleanup();

        static TResult enqueue(Socket *socket);

        static TResult dequeue(Socket *socket);

        static bool isBigEndian() { uint16_t x = 0x1234; return (*(unsigned char*)&x == 0x12); }

        static uint16_t swap16(uint16_t val)
        {
            return ((((uint16_t)(val) & 0xff00) >> 8) | (((uint16_t)(val) & 0x00ff) << 8));
        }

        static uint32_t swap32(uint32_t val)
        {
            return ((((uint32_t)(val) & 0xff000000) >> 24) | (((uint32_t)(val) & 0x00ff0000) >> 8)
                | (((uint32_t)(val) & 0x0000ff00) << 8) | (((uint32_t)(val) & 0x000000ff) << 24));
        }

        void callOnAccepted(Socket *sockClient) { if (mOnAccepted) mOnAccepted(this, sockClient); }

        void callOnConnected(TResult result) { if (mOnConnected) mOnConnected(this, result); }

        TResult callOnRecv() { return mOnRecv ? mOnRecv(this) : T3D_OK; }

        TResult callOnSend() { return mOnSend ? mOnSend(this) : T3D_OK; };

        TResult callOnException() { return mOnException ? mOnException(this) : T3D_OK; }
        
        void callOnDisconnected(TResult result) { if (mOnDisconnected) mOnDisconnected(this, result); }

    protected:
        OnAccepted mOnAccepted {nullptr};
        OnConnected mOnConnected {nullptr};
        OnSend mOnSend {nullptr};
        OnRecv mOnRecv {nullptr};
        OnDisconnected mOnDisconnected {nullptr};
        OnException mOnException {nullptr};
        
        SOCKET mSocket {INVALID_SOCKET};
        State mState {State::kDisconnected};
        
        static int32_t  msCount;

        using Sockets = TList<Socket*>;
        static Sockets msSockets;
    };
}
 

#endif  /*__T3D_SOCKER_H__*/