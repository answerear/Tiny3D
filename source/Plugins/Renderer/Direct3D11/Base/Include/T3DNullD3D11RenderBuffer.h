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

#ifndef __T3D_NULLD3D11_RENDER_BUFFER_H__
#define __T3D_NULLD3D11_RENDER_BUFFER_H__


#include "T3DNullD3D11Prerequisites.h"


namespace Tiny3D
{
    class NullD3D11VertexDeclaration : public RHIVertexDeclaration
    {
    public:
        static NullD3D11VertexDeclarationPtr create();

        ~NullD3D11VertexDeclaration() override;

        void *getNativeObject() const override;
    protected:
        NullD3D11VertexDeclaration() = default;
    };
    
    class NullD3D11VertexBuffer : public RHIVertexBuffer
    {
    public:
        static NullD3D11VertexBufferPtr create();

        ~NullD3D11VertexBuffer() override;

        void *getNativeObject() const override;
    protected:
        NullD3D11VertexBuffer() = default;
    };

    class NullD3D11IndexBuffer : public RHIIndexBuffer
    {
    public:
        static NullD3D11IndexBufferPtr create();

        ~NullD3D11IndexBuffer() override;

        void *getNativeObject() const override;        
    protected:
        NullD3D11IndexBuffer() = default;
    };

    class NullD3D11ConstantBuffer : public RHIConstantBuffer
    {
    public:
        static NullD3D11ConstantBufferPtr create();

        ~NullD3D11ConstantBuffer() override;

        void *getNativeObject() const override;
    protected:
        NullD3D11ConstantBuffer() = default;
    };

    class NullD3D11PixelBuffer1D : public RHIPixelBuffer1D
    {
    public:
        static NullD3D11PixelBuffer1DPtr create();

        ~NullD3D11PixelBuffer1D() override;

        void *getNativeObject() const override;
    protected:
        NullD3D11PixelBuffer1D() = default;
    };

    class NullD3D11PixelBuffer2D : public RHIPixelBuffer2D
    {
    public:
        static NullD3D11PixelBuffer2DPtr create();

        ~NullD3D11PixelBuffer2D() override;

        void *getNativeObject() const override;
    protected:
        NullD3D11PixelBuffer2D() = default;
    };

    class NullD3D11PixelBuffer3D : public RHIPixelBuffer3D
    {
    public:
        void *getNativeObject() const override;
    };
}


#endif    /*__T3D_NULLD3D11_RENDER_BUFFER_H__*/
