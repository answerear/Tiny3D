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

#ifndef __T3D_PLATFORM_PREREQUISITES_H__
#define __T3D_PLATFORM_PREREQUISITES_H__


#if defined T3DPLATFORM_EXPORT
    #define T3D_PLATFORM_API    T3D_EXPORT_API
#else
    #define T3D_PLATFORM_API    T3D_IMPORT_API
#endif


#include "T3DType.h"


namespace Tiny3D
{
    class System;
    class Application;
    class Window;
    class TimerManager;
    class ITimerListener;
    class DateTime;
    class Console;
    class DeviceInfo;
    class Dir;
    class DataStream;
    class FileDataStream;
    class MemoryDataStream;
    class Locale;
    class Process;
    class Socket;
    class TCPConnection;
    class TCPListener;

    /// pid : 进程 ID
    /// pname : 进程名称
    using OnTraverseProcess = TFunction<bool(ulong_t pid, const String &pname)>;

    /// timerID : 定时器 ID，startTimer 返回的那个 ID
    /// dt : 距离启动或者上次回调（循环定时器时候存在上一次回调）的时间间隔
    using TimerCallback = TFunction<void(ID timerID, uint32_t dt)>;

    enum class FSMonitorAction : uint32_t
    {
        kNone = 0,
        /// 新增文件或文件夹
        kAdded,
        /// 删除文件或者文件夹
        kRemoved,
        /// 修改文件或文件夹
        kModifed,
        /// 重命名文件或文件夹，此时文件或文件夹名是旧名称
        kRenamedOld,
        /// 重命名文件或文件夹，此时文件或文件夹名是新名称
        kRenamedNew,
    };
        
    using FSMonitorExts = TList<String>;
    using FSMonitorExcludes = TList<String>;
    using FSMonitorOnChanged = TFunction<void(const String&, FSMonitorAction)>;
}
 

#endif  /*__T3D_PLATFORM_PREREQUISITES_H__*/