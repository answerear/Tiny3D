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

#include "T3DSystem.h"
#include "Adapter/T3DFactoryInterface.h"
#include "IO/T3DDir.h"
#include "Codec/T3DTextCodec.h"
#include "Console/T3DConsole.h"
#include "Device/T3DDeviceInfo.h"
#include "Time/T3DRunLoop.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(System);

    System::System()
        : m_pAdapterFactory(nullptr)
        , m_pTextCodec(nullptr)
        , m_pConsole(nullptr)
        , m_pDeviceInfo(nullptr)
        , m_pMainRunLoop(nullptr)
    {
        m_pAdapterFactory = createAdapterFactory();

        Dir::getNativeSeparator();

        m_pTextCodec = new TextCodec("GB2312.dat");
        m_pConsole = new Console(m_pAdapterFactory);
        m_pDeviceInfo = new DeviceInfo(m_pAdapterFactory);
        m_pMainRunLoop = new RunLoop();
    }

    System::~System()
    {
        T3D_SAFE_DELETE(m_pMainRunLoop);
        T3D_SAFE_DELETE(m_pConsole);
        T3D_SAFE_DELETE(m_pDeviceInfo);
        T3D_SAFE_DELETE(m_pAdapterFactory);
    }

    void System::process()
    {
        if (m_pMainRunLoop != nullptr)
        {
            m_pMainRunLoop->execute();
        }
    }

    RunLoop &System::getMainRunLoop()
    {
        return *m_pMainRunLoop;
    }
}
