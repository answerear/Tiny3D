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

#include <filesystem>

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
    }

    //--------------------------------------------------------------------------

    FileSystemMonitor::~FileSystemMonitor()
    {
        stopMonitor();
        T3D_SAFE_DELETE(mMonitor)
    }

    //--------------------------------------------------------------------------

    TResult FileSystemMonitor::startMonitor(const String &path, const FSMonitorExts &excludeExts, const FSMonitorExcludes &excludeFolders, const FSMonitorOnChanged &onChanged)
    {
        TResult ret = T3D_ERR_NOT_IMPLEMENT;
        
        if (mMonitor != nullptr)
        {
            ret = mMonitor->startWatching(path, excludeExts, excludeExts, onChanged);
            if (T3D_SUCCEEDED(ret))
            {
                msMonitors.emplace(path, this);
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult FileSystemMonitor::stopMonitor()
    {
        TResult ret = T3D_ERR_NOT_IMPLEMENT;
        
        if (mMonitor != nullptr)
        {
            ret = mMonitor->stopWatching();
            if (T3D_SUCCEEDED(ret))
            {
                msMonitors.erase(mMonitor->getPath());
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void FileSystemMonitor::update()
    {
        if (mMonitor != nullptr)
        {
            mMonitor->poll();
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


