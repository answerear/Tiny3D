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
    class T3D_ENGINE_API PixelBuffer1D : public RenderBuffer
    {
    public:
        static PixelBuffer1DPtr create(PixelBuffer1DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode);

        Type getType() const override;
        
    protected:
        PixelBuffer1D(PixelBuffer1DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode);

        ~PixelBuffer1D() override = default;

        bool onLoad() override;

        bool onUnload() override;

    protected:
        /// 缓冲区描述
        PixelBuffer1DDesc   *mDesc {nullptr};
    };
    
    class T3D_ENGINE_API PixelBuffer2D : public RenderBuffer
    {
    public:
        static PixelBuffer2DPtr create(PixelBuffer2DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode);

        Type getType() const override;
        
    protected:
        PixelBuffer2D(PixelBuffer2DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode);

        ~PixelBuffer2D() override = default;
        
        bool onLoad() override;

        bool onUnload() override;

    protected:
        /// 缓冲区描述
        PixelBuffer2DDesc   *mDesc {nullptr};
    };

    class T3D_ENGINE_API PixelBuffer3D : public RenderBuffer
    {
    public:
        static PixelBuffer3DPtr create(PixelBuffer3DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode);

        Type getType() const override;
        
    protected:
        PixelBuffer3D(PixelBuffer3DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode);

        ~PixelBuffer3D() override = default;

        bool onLoad() override;

        bool onUnload() override;

    protected:
        /// 缓冲区描述
        PixelBuffer3DDesc   *mDesc {nullptr};
    };
}


#endif  /*__T3D_PIXEL_BUFFER_H__*/
