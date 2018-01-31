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

#include "Adapter/iOS/T3DiOSDeviceInfo.h"
#include "Adapter/T3DFactoryInterface.h"
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sstream>
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

namespace Tiny3D
{
    iOSDeviceInfo::iOSDeviceInfo()
    {

    }

    iOSDeviceInfo::~iOSDeviceInfo()
    {

    }

    uint32_t iOSDeviceInfo::getPlatform() const
    {
        return E_PLATFORM_WIN32;
    }

    const String &iOSDeviceInfo::getSoftwareVersion() const
    {
        if (mSWVersion.empty())
        {
            NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
            NSDictionary *infoDictionary = [[NSBundle mainBundle] infoDictionary];
            NSString *version = [infoDictionary objectForKey:@"CFBundleVersion"];
            mSWVersion = [version UTF8String];
            [pool release];
        }
        
        return mSWVersion;
    }

    void iOSDeviceInfo::setSoftwareVersion(const char *version)
    {
        
    }

    const String &iOSDeviceInfo::getOSVersion() const
    {
        if (mOSVersion.empty())
        {
            NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
            String version = [[UIDevice currentDevice].systemVersion UTF8String];
            [pool release];
            mOSVersion = "iOS " + version;
        }
        
        return mOSVersion;
    }

    const String &iOSDeviceInfo::getDeviceVersion() const
    {
        if (mHWVersion.empty())
        {
            char buf[64];
            size_t buflen = 64;
            sysctlbyname("hw.machine", buf, &buflen, nullptr, 0);
            mHWVersion = buf;
        }
        
        return mHWVersion;
    }

    const String &iOSDeviceInfo::getSystemInfo() const
    {
        if (mSystemInfo.empty())
        {
            // OS Version
            std::stringstream ss;
            ss<<"Operating System : "<<getOSVersion()<<"\n";
            // CPU Architecture
            // CPU Type
            ss<<"CPU Type : "<<getCPUType()<<"\n";
            // Device Version
            ss<<"Device Version : "<<getDeviceVersion()<<"\n";
            // Number of CPU Processor
            ss<<"CPU Processor : "<<getNumberOfProcessors()<<"\n";
            mSystemInfo = ss.str();
        }
        
        return mSystemInfo;
    }

    int32_t iOSDeviceInfo::getScreenWidth() const
    {
        return 0;
    }

    int32_t iOSDeviceInfo::getScreenHeight() const
    {
        return 0;
    }

    float iOSDeviceInfo::getScreenDPI() const
    {
        return 0.0f;
    }

    const String &iOSDeviceInfo::getMacAddress() const
    {
        return "12-34-56-78-9A-BC";
    }

    const String &iOSDeviceInfo::getCPUType() const
    {
        return "Intel Core i5";
    }

    int32_t iOSDeviceInfo::getNumberOfProcessors() const
    {
        return 1;
    }

    uint32_t iOSDeviceInfo::getMemoryCapacity() const
    {
        return 0x80000000;
    }

    const String &iOSDeviceInfo::getDeviceID() const
    {
        return "12-34-56-78-9A-BC";
    }
}
