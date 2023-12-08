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


#include "Resource/T3DTextureManager.h"
#include "Resource/T3DTexture.h"
#include "Render/T3DPixelBuffer.h"
#include "Render/T3DRenderTexture.h"
#include "Serializer/T3DSerializerManager.h"


namespace Tiny3D
{   
    //--------------------------------------------------------------------------

    TextureManagerPtr TextureManager::create()
    {
        return new TextureManager();
    }
    
    //--------------------------------------------------------------------------

    RenderTexturePtr TextureManager::createRenderTexture(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality)
    {
        return smart_pointer_cast<RenderTexture>(createResource(name, 7,
            TEXTURE_TYPE::TT_RENDER_TEXTURE, width, height, format, mipmaps, MSAACount, MSAAQuality));
    }

    //--------------------------------------------------------------------------

    Texture1DPtr TextureManager::createTexture1D(const String &name, uint32_t width,
        PixelFormat format, const Buffer &data, uint32_t mipmaps/* = 0*/)
    {
        return smart_pointer_cast<Texture1D>(createResource(name, 6,
            TEXTURE_TYPE::TT_1D, width, format,
            data.Data, data.DataSize, mipmaps));
    }

    //--------------------------------------------------------------------------
    
    Texture2DPtr TextureManager::createTexture2D(const String &name, uint32_t width, uint32_t height,
        PixelFormat format, const Buffer &data, uint32_t mipmaps/* = 0*/, uint32_t MSAACount/* = 0*/, uint32_t MSAAQuality/* = 0*/)
    {
        return smart_pointer_cast<Texture2D>(createResource(name, 9,
            TEXTURE_TYPE::TT_2D, width, height, format,
            data.Data, data.DataSize, mipmaps, MSAACount, MSAAQuality));
    }

    //--------------------------------------------------------------------------
    
    Texture2DArrayPtr TextureManager::createTexture2DArray(const String &name, uint32_t width, uint32_t height,
        PixelFormat format, uint32_t arraySize, const Buffer &data, uint32_t mipmaps/* = 0*/, uint32_t MSAACount/* = 0*/, uint32_t MSAAQuality/* = 0*/)
    {
        return smart_pointer_cast<Texture2DArray>(createResource(name, 10,
            TEXTURE_TYPE::TT_2D_ARRAY, width, height, format, arraySize,
            data.Data, data.DataSize, mipmaps, MSAACount, MSAAQuality));
    }

    //--------------------------------------------------------------------------
    
    Texture3DPtr TextureManager::createTexture3D(const String &name, uint32_t width, uint32_t height, uint32_t depth,
        PixelFormat format, const Buffer &data, uint32_t mipmaps/* = 0*/)
    {
        return smart_pointer_cast<Texture3D>(createResource(name, 8,
            TEXTURE_TYPE::TT_3D, width, height, depth, format,
            data.Data, data.DataSize, mipmaps));
    }

    //--------------------------------------------------------------------------
    
    CubemapPtr TextureManager::createCubemap(const String &name, uint32_t width, uint32_t height,
        PixelFormat format, const Buffer &data, uint32_t mipmaps/* = 0*/, uint32_t MSAACount/* = 0*/, uint32_t MSAAQuality/* = 0*/)
    {
        return smart_pointer_cast<Cubemap>(createResource(name, 9,
            TEXTURE_TYPE::TT_CUBE, width, height, format,
            data.Data, data.DataSize, mipmaps, MSAACount, MSAAQuality));
    }

    //--------------------------------------------------------------------------
    
    CubemapArrayPtr TextureManager::createCubemapArray(const String &name, uint32_t width, uint32_t height,
        PixelFormat format, uint32_t arraySize, const Buffer &data, uint32_t mipmaps/* = 0*/, uint32_t MSAACount/* = 0*/, uint32_t MSAAQuality/* = 0*/)
    {
        return smart_pointer_cast<CubemapArray>(createResource(name, 10,
            TEXTURE_TYPE::TT_CUBE_ARRAY, width, height, format, arraySize,
            data.Data, data.DataSize, mipmaps, MSAACount, MSAAQuality));
    }

    //--------------------------------------------------------------------------

    ResourcePtr TextureManager::newResource(const String &name, int32_t argc, va_list args)
    {
        TEXTURE_TYPE texType = va_arg(args, TEXTURE_TYPE);

        TexturePtr texture;
        
        switch (texType)
        {
        case TEXTURE_TYPE::TT_1D:
            {
                uint32_t width = va_arg(args, uint32_t);
                PixelFormat format = va_arg(args, PixelFormat);
                Buffer data;
                data.Data = va_arg(args, uint8_t*);
                data.DataSize = va_arg(args, size_t);
                uint32_t mipmaps = va_arg(args, uint32_t);
                texture = Texture1D::create(name, width, format, mipmaps, data);
            }
            break;
        case TEXTURE_TYPE::TT_2D:
            {
                uint32_t width = va_arg(args, uint32_t);
                uint32_t height = va_arg(args, uint32_t);
                PixelFormat format = va_arg(args, PixelFormat);
                Buffer data;
                data.Data = va_arg(args, uint8_t*);
                data.DataSize = va_arg(args, size_t);
                uint32_t mipmaps = va_arg(args, uint32_t);
                uint32_t MSAACount = va_arg(args, uint32_t);
                uint32_t MSAAQuality = va_arg(args, uint32_t);
                texture = Texture2D::create(name, width, height, format, mipmaps, MSAACount, MSAAQuality, data);
            }
            break;
        case TEXTURE_TYPE::TT_2D_ARRAY:
            {
                uint32_t width = va_arg(args, uint32_t);
                uint32_t height = va_arg(args, uint32_t);
                PixelFormat format = va_arg(args, PixelFormat);
                uint32_t arraySize = va_arg(args, uint32_t);
                Buffer data;
                data.Data = va_arg(args, uint8_t*);
                data.DataSize = va_arg(args, size_t);
                uint32_t mipmaps = va_arg(args, uint32_t);
                uint32_t MSAACount = va_arg(args, uint32_t);
                uint32_t MSAAQuality = va_arg(args, uint32_t);
                texture = Texture2DArray::create(name, width, height, format, arraySize, mipmaps, MSAACount, MSAAQuality, data);
            }
            break;
        case TEXTURE_TYPE::TT_3D:
            {
                uint32_t width = va_arg(args, uint32_t);
                uint32_t height = va_arg(args, uint32_t);
                uint32_t depth = va_arg(args, uint32_t);
                PixelFormat format = va_arg(args, PixelFormat);
                Buffer data;
                data.Data = va_arg(args, uint8_t*);
                data.DataSize = va_arg(args, size_t);
                uint32_t mipmaps = va_arg(args, uint32_t);
                texture = Texture3D::create(name, width, height, depth, format, mipmaps, data);
            }
            break;
        case TEXTURE_TYPE::TT_CUBE:
            {
                uint32_t width = va_arg(args, uint32_t);
                uint32_t height = va_arg(args, uint32_t);
                PixelFormat format = va_arg(args, PixelFormat);
                Buffer data;
                data.Data = va_arg(args, uint8_t*);
                data.DataSize = va_arg(args, size_t);
                uint32_t mipmaps = va_arg(args, uint32_t);
                uint32_t MSAACount = va_arg(args, uint32_t);
                uint32_t MSAAQuality = va_arg(args, uint32_t);
                texture = Cubemap::create(name, width, height, format, mipmaps, MSAACount, MSAAQuality, data);
            }
            break;
        case TEXTURE_TYPE::TT_CUBE_ARRAY:
            {
                uint32_t width = va_arg(args, uint32_t);
                uint32_t height = va_arg(args, uint32_t);
                PixelFormat format = va_arg(args, PixelFormat);
                uint32_t arraySize = va_arg(args, uint32_t);
                Buffer data;
                data.Data = va_arg(args, uint8_t*);
                data.DataSize = va_arg(args, size_t);
                uint32_t mipmaps = va_arg(args, uint32_t);
                uint32_t MSAACount = va_arg(args, uint32_t);
                uint32_t MSAAQuality = va_arg(args, uint32_t);
                texture = CubemapArray::create(name, width, height, format, arraySize, mipmaps, MSAACount, MSAAQuality, data);
            }
            break;
        case TEXTURE_TYPE::TT_RENDER_TEXTURE:
            {
                uint32_t width = va_arg(args, uint32_t);
                uint32_t height = va_arg(args, uint32_t);
                PixelFormat format = va_arg(args, PixelFormat);
                uint32_t mipmaps = va_arg(args, uint32_t);
                uint32_t MSAACount = va_arg(args, uint32_t);
                uint32_t MSAAQuality = va_arg(args, uint32_t);
                texture = RenderTexture::create(name, width, height, format, mipmaps, MSAACount, MSAAQuality);
            }
            break;
        }
        
        return texture;
    }

    //--------------------------------------------------------------------------

    Texture1DPtr TextureManager::loadTexture1D(Archive *archive, const String &name)
    {
        return smart_pointer_cast<Texture1D>(load(archive, name, 1, TEXTURE_TYPE::TT_1D));
    }

    //--------------------------------------------------------------------------

    Texture2DPtr TextureManager::loadTexture2D(Archive *archive, const String &name)
    {
        return smart_pointer_cast<Texture2D>(load(archive, name, 1, TEXTURE_TYPE::TT_2D));
    }

    //--------------------------------------------------------------------------

    Texture2DArrayPtr TextureManager::loadTexture2DArray(Archive *archive, const String &name)
    {
        return smart_pointer_cast<Texture2DArray>(load(archive, name, 1, TEXTURE_TYPE::TT_2D_ARRAY));
    }

    //--------------------------------------------------------------------------

    Texture3DPtr TextureManager::loadTexture3D(Archive *archive, const String &name)
    {
        return smart_pointer_cast<Texture3D>(load(archive, name, 1, TEXTURE_TYPE::TT_3D));
    }

    //--------------------------------------------------------------------------

    CubemapPtr TextureManager::loadCubemap(Archive *archive, const String &name)
    {
        return smart_pointer_cast<Cubemap>(load(archive, name, 1, TEXTURE_TYPE::TT_CUBE));
    }

    //--------------------------------------------------------------------------

    CubemapArrayPtr TextureManager::loadCubemapArray(Archive *archive, const String &name)
    {
        return smart_pointer_cast<CubemapArray>(load(archive, name, 1, TEXTURE_TYPE::TT_CUBE_ARRAY));
    }

    //--------------------------------------------------------------------------

    ResourcePtr TextureManager::loadResource(const String &name, DataStream &stream, int32_t argc, va_list args)
    {
        TEXTURE_TYPE texType = va_arg(args, TEXTURE_TYPE);

        TexturePtr texture;

        switch (texType)
        {
        case TEXTURE_TYPE::TT_1D:
            {
                texture = T3D_SERIALIZER_MGR.deserialize<Texture1D>(stream);
            }
            break;
        case TEXTURE_TYPE::TT_2D:
            {
                texture = T3D_SERIALIZER_MGR.deserialize<Texture2D>(stream);
            }
            break;
        case TEXTURE_TYPE::TT_2D_ARRAY:
            {
                texture = T3D_SERIALIZER_MGR.deserialize<Texture2DArray>(stream);
            }
            break;
        case TEXTURE_TYPE::TT_3D:
            {
                texture = T3D_SERIALIZER_MGR.deserialize<Texture3D>(stream);
            }
            break;
        case TEXTURE_TYPE::TT_CUBE:
            {
                texture = T3D_SERIALIZER_MGR.deserialize<Cubemap>(stream);
            }
            break;
        case TEXTURE_TYPE::TT_CUBE_ARRAY:
            {
                texture = T3D_SERIALIZER_MGR.deserialize<CubemapArray>(stream);
            }
            break;
        case TEXTURE_TYPE::TT_RENDER_TEXTURE:
            {
                
            }
            break;
        }
        
        return texture;
    }

    //--------------------------------------------------------------------------

    TResult TextureManager::saveTexture(Archive *archive, Texture *texture)
    {
        return save(texture, archive);
    }
    
    //--------------------------------------------------------------------------

    TResult TextureManager::saveResource(DataStream &stream, Resource *res)
    {
        Texture *texture = static_cast<Texture*>(res);
        T3D_SERIALIZER_MGR.serialize(stream, texture);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}

