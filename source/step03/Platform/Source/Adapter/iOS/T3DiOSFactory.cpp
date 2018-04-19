﻿/*******************************************************************************
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

#include "T3DiOSFactory.h"
#include "Adapter/Common/T3DSDLApplication.h"
#include "Adapter/Mobile/T3DSDLMobileWindow.h"
#include "Adapter/Unix/T3DUnixTime.h"
#include "Adapter/iOS/T3DiOSDir.h"
#include "Adapter/iOS/T3DiOSDeviceInfo.h"
#include "Adapter/iOS/T3DiOSConsole.h"

namespace Tiny3D
{
    iOSFactory::iOSFactory()
    {

    }

    iOSFactory::~iOSFactory()
    {

    }

    IApplication *iOSFactory::createPlatformApplication()
    {
        return new SDLApplication();
    }

    IWindow *iOSFactory::createPlatformWindow()
    {
        return new SDLMobileWindow();
    }

	ITime *iOSFactory::createPlatformTime()
	{
		return new UnixTime();
	}
	
	IDir *iOSFactory::createPlatformDir()
	{
		return new iOSDir();
	}
	
	IDeviceInfo *iOSFactory::createPlatformDeviceInfo()
	{
		return new iOSDeviceInfo();
	}
	
	IConsole *iOSFactory::createPlatformConsole()
	{
		return new iOSConsole();
	}
	
    EPlatform iOSFactory::getPlatform()
    {
        return E_PLATFORM_IOS;
    }

    IFactory *createPlatformFactory()
    {
        return new iOSFactory();
    }
}