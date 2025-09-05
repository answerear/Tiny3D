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
        return T3D_NEW PixelBuffer2D(desc, memType, usage, accMode, isRenderTexture);
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
