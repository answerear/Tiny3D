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

#include "Adapter/OSX/T3DOSXDeviceInfo.h"
#include "Adapter/T3DFactoryInterface.h"
#import <Foundation/Foundation.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/machine.h>

namespace Tiny3D
{
    OSXDeviceInfo::OSXDeviceInfo()
		: mOSVersion()
		, mNumberOfProcessor(0)
		, mMemoryCapacity(0)
    {

    }

    OSXDeviceInfo::~OSXDeviceInfo()
    {

    }

    uint32_t OSXDeviceInfo::getPlatform() const
    {
        return E_PLATFORM_OSX;
    }

    const String &OSXDeviceInfo::getSoftwareVersion() const
    {
		
        return "3.0.0.0";
    }
	
	void OSXDeviceInfo::setSoftwareVersion(const char *version)
	{
		
	}

    const String &OSXDeviceInfo::getOSVersion() const
    {
		if (mOSVersion.empty())
		{
			NSString *ver = [[NSProcessInfo processInfo]
							 operatingSystemVersionString];
			mOSVersion = [ver UTF8String];
		}
		
        return mOSVersion;
    }

    const String &OSXDeviceInfo::getDeviceVersion() const
    {
        if (mHWVersion.empty())
        {
            int mib[2];
            mib[0] = CTL_HW;
            mib[1] = HW_MODEL;
            char buf[64];
            size_t buflen = 64;
            sysctl(mib, 2, buf, &buflen, nullptr, 0);
            if (buflen != 0)
                mHWVersion = buf;
        }
        
        return mHWVersion;
    }
	
	const String &OSXDeviceInfo::getSystemInfo() const
	{
		
		return "";
	}

    int32_t OSXDeviceInfo::getScreenWidth() const
    {
        return 0;
    }

    int32_t OSXDeviceInfo::getScreenHeight() const
    {
        return 0;
    }

    float OSXDeviceInfo::getScreenDPI() const
    {
        return 0.0f;
    }

    const String &OSXDeviceInfo::getMacAddress() const
    {
        return "12-34-56-78-9A-BC";
    }

    const String &OSXDeviceInfo::getCPUType() const
    {
        if (mCPUType.empty())
        {
            char buf[64];
            size_t buflen = 64;
            sysctlbyname("machdep.cpu.brand_string", &buf, &buflen, nullptr, 0);
            
            if (buflen != 0)
            {
                mCPUType = buf;
            }
        }
		
        return mCPUType;
    }

    int32_t OSXDeviceInfo::getNumberOfProcessors() const
    {
		if (mNumberOfProcessor == 0)
		{
            int numberOfProcessors = 0;
            size_t size = sizeof(numberOfProcessors);
            sysctlbyname("hw.physicalcpu", &numberOfProcessors, &size, nullptr, 0);
            mNumberOfProcessor = numberOfProcessors;
		}
		
        return mNumberOfProcessor;
    }

    uint32_t OSXDeviceInfo::getMemoryCapacity() const
    {
		if (mMemoryCapacity == 0)
		{
            int mib[2];
            mib[0] = CTL_HW;
            mib[1] = HW_MEMSIZE;
			uint64_t totalMemory = 0;
            size_t size = sizeof(totalMemory);
            sysctl(mib, 2, &totalMemory, &size, nullptr, 0);
			mMemoryCapacity = (uint32_t)(totalMemory / (1024 * 1024));
		}
        
        return mMemoryCapacity;
    }

    const String &OSXDeviceInfo::getDeviceID() const
    {
        return "12-34-56-78-9A-BC";
    }
}
