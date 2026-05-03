/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#ifndef __T3D_NULLGL4_RENDER_BUFFER_H__
#define __T3D_NULLGL4_RENDER_BUFFER_H__


#include "T3DNullGL4Prerequisites.h"


namespace Tiny3D
{
    class NullGL4VertexDeclaration : public RHIVertexDeclaration
    {
    public:
        static NullGL4VertexDeclarationPtr create();
        ~NullGL4VertexDeclaration() override;
        void *getNativeObject() const override;
    protected:
        NullGL4VertexDeclaration() = default;
    };

    class NullGL4VertexBuffer : public RHIVertexBuffer
    {
    public:
        static NullGL4VertexBufferPtr create();
        ~NullGL4VertexBuffer() override;
        void *getNativeObject() const override;
    protected:
        NullGL4VertexBuffer() = default;
    };

    class NullGL4IndexBuffer : public RHIIndexBuffer
    {
    public:
        static NullGL4IndexBufferPtr create();
        ~NullGL4IndexBuffer() override;
        void *getNativeObject() const override;
    protected:
        NullGL4IndexBuffer() = default;
    };

    class NullGL4ConstantBuffer : public RHIConstantBuffer
    {
    public:
        static NullGL4ConstantBufferPtr create();
        ~NullGL4ConstantBuffer() override;
        void *getNativeObject() const override;
    protected:
        NullGL4ConstantBuffer() = default;
    };

    class NullGL4PixelBuffer1D : public RHIPixelBuffer1D
    {
    public:
        static NullGL4PixelBuffer1DPtr create();
        ~NullGL4PixelBuffer1D() override;
        void *getNativeObject() const override;
    protected:
        NullGL4PixelBuffer1D() = default;
    };

    class NullGL4PixelBuffer2D : public RHIPixelBuffer2D
    {
    public:
        static NullGL4PixelBuffer2DPtr create();
        ~NullGL4PixelBuffer2D() override;
        void *getNativeObject() const override;
    protected:
        NullGL4PixelBuffer2D() = default;
    };

    class NullGL4PixelBuffer3D : public RHIPixelBuffer3D
    {
    public:
        void *getNativeObject() const override;
    };
}


#endif    /*__T3D_NULLGL4_RENDER_BUFFER_H__*/
