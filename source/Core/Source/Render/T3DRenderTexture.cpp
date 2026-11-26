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


#include "Render/T3DRenderTexture.h"
#include "Render/T3DPixelBuffer.h"
#include "Render/T3DRenderResourceManager.h"
#include "Kernel/T3DAgent.h"
#include "RHI/T3DRHIContext.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    RenderTexturePtr RenderTexture::create(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, bool shaderReadable, uint32_t accMode, uint32_t gpuAccess)
    {
        return T3D_NEW RenderTexture(name, width, height, format, mipmaps, MSAACount, MSAAQuality, shaderReadable, accMode, gpuAccess);
    }
    
    //--------------------------------------------------------------------------
    
    RenderTexture::RenderTexture(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, bool shaderReadable, uint32_t accMode, uint32_t gpuAccess)
        : Texture2D(name, width, height, format, mipmaps, MSAACount, MSAAQuality, Buffer{}, shaderReadable, accMode)
    {
        mDesc.gpuAccess = gpuAccess;
    }

    //--------------------------------------------------------------------------

    RenderTexture::RenderTexture(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data, bool shaderReadable)
        : Texture2D(name, width, height, format, mipmaps, MSAACount, MSAAQuality, data, shaderReadable)
    {
        
    }
    
    //--------------------------------------------------------------------------
    
    RenderTexturePtr RenderTexture::create(const String &name, TEXTURE_TYPE dimension, uint32_t width, uint32_t height, uint32_t arraySize, PixelFormat format, uint32_t mipmaps, bool shaderReadable, uint32_t accMode, uint32_t gpuAccess)
    {
        uint32_t layers = 1;
        bool cubemap = false;
        switch (dimension)
        {
        case TEXTURE_TYPE::TT_2D:
            break;
        case TEXTURE_TYPE::TT_2D_ARRAY:
            layers = arraySize;
            break;
        case TEXTURE_TYPE::TT_CUBE:
            layers = PixelBufferCubemap::FACE_COUNT;
            cubemap = true;
            break;
        case TEXTURE_TYPE::TT_CUBE_ARRAY:
            layers = arraySize * PixelBufferCubemap::FACE_COUNT;
            cubemap = true;
            break;
        default:
            T3D_LOG_ERROR(LOG_TAG_ENGINE, "RenderTexture::create : unsupported dimension %u for render texture [%s] !",
                static_cast<uint32_t>(dimension), name.c_str());
            return nullptr;
        }

        if (layers == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_ENGINE, "RenderTexture::create : array size of render texture [%s] is 0 !", name.c_str());
            return nullptr;
        }

        if (mipmaps == 0)
        {
            mipmaps = TextureSubresource::getFullMipCount(width, height);
        }

        return T3D_NEW RenderTexture(name, width, height, layers, cubemap, format, mipmaps, shaderReadable, accMode, gpuAccess);
    }

    //--------------------------------------------------------------------------

    RenderTexture::RenderTexture(const String &name, uint32_t width, uint32_t height, uint32_t layers, bool cubemap, PixelFormat format, uint32_t mipmaps, bool shaderReadable, uint32_t accMode, uint32_t gpuAccess)
        : Texture2D(name, width, height, format, mipmaps, 1, 0, Buffer{}, shaderReadable, accMode)
    {
        mDesc.arraySize = layers;
        mDesc.cubemap = cubemap;
        mDesc.gpuAccess = gpuAccess;
    }

    //--------------------------------------------------------------------------
    
    RenderTexture::~RenderTexture()
    {

    }

    //--------------------------------------------------------------------------

    TEXTURE_TYPE RenderTexture::getTextureType() const
    {
        return TEXTURE_TYPE::TT_RENDER_TEXTURE;
    }

    //--------------------------------------------------------------------------

    TEXTURE_TYPE RenderTexture::getRenderTextureType() const
    {
        if (mDesc.cubemap)
        {
            return getArraySize() > PixelBufferCubemap::FACE_COUNT ? TEXTURE_TYPE::TT_CUBE_ARRAY : TEXTURE_TYPE::TT_CUBE;
        }
        return getArraySize() > 1 ? TEXTURE_TYPE::TT_2D_ARRAY : TEXTURE_TYPE::TT_2D;
    }

    //--------------------------------------------------------------------------

    TResult RenderTexture::generateMips()
    {
        if (mPixelBuffer == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_ENGINE, "RenderTexture::generateMips : render texture [%s] has no pixel buffer !", getName().c_str());
            return T3D_ERR_INVALID_PARAM;
        }
        return T3D_AGENT.getActiveRHIContext()->generateMips(mPixelBuffer);
    }

    //--------------------------------------------------------------------------

    TResult RenderTexture::resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_ENGINE, "RenderTexture::resize : invalid size [%u x %u] !", width, height);
            return T3D_ERR_INVALID_PARAM;
        }

        if (mDesc.width == width && mDesc.height == height)
        {
            return T3D_OK;
        }

        mDesc.width = width;
        mDesc.height = height;

        if (mPixelBuffer == nullptr || mPixelBuffer->getRHIResource() == nullptr)
        {
            // 还没创建 GPU 资源，改完描述即可，后续 onCreate / onLoad 会按新尺寸创建
            return T3D_OK;
        }

        return T3D_AGENT.getActiveRHIContext()->resizeRenderTexture(this, width, height);
    }

    //--------------------------------------------------------------------------

    ResourcePtr RenderTexture::clone() const
    {
        RenderTexturePtr texture = create(getName(), mDesc.width, mDesc.height, mDesc.format, mDesc.mipmaps, mDesc.sampleDesc.Count, mDesc.sampleDesc.Quality, mDesc.shaderReadable, mCPUAccessMode, mDesc.gpuAccess);
        texture->mDesc.arraySize = mDesc.arraySize;
        texture->mDesc.cubemap = mDesc.cubemap;
        texture->cloneProperties(this);
        return texture;
    }

    //--------------------------------------------------------------------------

    void RenderTexture::cloneProperties(const Resource *const src)
    {
        const RenderTexture * const other = static_cast<const RenderTexture * const>(src);
    }

    //--------------------------------------------------------------------------

    TResult RenderTexture::onCreate()
    {
        mPixelBuffer = T3D_RENDER_BUFFER_MGR.loadRenderTexture(&mDesc, MemoryType::kVRAM, Usage::kStatic, mCPUAccessMode);
        if (mPixelBuffer != nullptr)
        {
            mPixelBuffer->setDebugName(getName());
        }
        if (mDesc.shaderReadable)
        {
            ensureDefaultSampler();
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult RenderTexture::onLoad(Archive *archive)
    {
        mPixelBuffer = T3D_RENDER_BUFFER_MGR.loadRenderTexture(&mDesc, MemoryType::kVRAM, Usage::kStatic, mCPUAccessMode);
        if (mPixelBuffer != nullptr)
        {
            mPixelBuffer->setDebugName(getName());
        }
        if (mDesc.shaderReadable)
        {
            ensureDefaultSampler();
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}

