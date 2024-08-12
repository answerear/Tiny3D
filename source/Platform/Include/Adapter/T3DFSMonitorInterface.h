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

#ifndef __T3D_FS_MONITOR_INTERFACE_H__
#define __T3D_FS_MONITOR_INTERFACE_H__


#include "T3DMacro.h"
#include "T3DPlatformPrerequisites.h"


namespace Tiny3D
{
    class T3D_PLATFORM_API IFSMonitor
    {
        T3D_DECLARE_INTERFACE(IFSMonitor);

    public:
        /**
         * 初始化监控
         * @param [in] path : 监控的路径
         * @param [in] excludeExts : 排除的文件扩展名，不监控这些扩展名文件
         * @param [in] excludeFolders : 排除的文件夹，不监控这些文件夹
         * @param [in] onChanged : 发生变化的回调
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult init(const String &path, const FSMonitorExts &excludeExts, const FSMonitorExcludes &excludeFolders, const FSMonitorOnChanged &onChanged) = 0;
        
        /**
         * 监控文件系统变化
         */
        virtual TResult monitor() = 0;

        /**
         * 监控结束，清理操作
         */
        virtual void cleanup() = 0;
    };
}

#endif  /*__T3D_FS_MONITOR_INTERFACE_H__*/
