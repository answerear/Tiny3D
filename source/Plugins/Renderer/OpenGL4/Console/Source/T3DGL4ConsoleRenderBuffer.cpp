/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DGL4ConsoleRenderBuffer.h"


namespace Tiny3D
{
    GL4ConsoleVertexDeclarationPtr GL4ConsoleVertexDeclaration::create() { return T3D_NEW GL4ConsoleVertexDeclaration(); }
    GL4ConsoleVertexDeclaration::~GL4ConsoleVertexDeclaration() {}
    void *GL4ConsoleVertexDeclaration::getNativeObject() const { return nullptr; }

    GL4ConsoleVertexBufferPtr GL4ConsoleVertexBuffer::create() { return T3D_NEW GL4ConsoleVertexBuffer(); }
    GL4ConsoleVertexBuffer::~GL4ConsoleVertexBuffer() {}
    void *GL4ConsoleVertexBuffer::getNativeObject() const { return nullptr; }

    GL4ConsoleIndexBufferPtr GL4ConsoleIndexBuffer::create() { return T3D_NEW GL4ConsoleIndexBuffer(); }
    GL4ConsoleIndexBuffer::~GL4ConsoleIndexBuffer() {}
    void *GL4ConsoleIndexBuffer::getNativeObject() const { return nullptr; }

    GL4ConsoleConstantBufferPtr GL4ConsoleConstantBuffer::create() { return T3D_NEW GL4ConsoleConstantBuffer(); }
    GL4ConsoleConstantBuffer::~GL4ConsoleConstantBuffer() {}
    void *GL4ConsoleConstantBuffer::getNativeObject() const { return nullptr; }

    GL4ConsolePixelBuffer1DPtr GL4ConsolePixelBuffer1D::create() { return T3D_NEW GL4ConsolePixelBuffer1D(); }
    GL4ConsolePixelBuffer1D::~GL4ConsolePixelBuffer1D() {}
    void *GL4ConsolePixelBuffer1D::getNativeObject() const { return nullptr; }

    GL4ConsolePixelBuffer2DPtr GL4ConsolePixelBuffer2D::create() { return T3D_NEW GL4ConsolePixelBuffer2D(); }
    GL4ConsolePixelBuffer2D::~GL4ConsolePixelBuffer2D() {}
    void *GL4ConsolePixelBuffer2D::getNativeObject() const { return nullptr; }

    void *GL4ConsolePixelBuffer3D::getNativeObject() const { return nullptr; }
}
