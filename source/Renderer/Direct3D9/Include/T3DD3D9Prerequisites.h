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

#ifndef __T3D_D3D9PREREQUISITES_H__
#define __T3D_D3D9PREREQUISITES_H__


#include <T3DPlatform.h>
#include <T3DLog.h>
#include <Tiny3D.h>
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <DxErr.h>


#if defined T3DD3D9RENDERER_EXPORT
    #define T3D_D3D9RENDERER_API        T3D_EXPORT_API
#else
    #define T3D_D3D9RENDERER_API        T3D_IMPORT_API
#endif


#define D3D_SAFE_RELEASE(p) \
    if ((p) != nullptr)   \
    {   \
        (p)->Release();   \
        (p) = nullptr;    \
    }


namespace Tiny3D
{
    class D3D9Renderer;
    class D3D9RenderWindow;
    class D3D9HardwareVertexBuffer;
    class D3D9HardwareIndexBuffer;
    class D3D9HardwarePixelBuffer;
    class D3D9HardwareBufferManager;
    class D3D9Material;
    class D3D9VertexDeclaration;
}


#endif  /*__T3D_D3D9PREREQUISITES_H__*/

