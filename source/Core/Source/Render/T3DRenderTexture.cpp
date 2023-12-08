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


#include "Render/T3DRenderTexture.h"
#include "Render/T3DPixelBuffer.h"
#include "Render/T3DRenderResourceManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    RenderTexturePtr RenderTexture::create(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality)
    {
        return new RenderTexture(name, width, height, format, mipmaps, MSAACount, MSAAQuality);
    }
    
    //--------------------------------------------------------------------------
    
    RenderTexture::RenderTexture(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality)
        : Texture2D(name, width, height, format, mipmaps, MSAACount, MSAAQuality, Buffer{})
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
        RenderTexturePtr texture = create(getName(), mDesc.width, mDesc.height, mDesc.format, mDesc.mipmaps, mDesc.sampleDesc.Count, mDesc.sampleDesc.Quality);
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

    TResult RenderTexture::onLoad()
    {
        mPixelBuffer = T3D_RENDER_BUFFER_MGR.loadRenderTexture(&mDesc, MemoryType::kVRAM, Usage::kStatic, CPUAccessMode::kCPUNone);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}

