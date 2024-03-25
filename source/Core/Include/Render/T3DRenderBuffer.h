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

        const Buffer &getBuffer() const { return mBuffer; }

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
