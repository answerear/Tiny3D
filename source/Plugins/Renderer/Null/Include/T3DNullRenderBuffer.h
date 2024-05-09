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

#ifndef __T3D_NULL_RENDER_BUFFER_H__
#define __T3D_NULL_RENDER_BUFFER_H__


#include "T3DNullPrerequisites.h"


namespace Tiny3D
{
    class NullVertexDeclaration : public RHIVertexDeclaration
    {
    public:
        static NullVertexDeclarationPtr create();

        ~NullVertexDeclaration() override;

        void *getNativeObject() const override;
    protected:
        NullVertexDeclaration() = default;
    };
    
    class NullVertexBuffer : public RHIVertexBuffer
    {
    public:
        static NullVertexBufferPtr create();

        ~NullVertexBuffer() override;

        void *getNativeObject() const override;
    protected:
        NullVertexBuffer() = default;
    };

    class NullIndexBuffer : public RHIIndexBuffer
    {
    public:
        static NullIndexBufferPtr create();

        ~NullIndexBuffer() override;

        void *getNativeObject() const override;        
    protected:
        NullIndexBuffer() = default;
    };

    class NullConstantBuffer : public RHIConstantBuffer
    {
    public:
        static NullConstantBufferPtr create();

        ~NullConstantBuffer() override;

        void *getNativeObject() const override;
    protected:
        NullConstantBuffer() = default;
    };

    class NullPixelBuffer1D : public RHIPixelBuffer1D
    {
    public:
        static NullPixelBuffer1DPtr create();

        ~NullPixelBuffer1D() override;

        void *getNativeObject() const override;
    protected:
        NullPixelBuffer1D() = default;
    };

    class NullPixelBuffer2D : public RHIPixelBuffer2D
    {
    public:
        static NullPixelBuffer2DPtr create();

        ~NullPixelBuffer2D() override;

        void *getNativeObject() const override;
    protected:
        NullPixelBuffer2D() = default;
    };

    class NullPixelBuffer3D : public RHIPixelBuffer3D
    {
    public:
        void *getNativeObject() const override;
    };
}


#endif    /*__T3D_NULL_RENDER_BUFFER_H__*/
