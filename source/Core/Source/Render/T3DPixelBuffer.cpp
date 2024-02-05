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


#include "Render/T3DPixelBuffer.h"
#include "Resource/T3DImage.h"
#include "Kernel/T3DAgent.h"
#include "RHI/T3DRHIContext.h"
#include "RHI/T3DRHIPixelBuffer.h"


namespace Tiny3D
{
    template class PixelBufferT<PixelBuffer1D, PixelBuffer1DDesc>;
    template class PixelBufferT<PixelBuffer2D, PixelBuffer2DDesc>;
    template class PixelBufferT<PixelBuffer3D, PixelBuffer3DDesc>;
    
    //--------------------------------------------------------------------------

    PixelBuffer::PixelBuffer(const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode)
        : RenderBuffer(buffer, memType, usage, accMode)
    {
        
    }
    
    //--------------------------------------------------------------------------

    PixelBuffer1D::PixelBuffer1D(PixelBuffer1DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode)
        : PixelBufferT(desc, memType, usage, accMode)
    {
        
    }

    //--------------------------------------------------------------------------

    RenderResource::Type PixelBuffer1D::getType() const
    {
        return Type::kPixelBuffer1D;
    }

    //--------------------------------------------------------------------------

    bool PixelBuffer1D::onLoad()
    {
        mRHIResource = T3D_AGENT.getActiveRHIContext()->createPixelBuffer1D(this);
        return true;
    }

    //--------------------------------------------------------------------------

    bool PixelBuffer1D::onUnload()
    {
        mRHIResource = nullptr;
        return true;
    }
    
    //--------------------------------------------------------------------------

    PixelBuffer2DPtr PixelBuffer2D::create(PixelBuffer2DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode, bool isRenderTexture)
    {
        return new PixelBuffer2D(desc, memType, usage, accMode, isRenderTexture);
    }

    //--------------------------------------------------------------------------

    PixelBuffer2D::PixelBuffer2D(PixelBuffer2DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode, bool isRenderTexture)
        : PixelBufferT(desc, memType, usage, accMode)
        , mIsRenderTexture(isRenderTexture)
    {
        // size_t bpp = Image::getBPP(mFormat);
        // mPitch = Image::calcPitch(mWidth, bpp);
    }

    //--------------------------------------------------------------------------

    RenderResource::Type PixelBuffer2D::getType() const
    {
        return Type::kPixelBuffer2D;
    }

    //--------------------------------------------------------------------------

    bool PixelBuffer2D::onLoad()
    {
        if (mIsRenderTexture)
        {
            mRHIResource = T3D_AGENT.getActiveRHIContext()->createRenderTexture(this);
        }
        else
        {
            mRHIResource = T3D_AGENT.getActiveRHIContext()->createPixelBuffer2D(this);
        }
        
        return true;
    }

    //--------------------------------------------------------------------------

    bool PixelBuffer2D::onUnload()
    {
        mRHIResource = nullptr;
        return true;
    }

    //--------------------------------------------------------------------------

    PixelBuffer3D::PixelBuffer3D(PixelBuffer3DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode)
        : PixelBufferT(desc, memType, usage, accMode)
    {
        
    }

    //--------------------------------------------------------------------------

    RenderResource::Type PixelBuffer3D::getType() const
    {
        return Type::kPixelBuffer3D;
    }

    //--------------------------------------------------------------------------

    bool PixelBuffer3D::onLoad()
    {
        mRHIResource = T3D_AGENT.getActiveRHIContext()->createPixelBuffer3D(this);
        return true;
    }

    //--------------------------------------------------------------------------

    bool PixelBuffer3D::onUnload()
    {
        mRHIResource = nullptr;
        return true;
    }
    
    //--------------------------------------------------------------------------
}
