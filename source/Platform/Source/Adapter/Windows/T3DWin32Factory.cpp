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


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    IPlatform *Win32Factory::createPlatform()
    {
        return new Win32Platform();
    }
    
    //--------------------------------------------------------------------------

    IApplication *Win32Factory::createPlatformApplication()
    {
        return new SDLApplication();
    }

    //--------------------------------------------------------------------------

    IWindow *Win32Factory::createPlatformWindow()
    {
        return new SDLDesktopWindow();
    }

    //--------------------------------------------------------------------------

    ITime *Win32Factory::createPlatformTime()
    {
        return new Win32Time();
    }

    //--------------------------------------------------------------------------

    ITimerService *Win32Factory::createPlatformTimerService()
    {
        return new TimerService();
    }

    //--------------------------------------------------------------------------

    IDir *Win32Factory::createPlatformDir()
    {
        return new Win32Dir();
    }

    //--------------------------------------------------------------------------

    IDeviceInfo *Win32Factory::createPlatformDeviceInfo()
    {
        return new Win32DeviceInfo();
    }

    //--------------------------------------------------------------------------

    IConsole *Win32Factory::createPlatformConsole()
    {
        return new Win32Console();
    }

    //--------------------------------------------------------------------------

    IThread *Win32Factory::createPlatformThread()
    {
        return new Win32Thread();
    }

    //--------------------------------------------------------------------------
    
    ICriticalSection *Win32Factory::createPlatformCriticalSection()
    {
        return new Win32CriticalSection();
    }

    //--------------------------------------------------------------------------
    
    IMutex *Win32Factory::createPlatformMutex()
    {
        return new Win32Mutex();
    }

    //--------------------------------------------------------------------------

    IRecursiveMutex *Win32Factory::createPlatformRecursiveMutex()
    {
        return new Win32RecursiveMutex();
    }

    //--------------------------------------------------------------------------
    
    ISemaphore *Win32Factory::createPlatformSemaphore()
    {
        return new Win32Semaphore();
    }

    //--------------------------------------------------------------------------
    
    IEvent *Win32Factory::createPlatformEvent()
    {
        return new Win32Event();
    }

    //--------------------------------------------------------------------------
    
    IWaitCondition *Win32Factory::createPlatformWaitCondition()
    {
        return new Win32WaitCondition();
    }
    
    //--------------------------------------------------------------------------

    IProcess *Win32Factory::createPlatformProcess()
    {
        return new Win32Process();
    }
    
    //--------------------------------------------------------------------------

    ILocale *Win32Factory::createPlatformLocale()
    {
        return new Win32Locale();
    }

    //--------------------------------------------------------------------------

    EPlatform Win32Factory::getPlatform()
    {
        return E_PLATFORM_WIN32;
    }

    //--------------------------------------------------------------------------

    IFactory *createPlatformFactory()
    {
        return new Win32Factory();
    }

    //--------------------------------------------------------------------------
}
