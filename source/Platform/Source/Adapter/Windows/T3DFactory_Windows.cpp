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

#include "T3DFactory_Windows.h"
#include "T3DConsole_Windows.h"
#include "T3DDir_Windows.h"
#include "T3DDeviceInfo_Windows.h"
#include "T3DTimer_Windows.h"
#include "T3DTimerContainer.h"


namespace Tiny3D
{
    FactoryInterface *createAdapterFactory()
    {
        return new Factory_Windows();
    }

    Factory_Windows::Factory_Windows()
        : m_pTimerContainer(new TimerContainer())
    {

    }

    Factory_Windows::~Factory_Windows()
    {
        T3D_SAFE_DELETE(m_pTimerContainer);
    }

    ConsoleInterface *Factory_Windows::createConsoleAdapter()
    {
        return new Console_Windows();
    }

    TimerInterface *Factory_Windows::createTimerAdapter()
    {
        return new Timer_Windows();
    }

    DirInterface *Factory_Windows::createDirAdapter()
    {
        return new Dir_Windows();
    }

    DeviceInfoInterface *Factory_Windows::createDeviceInfoAdapter()
    {
        return new DeviceInfo_Windows();
    }

    EPlatform Factory_Windows::getPlatform()
    {
        return E_PLATFORM_WIN32;
    }
}