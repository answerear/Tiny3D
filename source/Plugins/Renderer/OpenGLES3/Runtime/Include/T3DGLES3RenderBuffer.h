/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#ifndef __T3D_GLES3_RENDER_BUFFER_H__
#define __T3D_GLES3_RENDER_BUFFER_H__


#include "T3DGLES3Prerequisites.h"


namespace Tiny3D
{
    class GLES3VertexDeclaration : public RHIVertexDeclaration
    {
    public:
        static GLES3VertexDeclarationPtr create();
        ~GLES3VertexDeclaration() override;
        void *getNativeObject() const override;

        GLuint GLVAO {0};
    protected:
        GLES3VertexDeclaration() = default;
    };

    class GLES3VertexBuffer : public RHIVertexBuffer
    {
    public:
        static GLES3VertexBufferPtr create();
        ~GLES3VertexBuffer() override;
        void *getNativeObject() const override;

        GLuint GLBuffer {0};
    protected:
        GLES3VertexBuffer() = default;
    };

    class GLES3IndexBuffer : public RHIIndexBuffer
    {
    public:
        static GLES3IndexBufferPtr create();
        ~GLES3IndexBuffer() override;
        void *getNativeObject() const override;

        GLuint GLBuffer {0};
    protected:
        GLES3IndexBuffer() = default;
    };

    class GLES3ConstantBuffer : public RHIConstantBuffer
    {
    public:
        static GLES3ConstantBufferPtr create();
        ~GLES3ConstantBuffer() override;
        void *getNativeObject() const override;

        GLuint GLBuffer {0};
    protected:
        GLES3ConstantBuffer() = default;
    };

    class GLES3PixelBuffer1D : public RHIPixelBuffer1D
    {
    public:
        static GLES3PixelBuffer1DPtr create();
        ~GLES3PixelBuffer1D() override;
        void *getNativeObject() const override;

        GLuint GLTexture {0};
    protected:
        GLES3PixelBuffer1D() = default;
    };

    class GLES3PixelBuffer2D : public RHIPixelBuffer2D
    {
    public:
        static GLES3PixelBuffer2DPtr create();
        ~GLES3PixelBuffer2D() override;
        void *getNativeObject() const override;

        GLuint GLTexture {0};
        GLuint GLFBO {0};
        GLuint GLDepthRBO {0};
        GLuint GLResolveTex {0};
        GLuint GLResolveFBO {0};
        uint32_t GLMSAACount {1};
    protected:
        GLES3PixelBuffer2D() = default;
    };

    class GLES3PixelBuffer3D : public RHIPixelBuffer3D
    {
    public:
        static GLES3PixelBuffer3DPtr create();
        ~GLES3PixelBuffer3D() override;
        void *getNativeObject() const override;

        GLuint GLTexture {0};
    protected:
        GLES3PixelBuffer3D() = default;
    };

    class GLES3PixelBufferCubemap : public RHIPixelBufferCubemap
    {
    public:
        static GLES3PixelBufferCubemapPtr create();
        ~GLES3PixelBufferCubemap() override;
        void *getNativeObject() const override;

        GLuint GLTexture {0};
    protected:
        GLES3PixelBufferCubemap() = default;
    };
}


#endif  /*__T3D_GLES3_RENDER_BUFFER_H__*/
