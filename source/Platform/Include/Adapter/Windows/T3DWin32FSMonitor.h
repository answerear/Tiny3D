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

#ifndef __T3D_WIN32_FS_MONITOR_H__
#define __T3D_WIN32_FS_MONITOR_H__


#include "Adapter/T3DFSMonitorInterface.h"
#include <windows.h>


namespace Tiny3D
{
    class T3D_PLATFORM_API Win32FSMonitor : public IFSMonitor
    {
    public:
        Win32FSMonitor();

        ~Win32FSMonitor() override;

        TResult init(const String &path, const FSMonitorExts &excludeExts, const FSMonitorExcludes &excludeFolders, const FSMonitorOnChanged &onChanged) override;
        
        TResult monitor() override;

        void cleanup() override;

    protected:
        HANDLE mDirHandle {nullptr};
        OVERLAPPED mOverlapped {};
        String mPath {};
        FSMonitorExts mExcludeExts {};
        FSMonitorExcludes mExcludeFolders {};
        FSMonitorOnChanged mOnChanged {nullptr};
    };
}


#endif  /*__T3D_WIN32_FS_MONITOR_H__*/
