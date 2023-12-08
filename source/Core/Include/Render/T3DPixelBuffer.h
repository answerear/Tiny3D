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


#ifndef __T3D_PIXEL_BUFFER_H__
#define __T3D_PIXEL_BUFFER_H__


#include "Render/T3DRenderBuffer.h"
#include "Kernel/T3DConstant.h"
#include "Render/T3DRenderBufferDesc.h"


namespace Tiny3D
{
    template<typename Object_t, typename Descriptor_t>
    class T3D_ENGINE_API PixelBufferT : public RenderBuffer
    {
    public:
        static SmartPtr<Object_t> create(Descriptor_t *desc, MemoryType memType, Usage usage, uint32_t accMode)
        {
            return new Object_t(desc, memType, usage, accMode);
        }

        const Descriptor_t &getDescriptor() const { return *mDesc; }

    protected:
        PixelBufferT(Descriptor_t *desc, MemoryType memType, Usage usage, uint32_t accMode)
            : RenderBuffer(desc->buffer, memType, usage, accMode)
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
