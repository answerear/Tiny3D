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
        void enqueue(TCPConnection *connection);

        void enqueue(TCPConnection *connection, const String &path, const String &name);

        void dequeue(TCPConnection *connection);

        void closeAllConnections();

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
        
        /// 网络接入 listener，用于接入 editor 的连接请求
        TCPListener *mTCPListener {nullptr};

        using TCPConnections = TList<TCPConnection*>;
        using EditorInstances = TUnorderedMap<EditorInstance, TCPConnection*, EditorInstanceHash>;
        using ClientConnections = TUnorderedMap<TCPConnection*, EditorInstance>;

        /// 所有客户端套接字
        TCPConnections mTCPConnections {};
        /// 方便根据 EditorInstance 获取 connection
        ClientConnections mClientConnections {};
        /// 方便根据 connection 获取 editor 实例
        EditorInstances mEditorInstances {};
    };

    #define NETWORK_MGR     NetworkManager::getInstance()
    
    NS_END
}
