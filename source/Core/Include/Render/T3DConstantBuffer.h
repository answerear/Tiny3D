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


#ifndef __T3D_CONSTANT_BUFFER_H__
#define __T3D_CONSTANT_BUFFER_H__


#include "Render/T3DRenderBuffer.h"


namespace Tiny3D
{
    class T3D_ENGINE_API ConstantBuffer : public RenderBuffer
    {
    public:
        static ConstantBufferPtr create(const String &name, uint32_t binding, const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode);
        
        ~ConstantBuffer() override = default;

        Type getType() const override;

        const String &getName() const { return mName; }

        uint32_t getBinding() const { return mBinding; }
        
    protected:
        ConstantBuffer(const String &name, uint32_t binding, const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode);

        bool onLoad() override;

        bool onUnload() override;

        String  mName {};
        uint32_t mBinding {0};
    };
}


#endif  /*__T3D_CONSTANT_BUFFER_H__*/
