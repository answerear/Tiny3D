/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DGLES3RenderBuffer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------
    // GLES3VertexDeclaration
    //--------------------------------------------------------------------------

    GLES3VertexDeclarationPtr GLES3VertexDeclaration::create()
    {
        return T3D_NEW GLES3VertexDeclaration();
    }

    GLES3VertexDeclaration::~GLES3VertexDeclaration()
    {
        GL_SAFE_DELETE_VAO(GLVAO);
    }

    void *GLES3VertexDeclaration::getNativeObject() const
    {
        return (void *)(intptr_t)GLVAO;
    }

    //--------------------------------------------------------------------------
    // GLES3VertexBuffer
    //--------------------------------------------------------------------------

    GLES3VertexBufferPtr GLES3VertexBuffer::create()
    {
        return T3D_NEW GLES3VertexBuffer();
    }

    GLES3VertexBuffer::~GLES3VertexBuffer()
    {
        GL_SAFE_DELETE_BUFFER(GLBuffer);
    }

    void *GLES3VertexBuffer::getNativeObject() const
    {
        return (void *)(intptr_t)GLBuffer;
    }

    //--------------------------------------------------------------------------
    // GLES3IndexBuffer
    //--------------------------------------------------------------------------

    GLES3IndexBufferPtr GLES3IndexBuffer::create()
    {
        return T3D_NEW GLES3IndexBuffer();
    }

    GLES3IndexBuffer::~GLES3IndexBuffer()
    {
        GL_SAFE_DELETE_BUFFER(GLBuffer);
    }

    void *GLES3IndexBuffer::getNativeObject() const
    {
        return (void *)(intptr_t)GLBuffer;
    }

    //--------------------------------------------------------------------------
    // GLES3ConstantBuffer
    //--------------------------------------------------------------------------

    GLES3ConstantBufferPtr GLES3ConstantBuffer::create()
    {
        return T3D_NEW GLES3ConstantBuffer();
    }

    GLES3ConstantBuffer::~GLES3ConstantBuffer()
    {
        GL_SAFE_DELETE_BUFFER(GLBuffer);
    }

    void *GLES3ConstantBuffer::getNativeObject() const
    {
        return (void *)(intptr_t)GLBuffer;
    }

    //--------------------------------------------------------------------------
    // GLES3PixelBuffer1D
    //--------------------------------------------------------------------------

    GLES3PixelBuffer1DPtr GLES3PixelBuffer1D::create()
    {
        return T3D_NEW GLES3PixelBuffer1D();
    }

    GLES3PixelBuffer1D::~GLES3PixelBuffer1D()
    {
        GL_SAFE_DELETE_TEXTURE(GLTexture);
    }

    void *GLES3PixelBuffer1D::getNativeObject() const
    {
        return (void *)(intptr_t)GLTexture;
    }

    //--------------------------------------------------------------------------
    // GLES3PixelBuffer2D
    //--------------------------------------------------------------------------

    GLES3PixelBuffer2DPtr GLES3PixelBuffer2D::create()
    {
        return T3D_NEW GLES3PixelBuffer2D();
    }

    GLES3PixelBuffer2D::~GLES3PixelBuffer2D()
    {
        GL_SAFE_DELETE_FBO(GLResolveFBO);
        GL_SAFE_DELETE_TEXTURE(GLResolveTex);
        GL_SAFE_DELETE_RBO(GLDepthRBO);
        GL_SAFE_DELETE_FBO(GLFBO);
        GL_SAFE_DELETE_TEXTURE(GLTexture);
    }

    void *GLES3PixelBuffer2D::getNativeObject() const
    {
        return (void *)(intptr_t)GLTexture;
    }

    //--------------------------------------------------------------------------
    // GLES3PixelBuffer3D
    //--------------------------------------------------------------------------

    GLES3PixelBuffer3DPtr GLES3PixelBuffer3D::create()
    {
        return T3D_NEW GLES3PixelBuffer3D();
    }

    GLES3PixelBuffer3D::~GLES3PixelBuffer3D()
    {
        GL_SAFE_DELETE_TEXTURE(GLTexture);
    }

    void *GLES3PixelBuffer3D::getNativeObject() const
    {
        return (void *)(intptr_t)GLTexture;
    }
}
