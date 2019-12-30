﻿/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
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


#include "T3DD3D11Capabilities.h"
#include "T3DD3D11Renderer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11CapabilitiesPtr D3D11Capabilities::create()
    {
        D3D11CapabilitiesPtr cap = new D3D11Capabilities();
        if (cap == nullptr || cap->init() != T3D_OK)
        {
            cap = nullptr;
        }
        return cap;
    }

    //--------------------------------------------------------------------------

    D3D11Capabilities::D3D11Capabilities()
    {

    }

    //--------------------------------------------------------------------------

    D3D11Capabilities::~D3D11Capabilities()
    {

    }

    //--------------------------------------------------------------------------

    TResult D3D11Capabilities::init()
    {
        TResult ret = T3D_OK;

        mDriverVersion = D3D11_RENDERER.getDriverVersion();
        mDeviceName = D3D11_RENDERER.getDeviceName();

        mCategoryRelevant[CapabilitiesCategory::DIRECTX] = true;

        return ret;
    }
}