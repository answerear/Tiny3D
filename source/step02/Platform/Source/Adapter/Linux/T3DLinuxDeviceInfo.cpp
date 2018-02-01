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

#include "Adapter/Linux/T3DLinuxDeviceInfo.h"
#include "Adapter/T3DFactoryInterface.h"


namespace Tiny3D
{
    LinuxDeviceInfo::LinuxDeviceInfo()
    {

    }

    LinuxDeviceInfo::~LinuxDeviceInfo()
    {

    }

    uint32_t LinuxDeviceInfo::getPlatform() const
    {
        return E_PLATFORM_LINUX;
    }

    const String &LinuxDeviceInfo::getSoftwareVersion() const
    {
        return "3.0.0.0";
    }

    void LinuxDeviceInfo::setSoftwareVersion(const char *version)
    {
    }

    const String &LinuxDeviceInfo::getOSVersion() const
    {
        return "Windows 7";
    }

    const String &LinuxDeviceInfo::getDeviceVersion() const
    {
        return "PC";
    }

    const String &LinuxDeviceInfo::getSystemInfo() const
    {
        return "";
    }

    int32_t LinuxDeviceInfo::getScreenWidth() const
    {
        return 0;
    }

    int32_t LinuxDeviceInfo::getScreenHeight() const
    {
        return 0;
    }

    float LinuxDeviceInfo::getScreenDPI() const
    {
        return 0.0f;
    }

    const String &LinuxDeviceInfo::getMacAddress() const
    {
        return "12-34-56-78-9A-BC";
    }

    const String &LinuxDeviceInfo::getCPUType() const
    {
        return "Intel Core i5";
    }

    int32_t LinuxDeviceInfo::getNumberOfProcessors() const
    {
        return 1;
    }

    uint32_t LinuxDeviceInfo::getMemoryCapacity() const
    {
        return 0x80000000;
    }

    const String &LinuxDeviceInfo::getDeviceID() const
    {
        return "12-34-56-78-9A-BC";
    }
}
