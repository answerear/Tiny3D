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

#ifndef __T3D_OSX_FS_MONITOR_H__
#define __T3D_OSX_FS_MONITOR_H__


#include "T3DNoncopyable.h"
#include "Adapter/T3DFSMonitorInterface.h"
#include "Memory/T3DMemory.h"


namespace Tiny3D
{
    class Mutex;

    class OSXFSMonitor : public IFSMonitor, public Noncopyable
    {
    public:
        OSXFSMonitor();

        ~OSXFSMonitor() override;

        TResult startWatching(const String &path, const FSMonitorExts &excludeExts, const FSMonitorExcludes &excludeFolders, const FSMonitorOnChanged &onChanged) override;

        TResult poll() override;

        TResult stopWatching() override;

        const String &getPath() const override;

        void enqueueEvents(size_t numEvents, char **paths, const uint32_t *flags);

    protected:

        bool isExcludedExt(const String &filename) const;

        bool isExcludedFolder(const String &folder) const;

        void *                mStream {nullptr};
        Mutex *               mChangedQMutex {nullptr};
        ChangedItemQueue      mChangedItemsQ {};
        String                mPath;
        FSMonitorExts         mExcludeExts;
        FSMonitorExcludes     mExcludeFolders;
        FSMonitorOnChanged    mOnChanged {nullptr};
    };
}


#endif  /*__T3D_OSX_FS_MONITOR_H__*/
