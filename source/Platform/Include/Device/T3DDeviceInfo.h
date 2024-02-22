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

#ifndef __T3D_DEVICE_INFO_H__
#define __T3D_DEVICE_INFO_H__


#include "T3DSingleton.h"
#include "T3DPlatformPrerequisites.h"


namespace Tiny3D
{
    class IDeviceInfo;

    class T3D_PLATFORM_API DeviceInfo : public Singleton<DeviceInfo>
    {
    public:
        static const uint32_t PLATFORM_UNKNOWN;
        static const uint32_t PLATFORM_WINDOWS;
        static const uint32_t PLATFORM_MACOSX;
        static const uint32_t PLATFORM_LINUX;
        static const uint32_t PLATFORM_IOS;
        static const uint32_t PLATFORM_ANDROID;

        static const char* Unknown;
        static const char* Windows;
        static const char* MacOSX;
        static const char* Linux;
        static const char* iOS;
        static const char* Android;

    public:
        DeviceInfo();
        virtual ~DeviceInfo();

        /**
         * @brief 获取平台类型
         */
        uint32_t getPlatform() const;

        /**
         * @brief 获取平台类型字符串
         */
        String getPlatformString() const;

        /**
         * @brief 获取软件版本号字符串
         */
        String getSoftwareVersion() const;

        /**
         * @brief 设置软件版本号字符串
         */
        void setSoftwareVersion(const char *version);

        /**
         * @brief 获取操作系统版本号字符串
         */
        String getOSVersion() const;

        /**
         * @brief 获取设备机型版本信息字符串
         */
        String getDeviceVersion() const;

        /**
         * @brief 获取系统详细信息
         */
        String getSystemInfo() const;

        /**
         * @brief 获取屏幕宽度.
         */
        int32_t getScreenWidth() const;

        /**
         * @brief 获取屏幕高度.
         */
        int32_t getScreenHeight() const;

        /**
         * @brief 获取屏幕像素密度.
         */
        float getScreenDPI() const;

        /**
         * @brief 获取CPU类型信息.
         */
        String getCPUType() const;

        /**
         * @brief 获取CPU架构
         */
        String getCPUArchitecture() const;

        /**
         * @brief 获取CPU核数
         */
        int32_t getCPUCores() const;

        /**
         * @brief 获取系统内存总数.
         */
        uint64_t getSystemRAM() const;
        
        /**
         * @brief 获取设备ID.
         */
        String getDeviceID() const;

    protected:
        IDeviceInfo *mDeviceInfo;
    };

    #define T3D_DEVICE_INFO     DeviceInfo::getInstance()
}


#endif  /*__T3D_DEVICE_INFO_H__*/
