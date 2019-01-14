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


#include "T3DFSArchivePrerequisites.h"
#include "T3DFSArchivePlugin.h"


Tiny3D::FileSystemArchivePlugin *gPlugin = nullptr;

extern "C"
{
    void T3D_FSARCHIVE_API dllStartPlugin()
    {
        gPlugin = new Tiny3D::FileSystemArchivePlugin();
        Tiny3D::Engine::getInstance().installPlugin(gPlugin);
    }

    void T3D_FSARCHIVE_API dllStopPlugin()
    {
        Tiny3D::Engine::getInstance().uninstallPlugin(gPlugin);
        delete gPlugin;
        gPlugin = nullptr;
    }
}

