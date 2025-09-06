/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#include "Resource/T3DTexture.h"
#include "Render/T3DRenderResourceManager.h" 
#include "Render/T3DPixelBuffer.h"
#include "Resource/T3DImage.h"

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

    void Texture::setSamplerDesc(const SamplerDesc &desc)
    {
        uint32_t hashSrc = 0;
        if (mSamplerState != nullptr)
        {
            CRC::crc32((uint8_t*)&mSamplerState->getStateDesc(), sizeof(SamplerDesc));
        }
        
        uint32_t hashDst = CRC::crc32((uint8_t*)&desc, sizeof(SamplerDesc));

        if (hashSrc != hashDst)
        {
            // 新生成一个 sampler state 对象
            mSamplerState = T3D_RENDER_STATE_MGR.loadSamplerState(desc, hashDst);
        }
    }

    //--------------------------------------------------------------------------

    Texture1DPtr Texture1D::create(const String &name, uint32_t width, PixelFormat format, uint32_t mipmaps, const Buffer &data)
    {
        return T3D_NEW Texture1D(name, width, format, mipmaps, data);
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

    PixelBuffer *Texture1D::getPixelBuffer() const
    {
        return mPixelBuffer;
    }

    //--------------------------------------------------------------------------

    TResult Texture1D::onCreate()
    {
        TResult ret;

        do
        {
            ret = Texture::onCreate();
            if (T3D_FAILED(ret))
            {
                break;
            }

            mPixelBuffer = T3D_RENDER_BUFFER_MGR.loadPixelBuffer1D(&mDesc, MemoryType::kVRAM, Usage::kImmutable, CPUAccessMode::kCPUNone);
        } while (false);
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Texture1D::onLoad(Archive *archive)
    {
        TResult ret;

        do
        {
            ret = Texture::onLoad(archive);
            if (T3D_FAILED(ret))
            {
                break;
            }
            mPixelBuffer = T3D_RENDER_BUFFER_MGR.loadPixelBuffer1D(&mDesc, MemoryType::kVRAM, Usage::kImmutable, CPUAccessMode::kCPUNone);
        } while (false);
        
        return ret;
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
        return T3D_NEW Texture2D(name, width, height, format, mipmaps, MSAACount, MSAAQuality, data);
    }

    //--------------------------------------------------------------------------

    Texture2D::Texture2D(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data, bool shaderReadable)
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
        mDesc.shaderReadable = shaderReadable;
    }

    //--------------------------------------------------------------------------

    Texture2DPtr Texture2D::create(const String &name, Image *image, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality)
    {
        return T3D_NEW Texture2D(name, image, mipmaps, MSAACount, MSAAQuality);
    }

    //--------------------------------------------------------------------------

    Texture2D::Texture2D(const String &name, Image *image, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, bool shaderReadable)
        : Texture(name)
    {
        T3D_ASSERT(image != nullptr);
        
        mDesc.width = image->getWidth();
        mDesc.height = image->getHeight();
        mDesc.format = image->getFormat();
        mDesc.mipmaps = mipmaps;
        mDesc.arraySize = 1;
        mDesc.sampleDesc.Count = MSAACount;
        mDesc.sampleDesc.Quality = MSAAQuality;
        mDesc.buffer.setData(image->getData(), image->getSize());
        mDesc.shaderReadable = shaderReadable;
        mImage = image;
    }

    //--------------------------------------------------------------------------

    TEXTURE_TYPE Texture2D::getTextureType() const
    {
        return TEXTURE_TYPE::TT_2D;
    }

    //--------------------------------------------------------------------------

    PixelBuffer *Texture2D::getPixelBuffer() const
    {
        return mPixelBuffer;
    }

    //--------------------------------------------------------------------------

    TResult Texture2D::onCreate()
    {
        TResult ret;

        do
        {
            ret = Texture::onCreate();
            if (T3D_FAILED(ret))
            {
                break;
            }

            mPixelBuffer = T3D_RENDER_BUFFER_MGR.loadPixelBuffer2D(&mDesc, MemoryType::kVRAM, Usage::kImmutable, CPUAccessMode::kCPUNone);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Texture2D::onLoad(Archive *archive)
    {
        TResult ret;

        do
        {
            ret = Texture::onLoad(archive);
            if (T3D_FAILED(ret))
            {
                break;
            }
            mPixelBuffer = T3D_RENDER_BUFFER_MGR.loadPixelBuffer2D(&mDesc, MemoryType::kVRAM, Usage::kImmutable, CPUAccessMode::kCPUNone);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Texture2D::onUnload()
    {
        TResult ret;

        do
        {
            ret = Texture::onUnload();
            if (T3D_FAILED(ret))
            {
                break;
            }

            mImage = nullptr;
        } while (false);

        return ret;
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
        return T3D_NEW Texture2DArray(name, width, height, format, arraySize, mipmaps, MSAACount, MSAAQuality, data);
    }

    //--------------------------------------------------------------------------

    Texture2DArray::Texture2DArray(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data)
        : Texture2D(name, width, height, format, mipmaps, MSAACount, MSAAQuality, data)
    {
        mDesc.arraySize = arraySize;
    }

    //--------------------------------------------------------------------------

    TEXTURE_TYPE Texture2DArray::getTextureType() const
    {
        return TEXTURE_TYPE::TT_2D_ARRAY;
    }

    //--------------------------------------------------------------------------

    TResult Texture2DArray::onCreate()
    {
        TResult ret;

        do
        {
            ret = Texture2D::onCreate();
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Texture2DArray::onLoad(Archive *archive)
    {
        TResult ret;

        do
        {
            ret = Texture2D::onLoad(archive);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);
        
        return ret;
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
        return T3D_NEW Texture3D(name, width, height, depth, format, mipmaps, data);
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

    PixelBuffer *Texture3D::getPixelBuffer() const
    {
        return mPixelBuffer;
    }

    //--------------------------------------------------------------------------

    TResult Texture3D::onCreate()
    {
        TResult ret;

        do
        {
            ret = Texture::onCreate();
            if (T3D_FAILED(ret))
            {
                break;
            }
            
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Texture3D::onLoad(Archive *archive)
    {
        TResult ret;

        do
        {
            ret = Texture::onLoad(archive);
            if (T3D_FAILED(ret))
            {
                break;
            }
            
        } while (false);
        return ret;
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
        return T3D_NEW Cubemap(name, width, height, format, mipmaps, MSAACount, MSAAQuality, data);
    }

    //--------------------------------------------------------------------------

    Cubemap::Cubemap(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data)
        : Texture2D(name, width, height, format, mipmaps, MSAACount, MSAAQuality, data)
    {
        mDesc.arraySize = 6;
    }

    //--------------------------------------------------------------------------

    TEXTURE_TYPE Cubemap::getTextureType() const
    {
        return TEXTURE_TYPE::TT_CUBE;
    }

    //--------------------------------------------------------------------------

    TResult Cubemap::onCreate()
    {
        TResult ret;

        do
        {
            ret = Texture2D::onCreate();
            if (T3D_FAILED(ret))
            {
                break;
            }
            
        } while (false);
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Cubemap::onLoad(Archive *archive)
    {
        TResult ret;

        do
        {
            ret = Texture2D::onLoad(archive);
            if (T3D_FAILED(ret))
            {
                break;
            }
            
        } while (false);
        return ret;
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
        return T3D_NEW CubemapArray(name, width, height, format, arraySize, mipmaps, MSAACount, MSAAQuality, data);
    }

    //--------------------------------------------------------------------------

    CubemapArray::CubemapArray(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data)
        : Cubemap(name, width, height, format, mipmaps, MSAACount, MSAAQuality, data)
    {
        mDesc.arraySize = arraySize;
    }

    //--------------------------------------------------------------------------

    TEXTURE_TYPE CubemapArray::getTextureType() const
    {
        return TEXTURE_TYPE::TT_CUBE_ARRAY;
    }

    //--------------------------------------------------------------------------
    
    TResult CubemapArray::onCreate()
    {
        TResult ret;

        do
        {
            ret = Cubemap::onCreate();
            if (T3D_FAILED(ret))
            {
                break;
            }
            
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult CubemapArray::onLoad(Archive *archive)
    {
        TResult ret;

        do
        {
            ret = Cubemap::onLoad(archive);
            if (T3D_FAILED(ret))
            {
                break;
            }
            
        } while (false);
        
        return ret;
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
