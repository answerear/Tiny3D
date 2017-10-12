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

#include "T3DD3D9Plugin.h"
#include "Misc/T3DEntrance.h"
#include "T3DD3d9Renderer.h"


namespace Tiny3D
{
    D3D9Plugin::D3D9Plugin()
        : mName("Direct3D9 Renderer Plugin")
        , mRenderer(nullptr)
        , mTextureMgr(nullptr)
    {

    }

    D3D9Plugin::~D3D9Plugin()
    {

    }

    const String &D3D9Plugin::getName() const
    {
        return mName;
    }

    bool D3D9Plugin::install()
    {
// #if defined (T3D_EXPORT_API)
//         HINSTANCE hInstance = GetModuleHandle("T3DD3D9Renderer.dll");
// #else
//         HINSTANCE hInstance = GetModuleHandle(NULL);
// #endif
        HINSTANCE hInstance;
        static const TCHAR staticVar;
        GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS 
            | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, 
            &staticVar, &hInstance);

        mRenderer = new D3D9Renderer(hInstance);

        Entrance::getInstance().addRenderer(mRenderer);

//         mTextureMgr = new D3D9TextureManager();

        return true;
    }

    bool D3D9Plugin::startup()
    {
        return true;
    }

    void D3D9Plugin::shutdown()
    {
        
    }

    void D3D9Plugin::uninstall()
    {
        Entrance::getInstance().removeRenderer(mRenderer);

        T3D_SAFE_RELEASE(mRenderer);
    }
}
