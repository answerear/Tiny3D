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


#include "IO/T3DFileSystemMonitor.h"
#include "T3DPlatformErrorDef.h"
#include "Adapter/T3DFSMonitorInterface.h"
#include "T3DPlatform.h"
#include "Adapter/T3DFactoryInterface.h"
#include "Thread/T3DRunnableThread.h"
#include "Thread/T3DSyncObject.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    FileSystemMonitor::Monitors FileSystemMonitor::msMonitors;
    
    //--------------------------------------------------------------------------

    FileSystemMonitor::FileSystemMonitor()
    {
        mMonitor = T3D_PLATFORM_FACTORY.createFileSystemMonitor();
        mChangedQMutex = new Mutex();
    }

    //--------------------------------------------------------------------------

    FileSystemMonitor::~FileSystemMonitor()
    {
        stopMonitor();
        T3D_SAFE_DELETE(mChangedQMutex);
        T3D_SAFE_DELETE(mMonitor);
    }

    //--------------------------------------------------------------------------

    TResult FileSystemMonitor::startMonitor(const String &path, const FSMonitorExts &excludeExts, const FSMonitorExcludes &excludeFolders, const FSMonitorOnChanged &onChanged)
    {
        mPath = path;
        mExcludeExts = excludeExts;
        mExcludeFolders = excludeFolders;
        mOnChanged = onChanged;

        mThread = new RunnableThread();
        T3D_ASSERT(mThread);
        TResult ret = mThread->start(this, "FileSystemMonitorThread");
        if (T3D_SUCCEEDED(ret))
        {
            msMonitors.emplace(mPath, this);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FileSystemMonitor::stopMonitor()
    {
        if (mIsRunning)
        {
            stop();
            mThread->wait();
            T3D_SAFE_DELETE(mThread);

            msMonitors.erase(mPath);
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    bool FileSystemMonitor::init()
    {
        TResult ret = mMonitor->init(mPath, mExcludeExts, mExcludeFolders,
            [this](const String &path, FSMonitorAction action)
            {
                ScopeLock lock(mChangedQMutex);
                mChangedItemsQ.emplace_back(action, path);
            });
        mIsRunning = T3D_SUCCEEDED(ret);
        return mIsRunning;
    }

    //--------------------------------------------------------------------------

    TResult FileSystemMonitor::run()
    {
        TResult ret = T3D_OK;

        while (mIsRunning)
        {
            ret = mMonitor->monitor();
            if (T3D_FAILED(ret))
            {
                break;
            }

            RunnableThread::sleep(1000);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void FileSystemMonitor::exit()
    {
        mMonitor->cleanup();
    }

    //--------------------------------------------------------------------------

    void FileSystemMonitor::stop()
    {
        mIsRunning = false;
    }

    //--------------------------------------------------------------------------

    void FileSystemMonitor::update()
    {
        ScopeLock lock(mChangedQMutex);
        while (!mChangedItemsQ.empty())
        {
            auto &item = mChangedItemsQ.front();
            mOnChanged(item.filePath, item.action);
            mChangedItemsQ.pop_front();
        }
    }

    //--------------------------------------------------------------------------

    void FileSystemMonitor::poll()
    {
        for (auto item : msMonitors)
        {
            item.second->update();
        }
    }

    //--------------------------------------------------------------------------
}


