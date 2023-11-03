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


#include "Resource/T3DTexture.h"
#include "Render/T3DRenderResourceManager.h" 


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Texture::Texture(const String &name)
        : Resource(name)
    {
        
    }

    //--------------------------------------------------------------------------

    Resource::Type Texture::getType() const
    {
        return Type::kTexture;
    }

    //--------------------------------------------------------------------------
    
    void Texture::cloneProperties(const Resource *const src)
    {
        Resource::cloneProperties(src);
        const Texture *texture = static_cast<const Texture*>(src);
    }
    
    //--------------------------------------------------------------------------

    Texture1DPtr Texture1D::create(const String &name, uint32_t width, PixelFormat format, uint32_t mipmaps, const Buffer &data)
    {
        return new Texture1D(name, width, format, mipmaps, data);
    }

    //--------------------------------------------------------------------------

    Texture1D::Texture1D(const String &name, uint32_t width, PixelFormat format, uint32_t mipmaps, const Buffer &data)
        : Texture(name)
    {
        mDesc.width = width;
        mDesc.format = format;
        mDesc.mipmaps = mipmaps;
        mDesc.arraySize = 1;
        mDesc.buffer = data;
    }

    //--------------------------------------------------------------------------

    TEXTURE_TYPE Texture1D::getTextureType() const
    {
        return TEXTURE_TYPE::TT_1D;
    }

    //--------------------------------------------------------------------------

    TResult Texture1D::onCreate()
    {
        mPixelBuffer = T3D_RENDER_BUFFER_MGR.loadPixelBuffer1D(&mDesc, MemoryType::kBoth, Usage::kImmutable, CPUAccessMode::kCPUNone);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Texture1D::onLoad()
    {
        mPixelBuffer = T3D_RENDER_BUFFER_MGR.loadPixelBuffer1D(&mDesc, MemoryType::kBoth, Usage::kImmutable, CPUAccessMode::kCPUNone);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    ResourcePtr Texture1D::clone() const
    {
        Texture1DPtr texture = create(getName(), mDesc.width, mDesc.format, mDesc.mipmaps, mDesc.buffer);
        texture->cloneProperties(this);
        return texture;
    }
    
    //--------------------------------------------------------------------------
    
    Texture2DPtr Texture2D::create(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data)
    {
        return new Texture2D(name, width, height, format, mipmaps, MSAACount, MSAAQuality, data);
    }

    //--------------------------------------------------------------------------

    Texture2D::Texture2D(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data)
        : Texture(name)
    {
        mDesc.width = width;
        mDesc.height = height;
        mDesc.format = format;
        mDesc.mipmaps = mipmaps;
        mDesc.arraySize = 1;
        mDesc.sampleDesc.Count = MSAACount;
        mDesc.sampleDesc.Quality = MSAAQuality;
        mDesc.buffer = data;
    }

    //--------------------------------------------------------------------------

    TEXTURE_TYPE Texture2D::getTextureType() const
    {
        return TEXTURE_TYPE::TT_2D;
    }

    //--------------------------------------------------------------------------

    TResult Texture2D::onCreate()
    {
        mPixelBuffer = T3D_RENDER_BUFFER_MGR.loadPixelBuffer2D(&mDesc, MemoryType::kBoth, Usage::kImmutable, CPUAccessMode::kCPUNone);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Texture2D::onLoad()
    {
        mPixelBuffer = T3D_RENDER_BUFFER_MGR.loadPixelBuffer2D(&mDesc, MemoryType::kBoth, Usage::kImmutable, CPUAccessMode::kCPUNone);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    ResourcePtr Texture2D::clone() const
    {
        Texture2DPtr texture = create(getName(), mDesc.width, mDesc.height, mDesc.format, mDesc.mipmaps, mDesc.sampleDesc.Count, mDesc.sampleDesc.Quality, mDesc.buffer);
        texture->cloneProperties(this);
        return texture;
    }
    
    //--------------------------------------------------------------------------

    Texture2DArrayPtr Texture2DArray::create(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data)
    {
        return new Texture2DArray(name, width, height, format, arraySize, mipmaps, MSAACount, MSAAQuality, data);
    }

    //--------------------------------------------------------------------------

    Texture2DArray::Texture2DArray(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data)
        : Texture(name)
    {
        mDesc.width = width;
        mDesc.height = height;
        mDesc.format = format;
        mDesc.arraySize = arraySize;
        mDesc.sampleDesc.Count = MSAACount;
        mDesc.sampleDesc.Quality = MSAAQuality;
        mDesc.buffer = data;
    }

    //--------------------------------------------------------------------------

    TEXTURE_TYPE Texture2DArray::getTextureType() const
    {
        return TEXTURE_TYPE::TT_2D_ARRAY;
    }

    //--------------------------------------------------------------------------

    TResult Texture2DArray::onCreate()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Texture2DArray::onLoad()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    ResourcePtr Texture2DArray::clone() const
    {
        Texture2DArrayPtr texture = create(getName(), mDesc.width, mDesc.height, mDesc.format, mDesc.arraySize, mDesc.mipmaps, mDesc.sampleDesc.Count, mDesc.sampleDesc.Quality, mDesc.buffer);
        texture->cloneProperties(this);
        return texture;
    }
    
    //--------------------------------------------------------------------------

    Texture3DPtr Texture3D::create(const String &name, uint32_t width, uint32_t height, uint32_t depth, PixelFormat format, uint32_t mipmaps, const Buffer &data)
    {
        return new Texture3D(name, width, height, depth, format, mipmaps, data);
    }

    //--------------------------------------------------------------------------

    Texture3D::Texture3D(const String &name, uint32_t width, uint32_t height, uint32_t depth, PixelFormat format, uint32_t mipmaps, const Buffer &data)
        : Texture(name)
    {
        mDesc.width = width;
        mDesc.height = height;
        mDesc.depth = depth;
        mDesc.format = format;
        mDesc.mipmaps = mipmaps;
        mDesc.buffer = data;
    }

    //--------------------------------------------------------------------------

    TEXTURE_TYPE Texture3D::getTextureType() const
    {
        return TEXTURE_TYPE::TT_3D;
    }

    //--------------------------------------------------------------------------

    TResult Texture3D::onCreate()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Texture3D::onLoad()
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    ResourcePtr Texture3D::clone() const
    {
        Texture3DPtr texture = create(getName(), mDesc.width, mDesc.height, mDesc.depth, mDesc.format, mDesc.mipmaps, mDesc.buffer);
        texture->cloneProperties(this);
        return texture;
    }
    
    //--------------------------------------------------------------------------

    CubemapPtr Cubemap::create(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data)
    {
        return new Cubemap(name, width, height, format, mipmaps, MSAACount, MSAAQuality, data);
    }

    //--------------------------------------------------------------------------

    Cubemap::Cubemap(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data)
        : Texture(name)
    {
        mDesc.width = width;
        mDesc.height = height;
        mDesc.format = format;
        mDesc.mipmaps = mipmaps;
        mDesc.sampleDesc.Count = MSAACount;
        mDesc.sampleDesc.Quality = MSAAQuality;
        mDesc.arraySize = 6;
        mDesc.buffer = data;
    }

    //--------------------------------------------------------------------------

    TEXTURE_TYPE Cubemap::getTextureType() const
    {
        return TEXTURE_TYPE::TT_CUBE;
    }

    //--------------------------------------------------------------------------

    TResult Cubemap::onCreate()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Cubemap::onLoad()
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    ResourcePtr Cubemap::clone() const
    {
        CubemapPtr texture = create(getName(), mDesc.width, mDesc.height, mDesc.format, mDesc.mipmaps, mDesc.sampleDesc.Count, mDesc.sampleDesc.Quality, mDesc.buffer);
        texture->cloneProperties(this);
        return texture;
    }
    
    //--------------------------------------------------------------------------

    CubemapArrayPtr CubemapArray::create(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data)
    {
        return new CubemapArray(name, width, height, format, arraySize, mipmaps, MSAACount, MSAAQuality, data);
    }

    //--------------------------------------------------------------------------

    CubemapArray::CubemapArray(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data)
        : Texture(name)
    {
        mDesc.width = width;
        mDesc.height = height;
        mDesc.format = format;
        mDesc.mipmaps = mipmaps;
        mDesc.arraySize = arraySize;
        mDesc.sampleDesc.Count = MSAACount;
        mDesc.sampleDesc.Quality = MSAAQuality;
        mDesc.buffer = data;
    }

    //--------------------------------------------------------------------------

    TEXTURE_TYPE CubemapArray::getTextureType() const
    {
        return TEXTURE_TYPE::TT_CUBE_ARRAY;
    }

    //--------------------------------------------------------------------------
    
    TResult CubemapArray::onCreate()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult CubemapArray::onLoad()
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    ResourcePtr CubemapArray::clone() const
    {
        CubemapArrayPtr texture = create(getName(), mDesc.width, mDesc.height, mDesc.format, mDesc.arraySize, mDesc.mipmaps, mDesc.sampleDesc.Count, mDesc.sampleDesc.Quality, mDesc.buffer);
        texture->cloneProperties(this);
        return texture;
    }

    //--------------------------------------------------------------------------
}
