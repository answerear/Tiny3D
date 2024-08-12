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

#include "T3DPlatform.h"
#include "Adapter/T3DPlatformInterface.h"
#include "Adapter/T3DFactoryInterface.h"
#include "Time/T3DTimerManager.h"
#include "IO/T3DDir.h"
#include "Console/T3DConsole.h"
#include "Locale/T3DLocale.h"
#include "Device/T3DDeviceInfo.h"
#include "Thread/T3DRunnableThread.h"
#include "Thread/T3DThreadManager.h"
#include "T3DPlatformErrorDef.h"
#include "IO/T3DFileSystemMonitor.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Platform::Platform()
    {
        mPlatformFactory = createPlatformFactory();
        mPlatform = mPlatformFactory->createPlatform();
        Dir::getNativeSeparator();
        mThreadMgr = new ThreadManager();
        mConsole = new Console();
        mLocale = new Locale();
        mDeviceInfo = new DeviceInfo();
        mTimerMgr = new TimerManager();
    }

    //--------------------------------------------------------------------------

    Platform::~Platform()
    {
        T3D_SAFE_DELETE(mTimerMgr);
        T3D_SAFE_DELETE(mDeviceInfo);
        T3D_SAFE_DELETE(mConsole);
        T3D_SAFE_DELETE(mLocale);
        T3D_SAFE_DELETE(mThreadMgr);
        T3D_SAFE_DELETE(mPlatform);
        T3D_SAFE_DELETE(mPlatformFactory);
    }

    //--------------------------------------------------------------------------

    int32_t Platform::init()
    {
        int32_t ret = T3D_OK;

        do
        {
            String strCachePath = Dir::getCachePath();
            if (!Dir::exists(strCachePath))
            {
                if (!Dir::makeDir(strCachePath))
                    break;
            }
            
            ret  = mTimerMgr->init();
            if (T3D_FAILED(ret))
                break;
            
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void Platform::poll()
    {
        mTimerMgr->pollEvents();
        FileSystemMonitor::poll();
    }
    
    //--------------------------------------------------------------------------

    void Platform::memoryBarrier()
    {
        mPlatform->memoryBarrier();
    }

    //--------------------------------------------------------------------------
}
