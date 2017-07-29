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

#include "Resource/T3DTexture.h"
#include "Misc/T3DImage.h"


namespace Tiny3D
{
    Texture::Texture(const String &name, int32_t numMipMaps, 
        int32_t texWidth /* = -1 */, int32_t texHeight /* = -1 */, 
        TexUsage texUsage /* = E_TU_DEFAULT */, TexType texType /* = E_TEX_TYPE_1D */, 
        PixelFormat format /* = E_PF_A8R8G8B8 */)
        : Resource(name)
        , mTexType(texType)
        , mTexUsage(texUsage)
        , mNumMipMaps(numMipMaps)
        , mTexWidth(texWidth)
        , mTexHeight(texHeight)
        , mImgWidth(texWidth)
        , mImgHeight(texHeight)
        , mFormat(format)
        , mHasAlpha(false)
    {

    }

    Texture::~Texture()
    {

    }

    Resource::Type Texture::getType() const
    {
        return E_TYPE_TEXTURE;
    }
}
