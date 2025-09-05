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


#ifndef __T3D_PIXEL_BUFFER_H__
#define __T3D_PIXEL_BUFFER_H__


#include "Render/T3DRenderBuffer.h"
#include "Kernel/T3DConstant.h"
#include "Render/T3DRenderBufferDesc.h"


namespace Tiny3D
{
    class T3D_ENGINE_API PixelBuffer : public RenderBuffer
    {
    public:
        PixelBuffer(const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode);

    protected:
        ~PixelBuffer() override = default;
    };
    
    template<typename Object_t, typename Descriptor_t>
    class T3D_ENGINE_API PixelBufferT : public PixelBuffer
    {
    public:
        static SmartPtr<Object_t> create(Descriptor_t *desc, MemoryType memType, Usage usage, uint32_t accMode)
        {
            return T3D_NEW Object_t(desc, memType, usage, accMode);
        }

        const Descriptor_t &getDescriptor() const { return *mDesc; }

    protected:
        PixelBufferT(Descriptor_t *desc, MemoryType memType, Usage usage, uint32_t accMode)
            : PixelBuffer(desc->buffer, memType, usage, accMode)
            , mDesc(desc)
        {
            
        }
        
    protected:
        Descriptor_t *mDesc {nullptr};
    };

    template class T3D_ENGINE_API PixelBufferT<PixelBuffer1D, PixelBuffer1DDesc>;
    
    class T3D_ENGINE_API PixelBuffer1D : public PixelBufferT<PixelBuffer1D, PixelBuffer1DDesc>
    {
    public:
        PixelBuffer1D(PixelBuffer1DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode);

        Type getType() const override;
        
    protected:        
        ~PixelBuffer1D() override = default;

        bool onLoad() override;

        bool onUnload() override;
    };

    template class T3D_ENGINE_API PixelBufferT<PixelBuffer2D, PixelBuffer2DDesc>;
    
    class T3D_ENGINE_API PixelBuffer2D : public PixelBufferT<PixelBuffer2D, PixelBuffer2DDesc>
    {
    public:
        static PixelBuffer2DPtr create(PixelBuffer2DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode, bool isRenderTexture);

        PixelBuffer2D(PixelBuffer2DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode, bool isRenderTexture = false);

        Type getType() const override;
        
    protected:
        ~PixelBuffer2D() override = default;
        
        bool onLoad() override;

        bool onUnload() override;

    protected:
        /// 是否 render texture
        bool    mIsRenderTexture {false};
    };

    template class T3D_ENGINE_API PixelBufferT<PixelBuffer3D, PixelBuffer3DDesc>;
    
    class T3D_ENGINE_API PixelBuffer3D : public PixelBufferT<PixelBuffer3D, PixelBuffer3DDesc>
    {
    public:
        PixelBuffer3D(PixelBuffer3DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode);

        Type getType() const override;
        
    protected:
        ~PixelBuffer3D() override = default;

        bool onLoad() override;

        bool onUnload() override;
    };
}


#endif  /*__T3D_PIXEL_BUFFER_H__*/
