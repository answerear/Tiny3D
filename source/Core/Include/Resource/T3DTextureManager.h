/*******************************************************************************
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

#ifndef __T3D_TEXTURE_MANAGER_H__
#define __T3D_TEXTURE_MANAGER_H__


#include "Resource/T3DResourceManager.h"
#include "Kernel/T3DConstant.h"


namespace Tiny3D
{
    class T3D_ENGINE_API TextureManager
        : public Singleton<TextureManager>
        , public ResourceManager
    {
    public:
        static TextureManagerPtr create();

        RenderTexturePtr createRenderTexture(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps = 1, uint32_t MSAACount = 0, uint32_t MSAAQuality = 0);

        Texture1DPtr createTexture1D(const String &name, uint32_t width, PixelFormat format, const Buffer &data, uint32_t mipmaps = 1);

        Texture2DPtr createTexture2D(const String &name, uint32_t width, uint32_t height, PixelFormat format, const Buffer &data, uint32_t mipmaps = 1, uint32_t MSAACount = 0, uint32_t MSAAQuality = 0);

        Texture2DArrayPtr createTexture2DArray(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, const Buffer &data, uint32_t mipmaps = 1, uint32_t MSAACount = 0, uint32_t MSAAQuality = 0);
        
        Texture3DPtr createTexture3D(const String &name, uint32_t width, uint32_t height, uint32_t depth, PixelFormat format, const Buffer &data, uint32_t mipmaps = 1);

        CubemapPtr createCubemap(const String &name, uint32_t width, uint32_t height, PixelFormat format, const Buffer &data, uint32_t mipmaps = 1, uint32_t MSAACount = 1, uint32_t MSAAQuality = 0);

        CubemapArrayPtr createCubemapArray(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, const Buffer &data, uint32_t mipmaps = 1, uint32_t MSAACount = 0, uint32_t MSAAQuality = 0);

        // Texture1DPtr loadTexture1D(Archive *archive, const String &name);
        //
        // Texture2DPtr loadTexture2D(Archive *archive, const String &name);
        //
        // Texture2DArrayPtr loadTexture2DArray(Archive *archive, const String &name);
        //
        // Texture3DPtr loadTexture3D(Archive *archive, const String &name);
        //
        // CubemapPtr loadCubemap(Archive *archive, const String &name);
        //
        // CubemapArrayPtr loadCubemapArray(Archive *archive, const String &name);

        TexturePtr loadTexture(Archive *archive, const String &name);

        TResult saveTexture(Archive *archive, Texture *texture);

    protected:

        ResourcePtr newResource(const String &name, int32_t argc, va_list args) override;

        ResourcePtr loadResource(const String &name, DataStream &stream, int32_t argc, va_list args) override;
        
        TResult saveResource(DataStream &stream, Resource *res) override;
    };

    #define T3D_TEXTURE_MGR    (TextureManager::getInstance()) 
}


#endif    /*__T3D_TEXTURE_MANAGER_H__*/
