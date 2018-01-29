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

#ifndef __T3D_OSX_DEVICE_INFO_H__
#define __T3D_OSX_DEVICE_INFO_H__


#include "Adapter/T3DDeviceInfoInterface.h"


namespace Tiny3D
{
    class OSXDeviceInfo : public IDeviceInfo
    {
        T3D_DISABLE_COPY(OSXDeviceInfo);

    public:
        OSXDeviceInfo();
        virtual ~OSXDeviceInfo();

        /**
         * @brief ��ȡƽ̨����
         */
        virtual uint32_t getPlatform() const override;

        /**
         * @brief ��ȡ����汾���ַ���
         */
        virtual const String &getSoftwareVersion() const override;
		
		virtual void setSoftwareVersion(const char *version) override;

        /**
         * @brief ��ȡ����ϵͳ�汾���ַ���
         */
        virtual const String &getOSVersion() const override;

        /**
         * @brief ��ȡ�豸���Ͱ汾��Ϣ�ַ���
         */
        virtual const String &getDeviceVersion() const override;
		
		virtual const String &getSystemInfo() const override;

        /**
         * @brief ��ȡ��Ļ���.
         */
        virtual int32_t getScreenWidth() const override;

        /**
         * @brief ��ȡ��Ļ�߶�.
         */
        virtual int32_t getScreenHeight() const override;

        /**
         * @brief ��ȡ��Ļ�����ܶ�.
         */
        virtual float getScreenDPI() const override;

        /**
         * @brief ��ȡ�豸mac��ַ.
         */
        virtual const String &getMacAddress() const override;

        /**
         * @brief ��ȡCPU������Ϣ.
         */
        virtual const String &getCPUType() const override;

        /**
         * @brief ��ȡCPU����
         */
        virtual int32_t getNumberOfProcessors() const;

        /**
         * @brief ��ȡ�ڴ���Ϣ.
         */
        virtual uint32_t getMemoryCapacity() const;

        /**
         * @brief ��ȡ�豸ID.
         */
        virtual const String &getDeviceID() const override;
		
	private:
		mutable	String	mOSVersion;
		mutable int32_t	mNumberOfProcessor;
		mutable uint32_t	mMemoryCapacity;
    };
}


#endif  /*__T3D_OSX_DEVICE_INFO_H__*/
