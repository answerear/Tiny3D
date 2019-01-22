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


#include "T3DFreeImageCodecPrerequisites.h"
#include "T3DFreeImageCodecPlugin.h"


Tiny3D::FreeImageCodecPlugin *gPlugin = nullptr;

extern "C"
{
    TResult T3D_FREEIMAGECODEC_API dllStartPlugin()
    {
        gPlugin = new Tiny3D::FreeImageCodecPlugin();
        return Tiny3D::Engine::getInstance().installPlugin(gPlugin);
    }

    TResult T3D_FREEIMAGECODEC_API dllStopPlugin()
    {
        TResult ret = Tiny3D::Engine::getInstance().uninstallPlugin(gPlugin);
        if (ret == Tiny3D::T3D_OK)
        {
            delete gPlugin;
            gPlugin = nullptr;
        }
        return ret;
    }
}

