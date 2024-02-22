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

#include "Device/T3DDeviceInfo.h"
#include "Adapter/T3DDeviceInfoInterface.h"
#include "Adapter/T3DFactoryInterface.h"
#include "T3DPlatform.h"

#include <sstream>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const uint32_t DeviceInfo::PLATFORM_UNKNOWN = E_PLATFORM_UNKNOWN;
    const uint32_t DeviceInfo::PLATFORM_WINDOWS = E_PLATFORM_WIN32;
    const uint32_t DeviceInfo::PLATFORM_MACOSX = E_PLATFORM_OSX;
    const uint32_t DeviceInfo::PLATFORM_LINUX = E_PLATFORM_LINUX;
    const uint32_t DeviceInfo::PLATFORM_IOS = E_PLATFORM_IOS;
    const uint32_t DeviceInfo::PLATFORM_ANDROID = E_PLATFORM_ANDROID;

    const char* DeviceInfo::Unknown = "Unknown";
    const char* DeviceInfo::Windows = "Windows";
    const char* DeviceInfo::MacOSX = "Mac OSX";
    const char* DeviceInfo::Linux = "Linux";
    const char* DeviceInfo::iOS = "iOS";
    const char* DeviceInfo::Android = "Android";

    //--------------------------------------------------------------------------

    DeviceInfo::DeviceInfo()
        : mDeviceInfo(nullptr)
    {
        mDeviceInfo = T3D_PLATFORM_FACTORY.createPlatformDeviceInfo();
    }

    //--------------------------------------------------------------------------

    DeviceInfo::~DeviceInfo()
    {
        T3D_SAFE_DELETE(mDeviceInfo);
    }

    //--------------------------------------------------------------------------

    uint32_t DeviceInfo::getPlatform() const
    {
        uint32_t unPlatform = PLATFORM_UNKNOWN;

        if (mDeviceInfo != nullptr)
        {
            unPlatform = mDeviceInfo->getPlatform();
        }

        return unPlatform;
    }

    //--------------------------------------------------------------------------

    String DeviceInfo::getPlatformString() const
    {
        String strPlatform = Unknown;

        if (mDeviceInfo != nullptr)
        {
            switch (mDeviceInfo->getPlatform())
            {
            case PLATFORM_WINDOWS:
                {
                    strPlatform = Windows;
                }
                break;
            case PLATFORM_MACOSX:
                {
                    strPlatform = MacOSX;
                }
                break;
            case PLATFORM_LINUX:
                {
                    strPlatform = Linux;
                }
                break;
            case PLATFORM_IOS:
                {
                    strPlatform = iOS;
                }
                break;
            case PLATFORM_ANDROID:
                {
                    strPlatform = Android;
                }
                break;
            }
        }

        return strPlatform;
    }

    //--------------------------------------------------------------------------

    String DeviceInfo::getSoftwareVersion() const
    {
        if (mDeviceInfo != nullptr)
        {
            return mDeviceInfo->getSoftwareVersion();
        }
        return "";
    }

    //--------------------------------------------------------------------------

    void DeviceInfo::setSoftwareVersion(const char *version)
    {
        if (mDeviceInfo != nullptr)
        {
            mDeviceInfo->setSoftwareVersion(version);
        }
    }

    //--------------------------------------------------------------------------

    String DeviceInfo::getOSVersion() const
    {
        if (mDeviceInfo != nullptr)
        {
            return mDeviceInfo->getOSVersion();
        }
        return "";
    }

    //--------------------------------------------------------------------------

    String DeviceInfo::getDeviceVersion() const
    {
        if (mDeviceInfo != nullptr)
        {
            return mDeviceInfo->getDeviceVersion();
        }
        return "";
    }

    //--------------------------------------------------------------------------

    String DeviceInfo::getSystemInfo() const
    {
        std::stringstream ss;

        // OS Version
        ss << "Operating System : " << getOSVersion() << std::endl;
        // Device Version
        ss << "Device Version : " << getDeviceVersion() << std::endl;
        // Device ID
        ss << "Device ID : " << getDeviceID() << std::endl;
        // CPU Type
        ss << "CPU Type : " << getCPUType() << std::endl;
        // CPU Architecture
        ss << "CPU Architecture : " << getCPUArchitecture() << std::endl;
        // CPU Cores
        ss << "CPU Cores : " << getCPUCores() << std::endl;
        // System RAM
        ss << "System RAM : " << getSystemRAM() << "MB" << std::endl;
        // Screen size & DPI
        ss << "Screen Size : " << getScreenWidth() << "x" << getScreenHeight() << std::endl;
        ss << "Screen DPI : " << getScreenDPI() << std::endl;

        return ss.str();
    }

    //--------------------------------------------------------------------------

    int32_t DeviceInfo::getScreenWidth() const
    {
        if (mDeviceInfo != nullptr)
        {
            return mDeviceInfo->getScreenWidth();
        }
        return 0;
    }

    //--------------------------------------------------------------------------

    int32_t DeviceInfo::getScreenHeight() const
    {
        if (mDeviceInfo != nullptr)
        {
            return mDeviceInfo->getScreenHeight();
        }
        return 0;
    }

    //--------------------------------------------------------------------------

    float DeviceInfo::getScreenDPI() const
    {
        if (mDeviceInfo != nullptr)
        {
            return mDeviceInfo->getScreenDPI();
        }
        return 0.0f;
    }

    //--------------------------------------------------------------------------

    String DeviceInfo::getCPUType() const
    {
        if (mDeviceInfo != nullptr)
        {
            return mDeviceInfo->getCPUType();
        }
        return "";
    }

    //--------------------------------------------------------------------------

    String DeviceInfo::getCPUArchitecture() const
    {
        if (mDeviceInfo != nullptr)
        {
            return mDeviceInfo->getCPUArchitecture();
        }

        return "";
    }

    //--------------------------------------------------------------------------

    int32_t DeviceInfo::getCPUCores() const
    {
        if (mDeviceInfo != nullptr)
        {
            return mDeviceInfo->getCPUCores();
        }
        return 1;
    }

    //--------------------------------------------------------------------------

    uint64_t DeviceInfo::getSystemRAM() const
    {
        if (mDeviceInfo != nullptr)
        {
            return mDeviceInfo->getSystemRAM();
        }
        return 0;
    }

    //--------------------------------------------------------------------------

    String DeviceInfo::getDeviceID() const
    {
        if (mDeviceInfo != nullptr)
        {
            return mDeviceInfo->getDeviceID();
        }
        return "";
    }
}
