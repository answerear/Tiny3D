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
#include <sys/utsname.h>
#include <mach/mach_host.h>
#include <sstream>
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

namespace Tiny3D
{
    iOSDeviceInfo::iOSDeviceInfo()
        : mCPUProcessors(0)
        , mMemoryCapacity(0)
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
            String name = [[UIDevice currentDevice].systemName UTF8String];
            String version = [[UIDevice currentDevice].systemVersion UTF8String];
            [pool release];
            mOSVersion = name + " " + version;
        }
        
        return mOSVersion;
    }

    const String &iOSDeviceInfo::getDeviceVersion() const
    {
        if (mHWVersion.empty())
        {
            struct utsname systemInfo;
            uname(&systemInfo);
            String machine = systemInfo.machine;
            
            if (machine == "iPhone3,1")
                mHWVersion = "iPhone 4";
            else if (machine == "iPhone3,2")
                mHWVersion = "iPhone 4";
            else if (machine == "iPhone3,3")
                mHWVersion = "iPhone 4";
            else if (machine == "iPhone4,1")
                mHWVersion = "iPhone 4S";
            else if (machine == "iPhone5,1")
                mHWVersion = "iPhone 5";
            else if (machine == "iPhone5,2")
                mHWVersion = "iPhone 5 (GSM+CDMA)";
            else if (machine == "iPhone5,3")
                mHWVersion = "iPhone 5c (GSM)";
            else if (machine == "iPhone5,4")
                mHWVersion = "iPhone 5c (GSM+CDMA)";
            else if (machine == "iPhone6,1")
                mHWVersion = "iPhone 5s (GSM)";
            else if (machine == "iPhone6,2")
                mHWVersion = "iPhone 5s (GSM+CDMA)";
            else if (machine == "iPhone7,1")
                mHWVersion = "iPhone 6 Plus";
            else if (machine == "iPhone7,2")
                mHWVersion = "iPhone 6";
            else if (machine == "iPhone8,1")
                mHWVersion = "iPhone 6s";
            else if (machine == "iPhone8,2")
                mHWVersion = "iPhone 6s Plus";
            else if (machine == "iPhone8,4")
                mHWVersion = "iPhone SE";
            // 日行两款手机型号均为日本独占，可能使用索尼FeliCa支付方案而不是苹果支付
            else if (machine == "iPhone9,1")
                mHWVersion = "国行、日版、港行iPhone 7";
            else if (machine == "iPhone9,2")
                mHWVersion = "港行、国行iPhone 7 Plus";
            else if (machine == "iPhone9,3")
                mHWVersion = "美版、台版iPhone 7";
            else if (machine == "iPhone9,4")
                mHWVersion = "美版、台版iPhone 7 Plus";
            else if (machine == "iPhone10,1")
                mHWVersion = "国行(A1863)、日行(A1906)iPhone 8";
            else if (machine == "iPhone10,4")
                mHWVersion = "美版(Global/A1905)iPhone 8";
            else if (machine == "iPhone10,2")
                mHWVersion = "国行(A1864)、日行(A1898)iPhone 8 Plus";
            else if (machine == "iPhone10,5")
                mHWVersion = "美版(Global/A1897)iPhone 8 Plus";
            else if (machine == "iPhone10,3")
                mHWVersion = "国行(A1865)、日行(A1902)iPhone X";
            else if (machine == "iPhone10,6")
                mHWVersion = "美版(Global/A1901)iPhone X";
            else if (machine == "iPod1,1")
                mHWVersion = "iPod Touch 1G";
            else if (machine == "iPod2,1")
                mHWVersion = "iPod Touch 2G";
            else if (machine == "iPod3,1")
                mHWVersion = "iPod Touch 3G";
            else if (machine == "iPod4,1")
                mHWVersion = "iPod Touch 4G";
            else if (machine == "iPod5,1")
                mHWVersion = "iPod Touch (5 Gen)";
            else if (machine == "iPad1,1")
                mHWVersion = "iPad";
            else if (machine == "iPad1,2")
                mHWVersion = "iPad 3G";
            else if (machine == "iPad2,1")
                mHWVersion = "iPad 2 (WiFi)";
            else if (machine == "iPad2,2")
                mHWVersion = "iPad 2";
            else if (machine == "iPad2,3")
                mHWVersion = "iPad 2 (CDMA)";
            else if (machine == "iPad2,4")
                mHWVersion = "iPad 2";
            else if (machine == "iPad2,5")
                mHWVersion = "iPad Mini (WiFi)";
            else if (machine == "iPad2,6")
                mHWVersion = "iPad Mini";
            else if (machine == "iPad2,7")
                mHWVersion = "iPad Mini (GSM+CDMA)";
            else if (machine == "iPad3,1")
                mHWVersion = "iPad 3 (WiFi)";
            else if (machine == "iPad3,2")
                mHWVersion = "iPad 3 (GSM+CDMA)";
            else if (machine == "iPad3,3")
                mHWVersion = "iPad 3";
            else if (machine == "iPad3,4")
                mHWVersion = "iPad 4 (WiFi)";
            else if (machine == "iPad3,5")
                mHWVersion = "iPad 4";
            else if (machine == "iPad3,6")
                mHWVersion = "iPad 4 (GSM+CDMA)";
            else if (machine == "iPad4,1")
                mHWVersion = "iPad Air (WiFi)";
            else if (machine == "iPad4,2")
                mHWVersion = "iPad Air (Cellular)";
            else if (machine == "iPad4,4")
                mHWVersion = "iPad Mini 2 (WiFi)";
            else if (machine == "iPad4,5")
                mHWVersion = "iPad Mini 2 (Cellular)";
            else if (machine == "iPad4,6")
                mHWVersion = "iPad Mini 2";
            else if (machine == "iPad4,7")
                mHWVersion = "iPad Mini 3";
            else if (machine == "iPad4,8")
                mHWVersion = "iPad Mini 3";
            else if (machine == "iPad4,9")
                mHWVersion = "iPad Mini 3";
            else if (machine == "iPad5,1")
                mHWVersion = "iPad Mini 4 (WiFi)";
            else if (machine == "iPad5,2")
                mHWVersion = "iPad Mini 4 (LTE)";
            else if (machine == "iPad5,3")
                mHWVersion = "iPad Air 2";
            else if (machine == "iPad5,4")
                mHWVersion = "iPad Air 2";
            else if (machine == "iPad6,3")
                mHWVersion = "iPad Pro 9.7";
            else if (machine == "iPad6,4")
                mHWVersion = "iPad Pro 9.7";
            else if (machine == "iPad6,7")
                mHWVersion = "iPad Pro 12.9";
            else if (machine == "iPad6,8")
                mHWVersion = "iPad Pro 12.9";
            else if (machine == "iPad6,11")
                mHWVersion = "iPad 5 (WiFi)";
            else if (machine == "iPad6,12")
                mHWVersion = "iPad 5 (Cellular)";
            else if (machine == "iPad7,1")
                mHWVersion = "iPad Pro 12.9 inch 2nd gen (WiFi)";
            else if (machine == "iPad7,2")
                mHWVersion = "iPad Pro 12.9 inch 2nd gen (Cellular)";
            else if (machine == "iPad7,3")
                mHWVersion = "iPad Pro 10.5 inch (WiFi)";
            else if (machine == "iPad7,4")
                mHWVersion = "iPad Pro 10.5 inch (Cellular)";
            else if (machine == "AppleTV2,1")
                mHWVersion = "Apple TV 2";
            else if (machine == "AppleTV3,1")
                mHWVersion = "Apple TV 3";
            else if (machine == "AppleTV3,2")
                mHWVersion = "Apple TV 3";
            else if (machine == "AppleTV5,3")
                mHWVersion = "Apple TV 4";
            else if (machine == "i386")
                mHWVersion = "Simulator";
            else if (machine == "x86_64")
                mHWVersion = "Simulator";
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
        return getDeviceID();
    }

    const String &iOSDeviceInfo::getCPUType() const
    {
        return "Intel Core i5";
    }

    int32_t iOSDeviceInfo::getNumberOfProcessors() const
    {
        if (mCPUProcessors == 0)
        {
            NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
            mCPUProcessors = (int32_t)[NSProcessInfo processInfo].processorCount;
            [pool release];
        }
        
        return mCPUProcessors;
    }

    uint32_t iOSDeviceInfo::getMemoryCapacity() const
    {
        if (mMemoryCapacity == 0)
        {
            NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
            mMemoryCapacity = [[NSProcessInfo processInfo] physicalMemory];
            [pool release];
        }
        
        return mMemoryCapacity;
    }

    const String &iOSDeviceInfo::getDeviceID() const
    {
        if (mDeviceID.empty())
        {
            NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
            NSString *uuid = [[[UIDevice currentDevice] identifierForVendor]
                              UUIDString];
            mDeviceID = [uuid UTF8String];
            [pool release];
        }
        
        return mDeviceID;
    }
}
