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

#include "Adapter/OSX/T3DOSXFSMonitor.h"
#include "T3DDir.h"
#include "T3DPlatformErrorDef.h"
#include "T3DSyncObject.h"

#include <CoreServices/CoreServices.h>
#include <dispatch/dispatch.h>
#include <algorithm>
#include <unistd.h>


namespace Tiny3D
{
    static void OSXFSMonitorStreamCallback(
        ConstFSEventStreamRef stream,
        void *clientCallBackInfo,
        size_t numEvents,
        void *eventPaths,
        const FSEventStreamEventFlags eventFlags[],
        const FSEventStreamEventId eventIds[])
    {
        (void)stream;
        (void)eventIds;

        auto *self = static_cast<OSXFSMonitor *>(clientCallBackInfo);
        if (self != nullptr)
        {
            self->enqueueEvents(numEvents, static_cast<char **>(eventPaths),
                reinterpret_cast<const uint32_t *>(eventFlags));
        }
    }

    OSXFSMonitor::OSXFSMonitor()
    {
        mChangedQMutex = T3D_NEW Mutex();
    }

    OSXFSMonitor::~OSXFSMonitor()
    {
        stopWatching();
        T3D_SAFE_DELETE(mChangedQMutex);
    }

    TResult OSXFSMonitor::startWatching(const String &path, const FSMonitorExts &excludeExts, const FSMonitorExcludes &excludeFolders, const FSMonitorOnChanged &onChanged)
    {
        TResult ret = T3D_OK;

        do
        {
            if (path.empty())
            {
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }

            stopWatching();

            mPath = path;
            mExcludeExts = excludeExts;
            mExcludeFolders = excludeFolders;
            mOnChanged = onChanged;

            CFStringRef cfPath = CFStringCreateWithCString(kCFAllocatorDefault, path.c_str(), kCFStringEncodingUTF8);
            if (cfPath == nullptr)
            {
                ret = T3D_ERR_FS_MONITOR_CREATED;
                break;
            }

            CFArrayRef pathsToWatch = CFArrayCreate(kCFAllocatorDefault, (const void **)&cfPath, 1, &kCFTypeArrayCallBacks);
            CFRelease(cfPath);
            if (pathsToWatch == nullptr)
            {
                ret = T3D_ERR_FS_MONITOR_CREATED;
                break;
            }

            FSEventStreamContext ctx = {0, this, nullptr, nullptr, nullptr};
            const FSEventStreamCreateFlags flags =
                kFSEventStreamCreateFlagFileEvents | kFSEventStreamCreateFlagNoDefer;

            FSEventStreamRef stream = FSEventStreamCreate(
                kCFAllocatorDefault,
                &OSXFSMonitorStreamCallback,
                &ctx,
                pathsToWatch,
                kFSEventStreamEventIdSinceNow,
                0.25,
                flags);
            CFRelease(pathsToWatch);

            if (stream == nullptr)
            {
                ret = T3D_ERR_FS_MONITOR_CREATED;
                break;
            }

            FSEventStreamSetDispatchQueue(stream, dispatch_get_global_queue(QOS_CLASS_UTILITY, 0));
            if (!FSEventStreamStart(stream))
            {
                FSEventStreamSetDispatchQueue(stream, nullptr);
                FSEventStreamInvalidate(stream);
                FSEventStreamRelease(stream);
                ret = T3D_ERR_FS_MONITOR_CREATED;
                break;
            }

            mStream = stream;
        } while (false);

        if (ret != T3D_OK)
        {
            mPath.clear();
            mExcludeExts.clear();
            mExcludeFolders.clear();
            mOnChanged = nullptr;
        }

        return ret;
    }

    TResult OSXFSMonitor::poll()
    {
        if (mStream != nullptr)
        {
            FSEventStreamFlushAsync(static_cast<FSEventStreamRef>(mStream));
        }

        if (mOnChanged != nullptr && mChangedQMutex != nullptr)
        {
            ScopeLock lock(mChangedQMutex);
            while (!mChangedItemsQ.empty())
            {
                const auto &item = mChangedItemsQ.front();
                mOnChanged(item.filePath, item.action);
                mChangedItemsQ.pop_front();
            }
        }

        return T3D_OK;
    }

    TResult OSXFSMonitor::stopWatching()
    {
        if (mStream != nullptr)
        {
            FSEventStreamRef stream = static_cast<FSEventStreamRef>(mStream);
            FSEventStreamStop(stream);
            FSEventStreamSetDispatchQueue(stream, nullptr);
            FSEventStreamInvalidate(stream);
            FSEventStreamRelease(stream);
            mStream = nullptr;
        }

        if (mChangedQMutex != nullptr)
        {
            ScopeLock lock(mChangedQMutex);
            mChangedItemsQ.clear();
        }

        mPath.clear();
        mExcludeExts.clear();
        mExcludeFolders.clear();
        mOnChanged = nullptr;
        return T3D_OK;
    }

    const String &OSXFSMonitor::getPath() const
    {
        return mPath;
    }

    void OSXFSMonitor::enqueueEvents(size_t numEvents, char **paths, const uint32_t *flags)
    {
        if (paths == nullptr || flags == nullptr || mChangedQMutex == nullptr)
        {
            return;
        }

        ScopeLock lock(mChangedQMutex);

        for (size_t i = 0; i < numEvents; ++i)
        {
            if (paths[i] == nullptr)
            {
                continue;
            }

            const String path(paths[i]);
            if (path.empty())
            {
                continue;
            }

            if (!mExcludeExts.empty() || !mExcludeFolders.empty())
            {
                if (isExcludedExt(path) || isExcludedFolder(path))
                {
                    continue;
                }
            }

            const uint32_t eventFlags = flags[i];
            FSMonitorAction action = FSMonitorAction::kNone;

            if ((eventFlags & kFSEventStreamEventFlagItemRenamed) != 0)
            {
                action = (access(path.c_str(), F_OK) == 0)
                    ? FSMonitorAction::kRenamedNew
                    : FSMonitorAction::kRenamedOld;
            }
            else if ((eventFlags & kFSEventStreamEventFlagItemRemoved) != 0)
            {
                action = FSMonitorAction::kRemoved;
            }
            else if ((eventFlags & kFSEventStreamEventFlagItemCreated) != 0)
            {
                action = FSMonitorAction::kAdded;
            }
            else if ((eventFlags & (kFSEventStreamEventFlagItemModified
                | kFSEventStreamEventFlagItemInodeMetaMod
                | kFSEventStreamEventFlagItemXattrMod)) != 0)
            {
                action = FSMonitorAction::kModifed;
            }

            if (action != FSMonitorAction::kNone)
            {
                mChangedItemsQ.emplace_back(action, path);
            }
        }
    }

    bool OSXFSMonitor::isExcludedExt(const String &filename) const
    {
        String dir;
        String title;
        String ext;
        if (!Dir::parsePath(filename, dir, title, ext) || ext.empty())
        {
            return false;
        }

        return std::find(mExcludeExts.begin(), mExcludeExts.end(), ext) != mExcludeExts.end();
    }

    bool OSXFSMonitor::isExcludedFolder(const String &folder) const
    {
        String dir;
        String title;
        String ext;
        if (!Dir::parsePath(folder, dir, title, ext) || title.empty())
        {
            return false;
        }

        return std::find(mExcludeFolders.begin(), mExcludeFolders.end(), title) != mExcludeFolders.end();
    }
}
