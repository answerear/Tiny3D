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

#include "Adapter/Linux/T3DLinuxFSMonitor.h"
#include "T3DCommonErrorDef.h"
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <poll.h>
#include <string.h>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    LinuxFSMonitor::LinuxFSMonitor()
        : mInotifyFd(-1)
    {
    }

    //--------------------------------------------------------------------------

    LinuxFSMonitor::~LinuxFSMonitor()
    {
        stopWatching();
    }

    //--------------------------------------------------------------------------

    TResult LinuxFSMonitor::startWatching(const String &path, const FSMonitorExts &excludeExts, const FSMonitorExcludes &excludeFolders, const FSMonitorOnChanged &onChanged)
    {
        TResult ret = T3D_OK;

        do
        {
            mInotifyFd = inotify_init1(IN_NONBLOCK);
            if (mInotifyFd < 0)
            {
                ret = T3D_ERR_FAIL;
                break;
            }

            mPath = path;
            mExcludeExts = excludeExts;
            mExcludeFolders = excludeFolders;
            mOnChanged = onChanged;

            addWatchRecursive(path);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult LinuxFSMonitor::poll()
    {
        if (mInotifyFd < 0)
            return T3D_ERR_FAIL;

        struct pollfd pfd;
        pfd.fd = mInotifyFd;
        pfd.events = POLLIN;

        int pollRet = ::poll(&pfd, 1, 0);
        if (pollRet <= 0)
            return T3D_OK;

        char buffer[4096];
        ssize_t len = read(mInotifyFd, buffer, sizeof(buffer));
        if (len <= 0)
            return T3D_OK;

        ssize_t offset = 0;
        while (offset < len)
        {
            struct inotify_event *event = (struct inotify_event *)(buffer + offset);
            offset += sizeof(struct inotify_event) + event->len;

            if (event->len == 0)
                continue;

            String filename(event->name);

            // Find the watch path
            auto it = mWatchDescriptors.find(event->wd);
            if (it == mWatchDescriptors.end())
                continue;

            String fullPath = it->second + "/" + filename;

            // Check exclusions
            if (event->mask & IN_ISDIR)
            {
                if (isExcludedFolder(filename))
                    continue;
            }
            else
            {
                if (isExcludedExt(filename))
                    continue;
            }

            // Determine action
            FSMonitorAction action = FSMonitorAction::kNone;

            if (event->mask & IN_CREATE)
            {
                action = FSMonitorAction::kAdded;

                // If a new directory is created, add watch recursively
                if (event->mask & IN_ISDIR)
                {
                    addWatchRecursive(fullPath);
                }
            }
            else if (event->mask & IN_DELETE)
            {
                action = FSMonitorAction::kRemoved;
            }
            else if (event->mask & IN_MODIFY)
            {
                action = FSMonitorAction::kModifed;
            }
            else if (event->mask & IN_MOVED_FROM)
            {
                action = FSMonitorAction::kRenamedOld;
            }
            else if (event->mask & IN_MOVED_TO)
            {
                action = FSMonitorAction::kRenamedNew;
            }

            if (action != FSMonitorAction::kNone && mOnChanged)
            {
                mOnChanged(fullPath, action);
            }
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult LinuxFSMonitor::stopWatching()
    {
        if (mInotifyFd >= 0)
        {
            for (auto &pair : mWatchDescriptors)
            {
                inotify_rm_watch(mInotifyFd, pair.first);
            }
            mWatchDescriptors.clear();

            close(mInotifyFd);
            mInotifyFd = -1;
        }

        mPath.clear();
        mOnChanged = nullptr;

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    const String &LinuxFSMonitor::getPath() const
    {
        return mPath;
    }

    //--------------------------------------------------------------------------

    void LinuxFSMonitor::addWatchRecursive(const String &path)
    {
        int wd = inotify_add_watch(mInotifyFd, path.c_str(),
            IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);

        if (wd >= 0)
        {
            mWatchDescriptors[wd] = path;
        }

        // Recurse into subdirectories
        DIR *dir = opendir(path.c_str());
        if (dir == nullptr)
            return;

        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            if (entry->d_type != DT_DIR)
                continue;

            String dirName(entry->d_name);
            if (isExcludedFolder(dirName))
                continue;

            String subPath = path + "/" + dirName;
            addWatchRecursive(subPath);
        }

        closedir(dir);
    }

    //--------------------------------------------------------------------------

    bool LinuxFSMonitor::isExcludedExt(const String &filename) const
    {
        size_t dotPos = filename.rfind('.');
        if (dotPos == String::npos)
            return false;

        String ext = filename.substr(dotPos);

        for (const auto &excludeExt : mExcludeExts)
        {
            if (ext == excludeExt)
                return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool LinuxFSMonitor::isExcludedFolder(const String &folder) const
    {
        for (const auto &excludeFolder : mExcludeFolders)
        {
            if (folder == excludeFolder)
                return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------
}
