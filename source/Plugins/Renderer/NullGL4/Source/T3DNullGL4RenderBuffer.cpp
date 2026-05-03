/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DNullGL4RenderBuffer.h"


namespace Tiny3D
{
    NullGL4VertexDeclarationPtr NullGL4VertexDeclaration::create() { return T3D_NEW NullGL4VertexDeclaration(); }
    NullGL4VertexDeclaration::~NullGL4VertexDeclaration() {}
    void *NullGL4VertexDeclaration::getNativeObject() const { return nullptr; }

    NullGL4VertexBufferPtr NullGL4VertexBuffer::create() { return T3D_NEW NullGL4VertexBuffer(); }
    NullGL4VertexBuffer::~NullGL4VertexBuffer() {}
    void *NullGL4VertexBuffer::getNativeObject() const { return nullptr; }

    NullGL4IndexBufferPtr NullGL4IndexBuffer::create() { return T3D_NEW NullGL4IndexBuffer(); }
    NullGL4IndexBuffer::~NullGL4IndexBuffer() {}
    void *NullGL4IndexBuffer::getNativeObject() const { return nullptr; }

    NullGL4ConstantBufferPtr NullGL4ConstantBuffer::create() { return T3D_NEW NullGL4ConstantBuffer(); }
    NullGL4ConstantBuffer::~NullGL4ConstantBuffer() {}
    void *NullGL4ConstantBuffer::getNativeObject() const { return nullptr; }

    NullGL4PixelBuffer1DPtr NullGL4PixelBuffer1D::create() { return T3D_NEW NullGL4PixelBuffer1D(); }
    NullGL4PixelBuffer1D::~NullGL4PixelBuffer1D() {}
    void *NullGL4PixelBuffer1D::getNativeObject() const { return nullptr; }

    NullGL4PixelBuffer2DPtr NullGL4PixelBuffer2D::create() { return T3D_NEW NullGL4PixelBuffer2D(); }
    NullGL4PixelBuffer2D::~NullGL4PixelBuffer2D() {}
    void *NullGL4PixelBuffer2D::getNativeObject() const { return nullptr; }

    void *NullGL4PixelBuffer3D::getNativeObject() const { return nullptr; }
}
