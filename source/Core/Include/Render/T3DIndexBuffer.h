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


#ifndef __T3D_INDEX_BUFFER_H__
#define __T3D_INDEX_BUFFER_H__


#include "Render/T3DRenderBuffer.h"
#include "Render/T3DRenderConstant.h"


namespace Tiny3D
{
    class T3D_ENGINE_API IndexBuffer : public RenderBuffer
    {
    public:
        static IndexBufferPtr create(IndexType idxType, uint32_t idxCount, const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode);

        Type getType() const override;

        IndexType getIndexType() const { return mIndexType; }

        uint32_t getIndexCount() const { return mIndexCount; }

        uint32_t getIndexSize() const { return mIndexSize; }
        
    protected:
        IndexBuffer(IndexType idxType, uint32_t idxCount, const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode);

        ~IndexBuffer() override = default;
        
        bool onLoad() override;

        bool onUnload() override;

        IndexType   mIndexType {IndexType::E_IT_16BITS};
        uint32_t  mIndexSize {0};
        uint32_t  mIndexCount {0};
    };
}


#endif  /*__T3D_INDEX_BUFFER_H__*/
