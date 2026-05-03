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

#ifndef __T3D_D3D11CONSOLE_RENDER_BUFFER_H__
#define __T3D_D3D11CONSOLE_RENDER_BUFFER_H__


#include "T3DD3D11ConsolePrerequisites.h"


namespace Tiny3D
{
    class D3D11ConsoleVertexDeclaration : public RHIVertexDeclaration
    {
    public:
        static D3D11ConsoleVertexDeclarationPtr create();

        ~D3D11ConsoleVertexDeclaration() override;

        void *getNativeObject() const override;
    protected:
        D3D11ConsoleVertexDeclaration() = default;
    };
    
    class D3D11ConsoleVertexBuffer : public RHIVertexBuffer
    {
    public:
        static D3D11ConsoleVertexBufferPtr create();

        ~D3D11ConsoleVertexBuffer() override;

        void *getNativeObject() const override;
    protected:
        D3D11ConsoleVertexBuffer() = default;
    };

    class D3D11ConsoleIndexBuffer : public RHIIndexBuffer
    {
    public:
        static D3D11ConsoleIndexBufferPtr create();

        ~D3D11ConsoleIndexBuffer() override;

        void *getNativeObject() const override;        
    protected:
        D3D11ConsoleIndexBuffer() = default;
    };

    class D3D11ConsoleConstantBuffer : public RHIConstantBuffer
    {
    public:
        static D3D11ConsoleConstantBufferPtr create();

        ~D3D11ConsoleConstantBuffer() override;

        void *getNativeObject() const override;
    protected:
        D3D11ConsoleConstantBuffer() = default;
    };

    class D3D11ConsolePixelBuffer1D : public RHIPixelBuffer1D
    {
    public:
        static D3D11ConsolePixelBuffer1DPtr create();

        ~D3D11ConsolePixelBuffer1D() override;

        void *getNativeObject() const override;
    protected:
        D3D11ConsolePixelBuffer1D() = default;
    };

    class D3D11ConsolePixelBuffer2D : public RHIPixelBuffer2D
    {
    public:
        static D3D11ConsolePixelBuffer2DPtr create();

        ~D3D11ConsolePixelBuffer2D() override;

        void *getNativeObject() const override;
    protected:
        D3D11ConsolePixelBuffer2D() = default;
    };

    class D3D11ConsolePixelBuffer3D : public RHIPixelBuffer3D
    {
    public:
        void *getNativeObject() const override;
    };
}


#endif    /*__T3D_D3D11CONSOLE_RENDER_BUFFER_H__*/
