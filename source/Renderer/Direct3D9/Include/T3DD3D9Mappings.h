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

#ifndef __T3D_D3D9_MAPPING_H__
#define __T3D_D3D9_MAPPING_H__


#include "T3DD3D9Prerequisites.h"
#include "Render/T3DHardwareVertexBuffer.h"
#include "Render/T3DHardwareIndexBuffer.h"


namespace Tiny3D
{
    class D3D9Mappings
    {
    public:
        static D3DPRIMITIVETYPE get(Renderer::PrimitiveType priType);
        static DWORD get(HardwareBuffer::Usage usage);
        static DWORD get(HardwareBuffer::Usage usage, HardwareBuffer::LockOptions options);
        static D3DFORMAT get(HardwareIndexBuffer::Type type);
        static D3DDECLTYPE get(VertexElement::Type type);
        static D3DDECLUSAGE get(VertexElement::Semantic semantic);
        static D3DFORMAT get(PixelFormat format);
        static PixelFormat get(D3DFORMAT d3dfmt);
        static D3DCOLORVALUE get(const Color4 &color);
        static D3DLIGHTTYPE get(SGLight::LightType type);
    };
}


#endif  /*__T3D_D3D_MAPPING_H__*/
