/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#include "T3DXPlugin.h"
#include "T3DXRenderer.h"


namespace Tiny3D
{
    T3DXPlugin::T3DXPlugin()
        : mName("T3DXRenderer")
    {

    }

    T3DXPlugin::~T3DXPlugin()
    {

    }

    const String &T3DXPlugin::getName() const
    {
        return mName;
    }

    TResult T3DXPlugin::install()
    {
        TResult ret = T3D_OK;
        return ret;
    }

    TResult T3DXPlugin::startup()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    TResult T3DXPlugin::shutdown()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    TResult T3DXPlugin::uninstall()
    {
        TResult ret = T3D_OK;

        return ret;
    }
}
