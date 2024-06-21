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

#pragma once


#include "EditorPrerequisites.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    class NetworkManager
        : public EventHandler
        , public Singleton<NetworkManager>
    {
    public:
        NetworkManager();

        ~NetworkManager() override;

        TResult startup(const String &addr, uint16_t port);

        void poll();
        
        void shutdown();

        TResult createProject(const String &path, const String &name);

        TResult openProject(const String &path, const String &name);

    protected:
        void onConnected(TCPConnection *connection, TResult result);

        TResult onRecv(TCPConnection *connection, uint32_t seq, const void *data, int32_t dataSize);

        TResult onSend(TCPConnection *connection, uint32_t seq, const void *data, int32_t dataSize);

        TResult onException(TCPConnection *connection, TResult result);

        void onDisconnected(TCPConnection *connection, TResult result);

        void startMonitorTimer();

        void stopMonitorTimer();

        void startHelloTimer();

        void stopHelloTimer();

    protected:
        /// 跟 launcher 通信的连接
        TCPConnection *mConnection {nullptr};

        /// 检测 launcher 时间间隔
        static uint32_t MONITOR_INTERVAL;
        /// 心跳时间间隔
        static uint32_t HELLO_INTERVAL;
        
        /// 检测 launcher 是否存在的定时器
        ID mMonitorTimerID {T3D_INVALID_TIMER_ID};

        /// 心跳定时器
        ID mHelloTimerID {T3D_INVALID_TIMER_ID};

        /// 对端地址
        String mRemoteAddr {};
        /// 对端端口
        uint16_t mRemotePort {0};

        /// 发包序列号
        uint32_t mSeq {0};

        enum
        {
            SENDBUF_SIZE = 32*1024
        };
        
        uint8_t mSendBuffer[SENDBUF_SIZE];
    };

    #define NETWORK_MGR     NetworkManager::getInstance()
    
    NS_END
}
