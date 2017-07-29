/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Aaron Wong
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

#include "T3DDeviceInfo_Windows.h"
#include "Adapter/T3DFactoryInterface.h"


namespace Tiny3D
{
    DeviceInfo_Windows::DeviceInfo_Windows()
    {

    }

    DeviceInfo_Windows::~DeviceInfo_Windows()
    {

    }

    uint32_t DeviceInfo_Windows::getPlatform() const
    {
        return E_PLATFORM_WIN32;
    }

    String DeviceInfo_Windows::getSoftwareVersion() const
    {
        return "3.0.0.0";
    }

    String DeviceInfo_Windows::getOSVersion() const
    {
        return "Windows 7";
    }

    String DeviceInfo_Windows::getDeviceVersion() const
    {
        return "PC";
    }

    int32_t DeviceInfo_Windows::getScreenWidth() const
    {
        return 0;
    }

    int32_t DeviceInfo_Windows::getScreenHeight() const
    {
        return 0;
    }

    float DeviceInfo_Windows::getScreenDPI() const
    {
        return 0.0f;
    }

    String DeviceInfo_Windows::getMacAddress() const
    {
        return "12-34-56-78-9A-BC";
    }

    String DeviceInfo_Windows::getCPUType() const
    {
        return "Intel Core i5";
    }

    int32_t DeviceInfo_Windows::getNumberOfProcessors() const
    {
        return 1;
    }

    uint32_t DeviceInfo_Windows::getMemoryCapacity() const
    {
        return 0x80000000;
    }

    String DeviceInfo_Windows::getDeviceID() const
    {
        return "12-34-56-78-9A-BC";
    }
}
