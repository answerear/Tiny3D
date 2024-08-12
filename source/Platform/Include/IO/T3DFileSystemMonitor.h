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

#ifndef __T3D_FILESYSTEM_MONITOR_H__
#define __T3D_FILESYSTEM_MONITOR_H__


#include "T3DMacro.h"
#include "T3DPlatformPrerequisites.h"
#include "T3DNoncopyable.h"
#include "Thread/T3DRunnable.h"


namespace Tiny3D
{
    class IFSMonitor;
    class Mutex;
    
    class T3D_PLATFORM_API FileSystemMonitor
        : public Noncopyable
        , public Runnable
    {
    public:
        FileSystemMonitor();
        
        ~FileSystemMonitor() override;

        TResult startMonitor(const String &path, const FSMonitorExts &excludeExts, const FSMonitorExcludes &excludeFolders, const FSMonitorOnChanged &onChanged);

        TResult stopMonitor();

        void update();

        static void poll();

    protected:
        bool init() override;
        
        TResult run() override;

        void exit() override;

        void stop() override;

    protected:
        struct ChangedItem
        {
            ChangedItem(FSMonitorAction act, const String &path)
                : action (act)
                , filePath(path)
            {}
            
            FSMonitorAction action {FSMonitorAction::kAdded};
            String filePath {};
        };
        
        using Monitors = TMap<String, FileSystemMonitor*>;
        using ChangedItemQueue = TList<ChangedItem>;
        
        /// 所有监控器
        static Monitors msMonitors;

        /// 具体实现的 FileSystemMonitor
        IFSMonitor *mMonitor {nullptr};
        /// 监控线程
        RunnableThread *mThread {nullptr};
        /// 用于异步线程在主线程回调
        ChangedItemQueue mChangedItemsQ {};
        /// 回调队列互斥量
        Mutex *mChangedQMutex {};
        /// 要监控的路径
        String mPath {};
        /// 排除监控的文件扩展名列表
        FSMonitorExts mExcludeExts {};
        /// 排除监控文件夹列表
        FSMonitorExcludes mExcludeFolders {};
        /// 变化回调
        FSMonitorOnChanged mOnChanged {nullptr};
        /// 是否在运行
        bool mIsRunning {false};
    };
}


#endif  /*__T3D_FILESYSTEM_MONITOR_H__*/
