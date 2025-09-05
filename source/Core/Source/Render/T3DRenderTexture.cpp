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


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    RenderTexturePtr RenderTexture::create(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, bool shaderReadable)
    {
        return T3D_NEW RenderTexture(name, width, height, format, mipmaps, MSAACount, MSAAQuality, shaderReadable);
    }
    
    //--------------------------------------------------------------------------
    
    RenderTexture::RenderTexture(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, bool shaderReadable)
        : Texture2D(name, width, height, format, mipmaps, MSAACount, MSAAQuality, Buffer{}, shaderReadable)
    {

    }

    //--------------------------------------------------------------------------

    RenderTexture::RenderTexture(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data, bool shaderReadable)
        : Texture2D(name, width, height, format, mipmaps, MSAACount, MSAAQuality, data, shaderReadable)
    {
        
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

    ResourcePtr RenderTexture::clone() const
    {
        RenderTexturePtr texture = create(getName(), mDesc.width, mDesc.height, mDesc.format, mDesc.mipmaps, mDesc.sampleDesc.Count, mDesc.sampleDesc.Quality, mDesc.shaderReadable);
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
        mPixelBuffer = T3D_RENDER_BUFFER_MGR.loadRenderTexture(&mDesc, MemoryType::kVRAM, Usage::kStatic, CPUAccessMode::kCPUNone);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult RenderTexture::onLoad(Archive *archive)
    {
        mPixelBuffer = T3D_RENDER_BUFFER_MGR.loadRenderTexture(&mDesc, MemoryType::kVRAM, Usage::kStatic, CPUAccessMode::kCPUNone);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}

