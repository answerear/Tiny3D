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
    class IDeviceInfo
    {
        T3D_DECLARE_INTERFACE(IDeviceInfo);

    public:
        /**
         * @brief ��ȡƽ̨����
         */
        virtual uint32_t getPlatform() const = 0;

        /**
         * @brief ��ȡ����汾���ַ���
         */
        virtual const String &getSoftwareVersion() const = 0;

        /**
         * @brief ��������汾���ַ���
         */
        virtual void setSoftwareVersion(const char *version) = 0;

        /**
         * @brief ��ȡ����ϵͳ�汾���ַ���
         */
        virtual const String &getOSVersion() const = 0;

        /**
         * @brief ��ȡ�豸���Ͱ汾��Ϣ�ַ���
         */
        virtual const String &getDeviceVersion() const = 0;

        /**
         * @brief ��ȡϵͳ��ϸ��Ϣ�ַ���
         */
        virtual const String &getSystemInfo() const = 0;

        /**
         * @brief ��ȡ��Ļ���.
         */
        virtual int32_t getScreenWidth() const = 0;

        /**
         * @brief ��ȡ��Ļ�߶�.
         */
        virtual int32_t getScreenHeight() const = 0;

        /**
         * @brief ��ȡ��Ļ�����ܶ�.
         */
        virtual float getScreenDPI() const = 0;

        /**
         * @brief ��ȡ�豸mac��ַ.
         */
        virtual const String &getMacAddress() const = 0;

        /**
         * @brief ��ȡCPU������Ϣ.
         */
        virtual const String &getCPUType() const = 0;

        /**
         * @brief ��ȡCPU����.
         */
        virtual int32_t getNumberOfProcessors() const = 0;

        /**
         * @brief ��ȡ�ڴ���Ϣ.
         */
        virtual uint32_t getMemoryCapacity() const = 0;

        /**
         * @brief ��ȡ�豸ID.
         */
        virtual const String &getDeviceID() const = 0;
    };
}


#endif  /*__T3D_DEVICE_INFO_INTERFACE_H__*/
