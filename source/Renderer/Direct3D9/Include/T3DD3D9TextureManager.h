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

#ifndef __T3D_D3D9_TEXTURE_MANAGER_H__
#define __T3D_D3D9_TEXTURE_MANAGER_H__


#include "T3DD3D9Prerequisites.h"


namespace Tiny3D
{
#if 0
    class T3D_D3D9RENDERER_API D3D9TextureManager : public TextureManager
    {
    public:
        D3D9TextureManager();
        virtual ~D3D9TextureManager();

    protected:
        virtual TexturePtr createTexture(const String &name, int32_t width, int32_t height, int32_t numMipMaps, PixelFormat format, Texture::TexUsage texUsage, Texture::TexType texType);
    };
#endif
}


#endif  /*__T3D_D3D9_TEXTURE_MANAGER_H__*/
