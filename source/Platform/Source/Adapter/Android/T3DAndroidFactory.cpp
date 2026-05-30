/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "Adapter/Android/T3DAndroidFactory.h"
#include "Adapter/Common/T3DSDLApplication.h"
#include "Adapter/Common/T3DTimerService.h"
#include "Adapter/Mobile/T3DSDLMobileWindow.h"
#include "Adapter/Unix/T3DUnixTime.h"
#include "Adapter/Android/T3DAndroidDir.h"
#include "Adapter/Android/T3DAndroidConsole.h"
#include "Adapter/Android/T3DAndroidDeviceInfo.h"
#include "Adapter/Android/T3DAndroidPlatform.h"
#include "Adapter/Android/T3DAndroidThread.h"
#include "Adapter/Android/T3DAndroidSyncObject.h"
#include "Adapter/Android/T3DAndroidFSMonitor.h"
#include "Adapter/Unix/T3DPosixSyncObject.h"
#include "Adapter/Unix/T3DPosixProcess.h"
#include "Adapter/Unix/T3DPosixLocale.h"


namespace Tiny3D
{
    AndroidFactory::AndroidFactory()
    {

    }

    AndroidFactory::~AndroidFactory()
    {

    }

    IPlatform *AndroidFactory::createPlatform()
    {
        return T3D_NEW AndroidPlatform();
    }

    IApplication *AndroidFactory::createPlatformApplication()
    {
        return T3D_NEW SDLApplication();
    }

    IWindow *AndroidFactory::createPlatformWindow()
    {
        return T3D_NEW SDLMobileWindow();
    }

    ITime *AndroidFactory::createPlatformTime()
    {
        return T3D_NEW UnixTime();
    }

    ITimerService *AndroidFactory::createPlatformTimerService()
    {
        return T3D_NEW TimerService();
    }

    IDir *AndroidFactory::createPlatformDir()
    {
        return T3D_NEW AndroidDir();
    }

    IDeviceInfo *AndroidFactory::createPlatformDeviceInfo()
    {
        return T3D_NEW AndroidDeviceInfo();
    }

    IConsole *AndroidFactory::createPlatformConsole()
    {
        return T3D_NEW AndroidConsole();
    }

    IThread *AndroidFactory::createPlatformThread()
    {
        return T3D_NEW AndroidThread();
    }

    ICriticalSection *AndroidFactory::createPlatformCriticalSection()
    {
        return T3D_NEW AndroidCriticalSection();
    }

    IMutex *AndroidFactory::createPlatformMutex()
    {
        return T3D_NEW AndroidMutex();
    }

    IRecursiveMutex *AndroidFactory::createPlatformRecursiveMutex()
    {
        return T3D_NEW AndroidRecursiveMutex();
    }

    ISemaphore *AndroidFactory::createPlatformSemaphore()
    {
        return T3D_NEW AndroidSemaphore();
    }

    IEvent *AndroidFactory::createPlatformEvent()
    {
        return T3D_NEW PosixEvent();
    }

    IWaitCondition *AndroidFactory::createPlatformWaitCondition()
    {
        return T3D_NEW PosixWaitCondition();
    }

    IProcess *AndroidFactory::createPlatformProcess()
    {
        return T3D_NEW PosixProcess();
    }

    ILocale *AndroidFactory::createPlatformLocale()
    {
        return T3D_NEW PosixLocale();
    }

    IFSMonitor *AndroidFactory::createFileSystemMonitor()
    {
        return T3D_NEW AndroidFSMonitor();
    }

    EPlatform AndroidFactory::getPlatform()
    {
        return E_PLATFORM_ANDROID;
    }

    IFactory *createPlatformFactory()
    {
        return T3D_NEW AndroidFactory();
    }
}

