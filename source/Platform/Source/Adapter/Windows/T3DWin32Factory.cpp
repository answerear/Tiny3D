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

#include "T3DWin32Factory.h"
#include "Adapter/Common/T3DSDLApplication.h"
#include "Adapter/Common/T3DTimerService.h"
#include "Adapter/Desktop/T3DSDLDesktopWindow.h"
#include "Adapter/Windows/T3DWin32Time.h"
#include "Adapter/Windows/T3DWin32Dir.h"
#include "Adapter/Windows/T3DWin32DeviceInfo.h"
#include "Adapter/Windows/T3DWin32Console.h"
#include "Adapter/Windows/T3DWin32Thread.h"
#include "Adapter/Windows/T3DWin32SyncObject.h"
#include "Adapter/Windows/T3DWin32Process.h"
#include "Adapter/Windows/T3DWin32Locale.h"
#include "Adapter/Windows/T3DWin32Platform.h"
#include "Adapter/Windows/T3DWin32FSMonitor.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    IPlatform *Win32Factory::createPlatform()
    {
        return T3D_NEW Win32Platform();
    }
    
    //--------------------------------------------------------------------------

    IApplication *Win32Factory::createPlatformApplication()
    {
        return T3D_NEW SDLApplication();
    }

    //--------------------------------------------------------------------------

    IWindow *Win32Factory::createPlatformWindow()
    {
        return T3D_NEW SDLDesktopWindow();
    }

    //--------------------------------------------------------------------------

    ITime *Win32Factory::createPlatformTime()
    {
        return T3D_NEW Win32Time();
    }

    //--------------------------------------------------------------------------

    ITimerService *Win32Factory::createPlatformTimerService()
    {
        return T3D_NEW TimerService();
    }

    //--------------------------------------------------------------------------

    IDir *Win32Factory::createPlatformDir()
    {
        return T3D_NEW Win32Dir();
    }

    //--------------------------------------------------------------------------

    IDeviceInfo *Win32Factory::createPlatformDeviceInfo()
    {
        return T3D_NEW Win32DeviceInfo();
    }

    //--------------------------------------------------------------------------

    IConsole *Win32Factory::createPlatformConsole()
    {
        return T3D_NEW Win32Console();
    }

    //--------------------------------------------------------------------------

    IThread *Win32Factory::createPlatformThread()
    {
        return T3D_NEW Win32Thread();
    }

    //--------------------------------------------------------------------------
    
    ICriticalSection *Win32Factory::createPlatformCriticalSection()
    {
        return T3D_NEW Win32CriticalSection();
    }

    //--------------------------------------------------------------------------
    
    IMutex *Win32Factory::createPlatformMutex()
    {
        return T3D_NEW Win32Mutex();
    }

    //--------------------------------------------------------------------------

    IRecursiveMutex *Win32Factory::createPlatformRecursiveMutex()
    {
        return T3D_NEW Win32RecursiveMutex();
    }

    //--------------------------------------------------------------------------
    
    ISemaphore *Win32Factory::createPlatformSemaphore()
    {
        return T3D_NEW Win32Semaphore();
    }

    //--------------------------------------------------------------------------
    
    IEvent *Win32Factory::createPlatformEvent()
    {
        return T3D_NEW Win32Event();
    }

    //--------------------------------------------------------------------------
    
    IWaitCondition *Win32Factory::createPlatformWaitCondition()
    {
        return T3D_NEW Win32WaitCondition();
    }
    
    //--------------------------------------------------------------------------

    IProcess *Win32Factory::createPlatformProcess()
    {
        return T3D_NEW Win32Process();
    }
    
    //--------------------------------------------------------------------------

    ILocale *Win32Factory::createPlatformLocale()
    {
        return T3D_NEW Win32Locale();
    }

    //--------------------------------------------------------------------------

    IFSMonitor *Win32Factory::createFileSystemMonitor()
    {
        return T3D_NEW Win32FSMonitor();
    }

    //--------------------------------------------------------------------------

    EPlatform Win32Factory::getPlatform()
    {
        return E_PLATFORM_WIN32;
    }

    //--------------------------------------------------------------------------

    IFactory *createPlatformFactory()
    {
        return T3D_NEW Win32Factory();
    }

    //--------------------------------------------------------------------------
}
