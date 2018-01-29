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

#include "Adapter/Windows/T3DWin32DeviceInfo.h"
#include "Adapter/T3DFactoryInterface.h"

namespace Tiny3D
{
    Win32DeviceInfo::Win32DeviceInfo()
    {

    }

    Win32DeviceInfo::~Win32DeviceInfo()
    {

    }

    uint32_t Win32DeviceInfo::getPlatform() const
    {
        return E_PLATFORM_WIN32;
    }

    String Win32DeviceInfo::getSoftwareVersion() const
    {
        return "3.0.0.0";
    }

    String Win32DeviceInfo::getOSVersion() const
    {
        return "Windows 7";
    }

    String Win32DeviceInfo::getDeviceVersion() const
    {
        return "PC";
    }

    int32_t Win32DeviceInfo::getScreenWidth() const
    {
        return 0;
    }

    int32_t Win32DeviceInfo::getScreenHeight() const
    {
        return 0;
    }

    float Win32DeviceInfo::getScreenDPI() const
    {
        return 0.0f;
    }

    String Win32DeviceInfo::getMacAddress() const
    {
        return "12-34-56-78-9A-BC";
    }

    String Win32DeviceInfo::getCPUType() const
    {
        return "Intel Core i5";
    }

    int32_t Win32DeviceInfo::getNumberOfProcessors() const
    {
        return 1;
    }

    uint32_t Win32DeviceInfo::getMemoryCapacity() const
    {
        return 0x80000000;
    }

    String Win32DeviceInfo::getDeviceID() const
    {
        return "12-34-56-78-9A-BC";
    }
}
