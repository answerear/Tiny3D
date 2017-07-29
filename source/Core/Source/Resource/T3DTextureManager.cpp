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

#include "Resource/T3DTextureManager.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(TextureManager);

    TextureManager::TextureManager()
        : ResourceManager()
        , mDefaultNumMipMaps(1)
    {

    }

    TextureManager::~TextureManager()
    {

    }

    TexturePtr TextureManager::loadTexture(const String &name, int32_t numMipMaps /* = -1 */, 
        Texture::TexUsage texUsage /* = Texture::E_TU_DEFAULT */, Texture::TexType texType /* = Texture::E_TEX_TYPE_2D */)
    {
        return smart_pointer_cast<Texture>(ResourceManager::load(name, 6, -1, -1, numMipMaps, E_PF_A8R8G8B8, texUsage, texType));
    }

    TexturePtr TextureManager::loadTexture(const String &name, int32_t width, int32_t height, 
        int32_t numMipMaps /* = -1 */, PixelFormat format /* = E_PF_A8R8G8B8 */, 
        Texture::TexUsage texUsage /* = Texture::E_TU_BLANK */, Texture::TexType texType /* = Texture::E_TEX_TYPE_2D */)
    {
        return smart_pointer_cast<Texture>(ResourceManager::load(name, 6, width, height, numMipMaps, format, texUsage, texType));
    }

    ResourcePtr TextureManager::create(const String &name, int32_t argc, va_list args)
    {
        ResourcePtr res;

        if (argc == 6)
        {
            int32_t width = va_arg(args, int32_t);
            int32_t height = va_arg(args, int32_t);
            int32_t numMipMaps = va_arg(args, int32_t);
            PixelFormat format = va_arg(args, PixelFormat);
            Texture::TexUsage texUsage = va_arg(args, Texture::TexUsage);
            Texture::TexType texType = va_arg(args, Texture::TexType);
            numMipMaps = (numMipMaps == -1 ? mDefaultNumMipMaps : numMipMaps);
            res = createTexture(name, width, height, numMipMaps, format, texUsage, texType);
        }

        return res;
    }
}
