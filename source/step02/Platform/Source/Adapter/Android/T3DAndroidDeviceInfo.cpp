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

#include "Adapter/Android/T3DAndroidDeviceInfo.h"
#include "Adapter/T3DFactoryInterface.h"


namespace Tiny3D
{
    AndroidDeviceInfo::AndroidDeviceInfo()
    {

    }

    AndroidDeviceInfo::~AndroidDeviceInfo()
    {

    }

    uint32_t AndroidDeviceInfo::getPlatform() const
    {
        return E_PLATFORM_ANDROID;
    }

    const String &AndroidDeviceInfo::getSoftwareVersion() const
    {
        return "3.0.0.0";
    }

    void AndroidDeviceInfo::setSoftwareVersion(const char *version)
    {

    }

    const String &AndroidDeviceInfo::getOSVersion() const
    {
        return "Windows 7";
    }

    const String &AndroidDeviceInfo::getDeviceVersion() const
    {
        return "PC";
    }

    int32_t AndroidDeviceInfo::getScreenWidth() const
    {
        return 0;
    }

    int32_t AndroidDeviceInfo::getScreenHeight() const
    {
        return 0;
    }

    float AndroidDeviceInfo::getScreenDPI() const
    {
        return 0.0f;
    }

    const String &AndroidDeviceInfo::getCPUType() const
    {
        return "Intel Core i5";
    }

    const String &AndroidDeviceInfo::getCPUArchitecture() const
    {
        return "ARM";
    }

    int32_t AndroidDeviceInfo::getCPUCores() const
    {
        return 1;
    }

    uint32_t AndroidDeviceInfo::getSystemRAM() const
    {
        return 0x80000000;
    }

    const String &AndroidDeviceInfo::getDeviceID() const
    {
        return "12-34-56-78-9A-BC";
    }
}
