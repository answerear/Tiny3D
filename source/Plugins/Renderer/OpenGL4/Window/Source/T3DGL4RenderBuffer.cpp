/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/

#include "T3DGL4RenderBuffer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    GL4VertexDeclarationPtr GL4VertexDeclaration::create()
    {
        return T3D_NEW GL4VertexDeclaration();
    }

    GL4VertexDeclaration::~GL4VertexDeclaration()
    {
        GL_SAFE_DELETE_VAO(GLVAO);
    }

    void *GL4VertexDeclaration::getNativeObject() const
    {
        return (void *)(uintptr_t)GLVAO;
    }

    //--------------------------------------------------------------------------

    GL4VertexBufferPtr GL4VertexBuffer::create()
    {
        return T3D_NEW GL4VertexBuffer();
    }

    GL4VertexBuffer::~GL4VertexBuffer()
    {
        GL_SAFE_DELETE_BUFFER(GLBuffer);
    }

    void *GL4VertexBuffer::getNativeObject() const
    {
        return (void *)(uintptr_t)GLBuffer;
    }

    //--------------------------------------------------------------------------

    GL4IndexBufferPtr GL4IndexBuffer::create()
    {
        return T3D_NEW GL4IndexBuffer();
    }

    GL4IndexBuffer::~GL4IndexBuffer()
    {
        GL_SAFE_DELETE_BUFFER(GLBuffer);
    }

    void *GL4IndexBuffer::getNativeObject() const
    {
        return (void *)(uintptr_t)GLBuffer;
    }

    //--------------------------------------------------------------------------

    GL4ConstantBufferPtr GL4ConstantBuffer::create()
    {
        return T3D_NEW GL4ConstantBuffer();
    }

    GL4ConstantBuffer::~GL4ConstantBuffer()
    {
        GL_SAFE_DELETE_BUFFER(GLBuffer);
    }

    void *GL4ConstantBuffer::getNativeObject() const
    {
        return (void *)(uintptr_t)GLBuffer;
    }

    //--------------------------------------------------------------------------

    GL4PixelBuffer1DPtr GL4PixelBuffer1D::create()
    {
        return T3D_NEW GL4PixelBuffer1D();
    }

    GL4PixelBuffer1D::~GL4PixelBuffer1D()
    {
        GL_SAFE_DELETE_TEXTURE(GLTexture);
    }

    void *GL4PixelBuffer1D::getNativeObject() const
    {
        return (void *)(uintptr_t)GLTexture;
    }

    //--------------------------------------------------------------------------

    GL4PixelBuffer2DPtr GL4PixelBuffer2D::create()
    {
        return T3D_NEW GL4PixelBuffer2D();
    }

    GL4PixelBuffer2D::~GL4PixelBuffer2D()
    {
        GL_SAFE_DELETE_FBO(GLResolveFBO);
        GL_SAFE_DELETE_TEXTURE(GLResolveTex);
        GL_SAFE_DELETE_RBO(GLDepthRBO);
        GL_SAFE_DELETE_FBO(GLFBO);
        GL_SAFE_DELETE_TEXTURE(GLTexture);
    }

    void *GL4PixelBuffer2D::getNativeObject() const
    {
        return (void *)(uintptr_t)GLTexture;
    }

    //--------------------------------------------------------------------------

    GL4PixelBuffer3DPtr GL4PixelBuffer3D::create()
    {
        return T3D_NEW GL4PixelBuffer3D();
    }

    GL4PixelBuffer3D::~GL4PixelBuffer3D()
    {
        GL_SAFE_DELETE_TEXTURE(GLTexture);
    }

    void *GL4PixelBuffer3D::getNativeObject() const
    {
        return (void *)(uintptr_t)GLTexture;
    }

    //--------------------------------------------------------------------------

    GL4PixelBufferCubemapPtr GL4PixelBufferCubemap::create()
    {
        return T3D_NEW GL4PixelBufferCubemap();
    }

    GL4PixelBufferCubemap::~GL4PixelBufferCubemap()
    {
        GL_SAFE_DELETE_TEXTURE(GLTexture);
    }

    void *GL4PixelBufferCubemap::getNativeObject() const
    {
        return (void *)(uintptr_t)GLTexture;
    }

    //--------------------------------------------------------------------------
}
