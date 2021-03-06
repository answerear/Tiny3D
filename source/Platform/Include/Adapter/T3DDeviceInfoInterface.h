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

#ifndef __T3D_DEVICE_INFO_INTERFACE_H__
#define __T3D_DEVICE_INFO_INTERFACE_H__


#include "T3DType.h"
#include "T3DMacro.h"


namespace Tiny3D
{
    class DeviceInfoInterface
    {
        T3D_DECLARE_INTERFACE(DeviceInfoInterface);

    public:
        /**
         * @brief 获取平台类型
         */
        virtual uint32_t getPlatform() const = 0;

        /**
         * @brief 获取软件版本号字符串
         */
        virtual String getSoftwareVersion() const = 0;

        /**
         * @brief 获取操作系统版本号字符串
         */
        virtual String getOSVersion() const = 0;

        /**
         * @brief 获取设备机型版本信息字符串
         */
        virtual String getDeviceVersion() const = 0;

        /**
         * @brief 获取屏幕宽度.
         */
        virtual int32_t getScreenWidth() const = 0;

        /**
         * @brief 获取屏幕高度.
         */
        virtual int32_t getScreenHeight() const = 0;

        /**
         * @brief 获取屏幕像素密度.
         */
        virtual float getScreenDPI() const = 0;

        /**
         * @brief 获取设备mac地址.
         */
        virtual String getMacAddress() const = 0;

        /**
         * @brief 获取CPU类型信息.
         */
        virtual String getCPUType() const = 0;

        /**
         * @brief 获取CPU核数.
         */
        virtual int32_t getNumberOfProcessors() const = 0;

        /**
         * @brief 获取内存信息.
         */
        virtual uint32_t getMemoryCapacity() const = 0;

        /**
         * @brief 获取设备ID.
         */
        virtual String getDeviceID() const = 0;
    };
}


#endif  /*__T3D_DEVICE_INFO_INTERFACE_H__*/
