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

#ifndef __T3D_WIN32_DEVICE_INFO_H__
#define __T3D_WIN32_DEVICE_INFO_H__


#include "Adapter/T3DDeviceInfoInterface.h"
#include "T3DNoncopyable.h"


namespace Tiny3D
{
    class Win32DeviceInfo : public IDeviceInfo, public Noncopyable
    {
    public:
        Win32DeviceInfo();
        virtual ~Win32DeviceInfo();

        /**
         * @brief 获取平台类型
         */
        virtual uint32_t getPlatform() const override;

        /**
         * @brief 获取软件版本号字符串
         */
        virtual const String &getSoftwareVersion() const override;

        /**
        * @brief 设置软件版本号字符串
        */
        virtual void setSoftwareVersion(const char *version) override;

        /**
         * @brief 获取操作系统版本号字符串
         */
        virtual const String &getOSVersion() const override;
        
        /**
         * @brief 获取设备机型版本信息字符串
         */
        virtual const String &getDeviceVersion() const override;

        /**
         * @brief 获取屏幕宽度.
         */
        virtual int32_t getScreenWidth() const override;

        /**
         * @brief 获取屏幕高度.
         */
        virtual int32_t getScreenHeight() const override;

        /**
         * @brief 获取屏幕像素密度.
         */
        virtual float getScreenDPI() const override;

        /**
         * @brief 获取CPU类型信息.
         */
        virtual const String &getCPUType() const override;

        /**
        * @brief 获取CPU架构.
        */
        virtual const String &getCPUArchitecture() const override;

        /**
         * @brief 获取CPU核数
         */
        virtual int32_t getCPUCores() const override;

        /**
         * @brief 获取内存信息.
         */
        virtual uint64_t getSystemRAM() const override;

        /**
         * @brief 获取设备ID.
         */
        virtual const String &getDeviceID() const override;

    private:
        String getOSInfo() const;

        String getMacAddress() const;

        String getMainboardUUID() const;

        String getCPUID() const;

        void collectSystemInfo();

        /**
         * @brief 收集操作系统信息
         */
        void collectOSInfo();

        /**
         * @brief 收集CPU信息
         */
        void collectCPUInfo();

        /**
         * @brief 收集内存信息
         */
        void collectRAMInfo();

        /**
         * @brief 收集设备硬件信息
         */
        void collectDeviceInfo();

        /**
         * @brief 收集屏幕信息
         */
        void collectScreenInfo();

        String      mSWVersion;
        String      mOSVersion;
        String      mHWVersion;
        String      mCPUType;
        String      mCPUArchitecture;
        String      mDeviceID;
        int32_t     mCPUCores;
        uint64_t    mSystemRAM;
        int32_t     mScreenWidth;
        int32_t     mScreenHeight;
        float       mScreenDPI;
    };
}


#endif  /*__T3D_WIN32_DEVICE_INFO_H__*/