/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#ifndef __T3D_GL4_RENDER_BUFFER_H__
#define __T3D_GL4_RENDER_BUFFER_H__


#include "T3DGL4Prerequisites.h"


namespace Tiny3D
{
    class GL4VertexDeclaration : public RHIVertexDeclaration
    {
    public:
        static GL4VertexDeclarationPtr create();
        ~GL4VertexDeclaration() override;
        void *getNativeObject() const override;

        GLuint GLVAO {0};
    protected:
        GL4VertexDeclaration() = default;
    };

    class GL4VertexBuffer : public RHIVertexBuffer
    {
    public:
        static GL4VertexBufferPtr create();
        ~GL4VertexBuffer() override;
        void *getNativeObject() const override;

        GLuint GLBuffer {0};
    protected:
        GL4VertexBuffer() = default;
    };

    class GL4IndexBuffer : public RHIIndexBuffer
    {
    public:
        static GL4IndexBufferPtr create();
        ~GL4IndexBuffer() override;
        void *getNativeObject() const override;

        GLuint GLBuffer {0};
    protected:
        GL4IndexBuffer() = default;
    };

    class GL4ConstantBuffer : public RHIConstantBuffer
    {
    public:
        static GL4ConstantBufferPtr create();
        ~GL4ConstantBuffer() override;
        void *getNativeObject() const override;

        GLuint GLBuffer {0};
    protected:
        GL4ConstantBuffer() = default;
    };

    class GL4PixelBuffer1D : public RHIPixelBuffer1D
    {
    public:
        static GL4PixelBuffer1DPtr create();
        ~GL4PixelBuffer1D() override;
        void *getNativeObject() const override;

        GLuint GLTexture {0};
    protected:
        GL4PixelBuffer1D() = default;
    };

    class GL4PixelBuffer2D : public RHIPixelBuffer2D
    {
    public:
        static GL4PixelBuffer2DPtr create();
        ~GL4PixelBuffer2D() override;
        void *getNativeObject() const override;

        GLuint GLTexture {0};
        GLuint GLFBO {0};
        GLuint GLDepthRBO {0};
    protected:
        GL4PixelBuffer2D() = default;
    };

    class GL4PixelBuffer3D : public RHIPixelBuffer3D
    {
    public:
        static GL4PixelBuffer3DPtr create();
        ~GL4PixelBuffer3D() override;
        void *getNativeObject() const override;

        GLuint GLTexture {0};
    protected:
        GL4PixelBuffer3D() = default;
    };
}


#endif  /*__T3D_GL4_RENDER_BUFFER_H__*/
