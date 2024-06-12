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


#include "LauncherPrerequisites.h"


namespace Tiny3D
{
    NS_BEGIN(Launcher)

    class NetworkManager
        : public EventHandler
        , public Singleton<NetworkManager>
    {
    public:
        NetworkManager();

        ~NetworkManager() override;

        TResult startup();

        void poll();
        
        void shutdown();
        
    protected:
        void onAccepted(Socket *sockListen, Socket *sockClient);

        TResult onRecv(Socket *socket);

        TResult onSend(Socket *socket);

        TResult onException(Socket *socket);

        void enqueue(Socket *socket);

        void enqueue(Socket *socket, const String &path, const String &name);

        void dequeue(Socket *socket);

        void closeAllSockets();

    protected:
        struct EditorInstance
        {
            /// 编辑器打开工程的路径
            String path {};
            /// 编辑器打开工程的名称
            String name {};

            bool operator ==(const EditorInstance &other) const { return (path == other.path && name == other.name); }

            bool operator ==(const EditorInstance &other) { return (path == other.path && name == other.name); }
        };

        struct EditorInstanceHash
        {
            std::size_t operator()(const EditorInstance &inst) const {
                std::size_t h1 = std::hash<String>{}(inst.path);
                std::size_t h2 = std::hash<String>{}(inst.name);
                return h1 ^ (h2 << 1); // 合并两个哈希值
            }
        };
        
        /// 监听用 socket，用于接入 editor 的连接请求
        Socket *mListenSocket {nullptr};

        /// 当前在监听的套接字，当客户端连接进入后，则放入套接字队列里面管理
        Socket *mCurrentClientSock {nullptr};

        using Sockets = TList<Socket*>;
        using EditorInstances = TUnorderedMap<EditorInstance, Socket*, EditorInstanceHash>;
        using ClientSockets = TUnorderedMap<Socket*, EditorInstance>;

        /// 所有客户端套接字
        Sockets mSockets {};
        /// 方便根据 EditorInstance 获取 socket
        ClientSockets mClientSockets {};
        /// 方便根据 socket 获取 editor 实例
        EditorInstances mEditorInstances {};
    };

    #define NETWORK_MGR     NetworkManager::getInstance()
    
    NS_END
}
