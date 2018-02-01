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

#ifndef __T3D_IOS_DEVICE_INFO_H__
#define __T3D_IOS_DEVICE_INFO_H__


#include "Adapter/T3DDeviceInfoInterface.h"


namespace Tiny3D
{
    class iOSDeviceInfo : public IDeviceInfo
    {
        T3D_DISABLE_COPY(iOSDeviceInfo);

    public:
        iOSDeviceInfo();
        virtual ~iOSDeviceInfo();

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
		 * @brief 获取系统详细信息字符串
		 */
		virtual const String &getSystemInfo() const override;
		
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
         * @brief 获取设备mac地址.
         */
        virtual const String &getMacAddress() const override;

        /**
         * @brief 获取CPU类型信息.
         */
        virtual const String &getCPUType() const override;

        /**
         * @brief 获取CPU核数
         */
        virtual int32_t getNumberOfProcessors() const override;

        /**
         * @brief 获取内存信息.
         */
        virtual uint32_t getMemoryCapacity() const override;

        /**
         * @brief 获取设备ID.
         */
        virtual const String &getDeviceID() const override;
		
    private:
        mutable String  mOSVersion;
        mutable String  mSWVersion;
        mutable String  mHWVersion;
        mutable String  mCPUType;
        mutable String  mSystemInfo;
		mutable	String	mDeviceID;
		
		mutable int32_t		mCPUProcessors;
		mutable uint32_t	mMemoryCapacity;
        
        int32_t mScreenWidth;
        int32_t mScreenHeight;
        float   mDPI;
    };
}


#endif  /*__T3D_IOS_DEVICE_INFO_H__*/
