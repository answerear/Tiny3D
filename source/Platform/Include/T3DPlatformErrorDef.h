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

#ifndef __T3D_PLATFORM_ERROR_DEF_H__
#define __T3D_PLATFORM_ERROR_DEF_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DCommonErrorDef.h"

namespace Tiny3D
{
    enum PlatformErrorCode
    {
        // Platform 模块错误码从 T3D_ERR_PLATFORM 开始

        /// 线程已经创建了
        T3D_ERR_THREAD_ALREADY_CREATED      = T3D_ERR_PLATFORM,
        /// 创建系统线程失败
        T3D_ERR_THREAD_CREATED,
        /// 线程未创建
        T3D_ERR_THREAD_NOT_CREATED,
        /// 线程初始化失败
        T3D_ERR_THREAD_INIT,
        /// 线程挂起失败
        T3D_ERR_THREAD_SUSPEND,
        /// 线程继续失败
        T3D_ERR_THREAD_RESUME,
        /// 实现对象没有创建
        T3D_ERR_IMPLEMENT_NOT_CREATED,
        /// 原生对象没创建
        T3D_ERR_NATIVE_OBJECT_NOT_CREATED,
        /// 非可写文件
        T3D_ERR_NOT_WRAITABLE_FILE,
        /// 进程创建失败
        T3D_ERR_PROCESS_CREATED,
        /// 进程终止失败
        T3D_ERR_PROCESS_TERMINATED,
        /// socket 错误
        T3D_ERR_SOCKET_ERROR,
        /// socket 远端关闭
        T3D_ERR_REMOTE_DISCONNECTED,
        /// socket 启动失败，windows 系统专有错误码
        T3D_ERR_SOCKET_STARTUP,
        /// socket 版本错误，windows 系统专有错误码
        T3D_ERR_SOCKET_VERSION,
        /// socket 创建失败
        T3D_ERR_SOCKET_CREATED,
        /// socket 无效
        T3D_ERR_SOCKET_INVALID,
        /// socket 重复
        T3D_ERR_SOCKET_DUPLICATED,
        /// socket 没找到
        T3D_ERR_SOCKET_NOT_FOUND,
        /// DNS 解析失败
        T3D_ERR_SOCKET_DNS,
        /// socket 没有连接
        T3D_ERR_SOCKET_NOT_CONNECTED,
        /// socket 发送失败
        T3D_ERR_SOCKET_SEND,
        /// socket 发送缓冲区满了
        T3D_ERR_SOCKET_NO_SENDBUF,
        /// File System Monitor 创建失败
        T3D_ERR_FS_MONITOR_CREATED,
    };
};

#endif  /*__T3D_PLATFORM_ERROR_DEF_H__*/
