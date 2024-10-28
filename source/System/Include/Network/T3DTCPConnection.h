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

#ifndef __T3D_TCP_CONNECTION_H__
#define __T3D_TCP_CONNECTION_H__


#include "T3DSystemPrerequisites.h"


namespace Tiny3D
{
    /// listener : 监听者
    using OnTCPListening = TFunction<void(TCPListener *listener)>;
    
    /// listener : 监听者
    /// connection : 新接入的连接对象
    using OnTCPAccepted = TFunction<void(TCPListener *listener, TCPConnection *connection)>;

    /**
     * TCP 监听者，用于监听 TCP 连接请求
     */
    class T3D_SYSTEM_API TCPListener : public Allocator, public Noncopyable
    {
    public:
        /**
         * 轮训事件
         */
        static void poll();

        /// Constructor.
        TCPListener() = default;

        /// Destructor.
        ~TCPListener() override;

        /**
         * 设置开始监听回调
         * @param [in] onStarted : 回调 lambda 
         */
        void setOnStartedCallback(const OnTCPListening &onStarted) { mOnStartListening = onStarted; }

        /**
         * 设置新连接请求建立回调
         * @param [in] onAccepted : 回调 lambda
         */
        void setOnAcceptedCallback(const OnTCPAccepted &onAccepted) { mOnAccepted = onAccepted; }

        /**
         * 设置停止监听回调
         * @param [in] onStopped : 回调 lambda
         */
        void setOnStoppedCallback(const OnTCPListening &onStopped) { mOnStopListening = onStopped; }

        /**
         * 开始监听
         * @param [in] port : 绑定端口
         * @param [in] addr : 绑定 IP
         * @param [in] backlog : 等待队列最大值
         * @return 调用成功返回 T3D_OK.
         */
        TResult listen(uint16_t port, const String &addr = "", int32_t backlog = 5);

        /**
         * 停止监听
         * @return 调用成功返回 T3D_OK.
         */
        TResult stop();

    protected:
        void doAccepted();
        
        void callOnStarted() { if (mOnStartListening) mOnStartListening(this); }

        void callOnAccepted(TCPConnection *connection) { if (mOnAccepted) mOnAccepted(this, connection); }

        void callOnStopped() { if (mOnStopListening) mOnStopListening(this); }

    protected:
        /// 监听 socket
        Socket  *mSockListener {nullptr};

        /// 用于接受当前接入的客户端 socket
        Socket  *mSockClient {nullptr};

        /// 开始监听回调
        OnTCPListening mOnStartListening {nullptr};
        /// 停止监听回调
        OnTCPListening mOnStopListening {nullptr};
        /// 接受连接回调
        OnTCPAccepted mOnAccepted {nullptr};

        /// 是否在监听中
        bool mIsListening {false};

        using TCPListeners = TList<TCPListener*>;

        /// 所有监听者对象
        static TCPListeners msListeners;
    };

    /// connection : 连接对象
    /// result : 连接结果
    using OnTCPConnected = TFunction<void(TCPConnection *connection, TResult result)>;

    /// connection : 连接对象
    /// seq : 接收的序列号
    /// data : 接收的数据
    /// dataSize : 接收的数据大小
    using OnTCPRecv = TFunction<void(TCPConnection *connection, uint32_t seq, const void *data, int32_t dataSize)>;

    /// connection : 连接对象
    /// seq : 发送的序列号
    /// data : 发送的数据，跟 send 接口的 data 参数是一致的
    /// dataSize : 发送的数据大小，跟 send 接口的 dataSize 参数是一致的
    using OnTCPSend = TFunction<void(TCPConnection *connection, uint32_t seq, const void *data, int32_t dataSize)>;

    /// connection : 连接对象
    /// result : 异常错误码
    using OnTCPException = TFunction<void(TCPConnection *connection, TResult result)>;

    /// connection : 连接对象
    using OnTCPDisconnected = TFunction<void(TCPConnection *connection, TResult result)>;
    
    /**
     * TCP 连接
     */
    class T3D_SYSTEM_API TCPConnection : public Noncopyable
    {
    public:
        /**
         * 轮训事件
         */
        static void poll();

        /**
         * 关闭所有连接，并删除 connection 对象
         */
        static void closeAllConnections();

        /**
         * 释放连接对象，相当于 Delete ，请勿自己调用 delete 去删除，使用本函数代替
         * @param [in] connection : 要删除的连接对象
         */
        static void destroyConnection(TCPConnection *connection);

        static void destroyAll();

        /// Constructor.
        TCPConnection();

        /*
         * 构造函数
         * @param [in] socket : 连接相关的套接字
         */
        TCPConnection(Socket *socket);

        /// Destructor.
        ~TCPConnection() override;

        /**
         * 设置连接上回调
         * @param [in] onConnected : 回调 lambda 
         */
        void setOnConnectedCallbak(const OnTCPConnected &onConnected) { mOnConnected = onConnected; }

        /**
         * 设置收包的回调
         * @param [in] onRecv : 回调 lambda 
         */
        void setOnRecvCallback(const OnTCPRecv &onRecv) { mOnRecv = onRecv; }

        /**
         * 设置发包的回调
         * @param [in] onSend : 回调 lambda
         */
        void setOnSendCallback(const OnTCPSend &onSend) { mOnSend = onSend; }

        /**
         * 设置连接异常时回调
         * @param [in] onException : 回调 lambda 
         */
        void setOnExceptionCallback(const OnTCPException &onException) { mOnException = onException; }

        /**
         * 设置断开连接时回调
         * @param [in] onDisconnected : 回调 lambda 
         */
        void setOnDisconnectedCallback(const OnTCPDisconnected &onDisconnected) { mOnDisconnected = onDisconnected; }

        /**
         * 设置发送缓冲区容量
         * @param [in] capacity : 容量
         */
        void setSendBufferCapacity(int32_t capacity) { mSendBufferCapacity = capacity; mIsSendBufDirty = true; }

        /**
         * 设置接收缓冲区容量
         * @param capacity 
         */
        void setRecvBufferCapacity(int32_t capacity) { mRecvBufferCapacity = capacity; mIsRecvBufDirty = true; }
        
        /**
         * 发起连接
         * @param [in] addr : 远端地址
         * @param [in] port : 远端端口
         * @param [in] timeout : 连接超时
         * @return 调用成功返回 T3D_OK.
         */
        TResult connect(const String &addr, uint16_t port, uint32_t timeout = 10 * 1000);

        /**
         * 发送数据
         * @param [in] seq : 发送序列号
         * @param [in] data : 发送的数据
         * @param [in] dataSize : 发送数据的大小
         * @param [in] cacheWhenConnecting : 在连接中是否缓存
         * @return 调用成功返回 T3D_OK.
         */
        TResult send(uint32_t seq, const void *data, int32_t dataSize, bool cacheWhenConnecting);
        
        /**
         * 断开连接
         * @return 调用成功返回 T3D_OK.
         */
        TResult disconnect();

        const String &getPeerName() const { return mPeerName; }

        uint16_t getPeerPort() const { return mPeerPort; }

        bool isConnecting() const { return mSocket ? mSocket->isConnecting() : false; }

        bool isConnected() const { return mSocket ? mSocket->isConnected() : false; }

    protected:
        void setupCallbacks();
        
        void callOnConnected(TResult result) { if (mOnConnected) mOnConnected(this, result); }
        
        void callOnRecv(uint32_t seq, const void *data, int32_t dataSize) { if (mOnRecv) mOnRecv(this, seq, data, dataSize); }

        void callOnSend(uint32_t seq, const void *data, int32_t dataSize) { if (mOnSend) mOnSend(this, seq, data, dataSize); }
        
        void callOnException(TResult result) { if (mOnException) mOnException(this, result); }
        
        void callOnDisconnected(TResult result) { if (mOnDisconnected) mOnDisconnected(this, result); }

        void ensureSendBuffer();

        void ensureRecvBuffer();

        TResult onSend(Socket *socket);

        TResult onRecv(Socket *socket);

        TResult sendRemainData();

        static TResult enqueue(TCPConnection *connection);
    
        static TResult dequeue(TCPConnection *connection);

        void startConnectingTimer(uint32_t timeout);

        void stopConnectingTimer();
        
    protected:
        /// 连接 socket
        Socket *mSocket {nullptr};

        /// 对端地址
        String mPeerName {};

        /// 对端端口
        uint16_t mPeerPort {0};

        /// 接收数据缓冲区
        uint8_t *mRecvBuffer {nullptr};

        /// 接收数据的大小
        int32_t mRecvBufferSize {0};

        /// 接收数据缓冲区的容量
        int32_t mRecvBufferCapacity {32*1024};

        /// 发送数据缓冲区
        uint8_t *mSendBuffer {nullptr};

        /// 发送数据的大小
        int32_t mSendBufferSize {0};

        /// 发送数据缓冲区开始位置
        int32_t mSendBufferBegin {0};

        /// 发送数据缓冲区的容量
        int32_t mSendBufferCapacity {8*1024};

        /// 连接超时定时器
        ID mConnectTimerID {T3D_INVALID_TIMER_ID};

        /// 是否发送缓冲区重新分配空间
        bool mIsSendBufDirty {true};

        /// 是否接受缓冲区重新分配空间
        bool mIsRecvBufDirty {true};

        /// 连接上回调
        OnTCPConnected mOnConnected {nullptr};

        /// 收包回调
        OnTCPRecv mOnRecv {nullptr};

        /// 发包回调
        OnTCPSend mOnSend {nullptr};

        /// 发生异常回调
        OnTCPException mOnException {nullptr};

        /// 断开连接回调
        OnTCPDisconnected mOnDisconnected {nullptr};

        using TCPConnections = TUnorderedSet<TCPConnection*>;

        /// 所有的连接对象
        static TCPConnections msConnections;
        /// 待删除的连接对象
        static TCPConnections msWaitingDeleteConnections;
    };
}
 

#endif  /*__T3D_TCP_CONNECTION_H__*/