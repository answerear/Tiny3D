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

#ifndef __T3D_VKCONSOLE_RENDER_BUFFER_H__
#define __T3D_VKCONSOLE_RENDER_BUFFER_H__


#include "T3DVKConsolePrerequisites.h"


namespace Tiny3D
{
    class VKConsoleVertexDeclaration : public RHIVertexDeclaration
    {
    public:
        static VKConsoleVertexDeclarationPtr create();

        ~VKConsoleVertexDeclaration() override;

        void *getNativeObject() const override;

    protected:
        VKConsoleVertexDeclaration() = default;
    };
    
    class VKConsoleVertexBuffer : public RHIVertexBuffer
    {
    public:
        static VKConsoleVertexBufferPtr create();

        ~VKConsoleVertexBuffer() override;

        void *getNativeObject() const override;

    protected:
        VKConsoleVertexBuffer() = default;
    };

    class VKConsoleIndexBuffer : public RHIIndexBuffer
    {
    public:
        static VKConsoleIndexBufferPtr create();

        ~VKConsoleIndexBuffer() override;

        void *getNativeObject() const override;

    protected:
        VKConsoleIndexBuffer() = default;
    };

    class VKConsoleConstantBuffer : public RHIConstantBuffer
    {
    public:
        static VKConsoleConstantBufferPtr create();

        ~VKConsoleConstantBuffer() override;

        void *getNativeObject() const override;

    protected:
        VKConsoleConstantBuffer() = default;
    };

    class VKConsolePixelBuffer1D : public RHIPixelBuffer1D
    {
    public:
        static VKConsolePixelBuffer1DPtr create();

        ~VKConsolePixelBuffer1D() override;

        void *getNativeObject() const override;

    protected:
        VKConsolePixelBuffer1D() = default;
    };

    class VKConsolePixelBuffer2D : public RHIPixelBuffer2D
    {
    public:
        static VKConsolePixelBuffer2DPtr create();

        ~VKConsolePixelBuffer2D() override;

        void *getNativeObject() const override;

    protected:
        VKConsolePixelBuffer2D() = default;
    };

    class VKConsolePixelBuffer3D : public RHIPixelBuffer3D
    {
    public:
        void *getNativeObject() const override;
    };
}


#endif    /*__T3D_VKCONSOLE_RENDER_BUFFER_H__*/
