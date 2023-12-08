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


#ifndef __T3D_RENDER_BUFFER_H__
#define __T3D_RENDER_BUFFER_H__


#include "Render/T3DRenderResource.h"
#include "Render/T3DRenderConstant.h"


namespace Tiny3D
{
    using RenderBuffeReadCallback = TFunction<void(size_t size, void *dst)>;
    
    class T3D_ENGINE_API RenderBuffer : public RenderResource
    {
    public:
        virtual size_t readData(size_t offset, size_t size, void *dst);

        virtual TResult readData(size_t offset, size_t size, RenderBuffeReadCallback callback);

        virtual size_t writeData(size_t offset, size_t size, const void *src, bool discardWholeBuffer = false);

        virtual size_t copyData(RenderBufferPtr srcBuffer, size_t srcOffset, size_t dstOffset, size_t size, bool discardWholeBuffer = false);

        virtual size_t copyData(RenderBufferPtr srcBuffer);

        size_t getBufferSize() const { return mBuffer.DataSize; }

        MemoryType getMemoryType() const { return mMemoryType; }
        
        Usage getUsage() const { return mUsage; }
        
        uint32_t getCPUAccessMode() const { return mAccessMode; }
        
    protected:
        RenderBuffer(const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode);

        ~RenderBuffer() override;
        
        Buffer      mBuffer {};
        MemoryType  mMemoryType {MemoryType::kVRAM};
        Usage       mUsage {Usage::kStatic};
        uint32_t    mAccessMode {kCPUNone};
    };
}


#endif  /*__T3D_RENDER_BUFFER_H__*/
