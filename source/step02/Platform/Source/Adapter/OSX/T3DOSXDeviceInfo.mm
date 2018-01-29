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
        return "PC";
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
		NSMutableString *cpu = [[NSMutableString alloc] init];
		size_t size;
		cpu_type_t type;
		cpu_subtype_t subtype;
		size = sizeof(type);
		sysctlbyname("hw.cputype", &type, &size, NULL, 0);
		
		size = sizeof(subtype);
		sysctlbyname("hw.cpusubtype", &subtype, &size, NULL, 0);
		
		// values for cputype and cpusubtype defined in mach/machine.h
		if (type == CPU_TYPE_X86)
		{
			[cpu appendString:@"x86 "];
		}
		else if (type == CPU_TYPE_X86_64)
		{
			
		}
		else if (type == CPU_TYPE_ARM)
		{
			[cpu appendString:@"ARM"];
			[cpu appendFormat:@",Type:%d",subtype];
		}
		
        return "Intel Core i5";
    }

    int32_t OSXDeviceInfo::getNumberOfProcessors() const
    {
		if (mNumberOfProcessor == 0)
		{
			mNumberOfProcessor = [NSProcessInfo processInfo].processorCount;
		}
		
        return mNumberOfProcessor;
    }

    uint32_t OSXDeviceInfo::getMemoryCapacity() const
    {
		if (mMemoryCapacity == 0)
		{
			uint64_t totalMemory = [NSProcessInfo processInfo].physicalMemory;
			mMemoryCapacity = (uint32_t)(totalMemory / (1024 * 1024));
		}
        return mMemoryCapacity;
    }

    const String &OSXDeviceInfo::getDeviceID() const
    {
        return "12-34-56-78-9A-BC";
    }
}
