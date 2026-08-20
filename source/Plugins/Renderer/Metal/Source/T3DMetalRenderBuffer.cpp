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


#include "T3DMetalRenderBuffer.h"
#include "T3DMetalNative.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    MetalVertexDeclarationPtr MetalVertexDeclaration::create()
    {
        return T3D_NEW MetalVertexDeclaration();
    }

    MetalVertexDeclaration::~MetalVertexDeclaration()
    {
    }

    void *MetalVertexDeclaration::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    MetalVertexBufferPtr MetalVertexBuffer::create()
    {
        return T3D_NEW MetalVertexBuffer();
    }

    MetalVertexBuffer::~MetalVertexBuffer()
    {
        MetalRelease(mNative);
    }

    void *MetalVertexBuffer::getNativeObject() const
    {
        return mNative;
    }

    void MetalVertexBuffer::setNativeObject(void *buffer)
    {
        MetalAssign(mNative, buffer);
    }

    //--------------------------------------------------------------------------

    MetalIndexBufferPtr MetalIndexBuffer::create()
    {
        return T3D_NEW MetalIndexBuffer();
    }

    MetalIndexBuffer::~MetalIndexBuffer()
    {
        MetalRelease(mNative);
    }

    void *MetalIndexBuffer::getNativeObject() const
    {
        return mNative;
    }

    void MetalIndexBuffer::setNativeObject(void *buffer)
    {
        MetalAssign(mNative, buffer);
    }

    //--------------------------------------------------------------------------

    MetalConstantBufferPtr MetalConstantBuffer::create()
    {
        return T3D_NEW MetalConstantBuffer();
    }

    MetalConstantBuffer::~MetalConstantBuffer()
    {
        MetalRelease(mNative);
    }

    void *MetalConstantBuffer::getNativeObject() const
    {
        return mNative;
    }

    void MetalConstantBuffer::setNativeObject(void *buffer)
    {
        MetalAssign(mNative, buffer);
    }

    //--------------------------------------------------------------------------

    MetalPixelBuffer1DPtr MetalPixelBuffer1D::create()
    {
        return T3D_NEW MetalPixelBuffer1D();
    }

    MetalPixelBuffer1D::~MetalPixelBuffer1D()
    {
        MetalRelease(mNative);
    }

    void *MetalPixelBuffer1D::getNativeObject() const
    {
        return mNative;
    }

    void MetalPixelBuffer1D::setNativeObject(void *texture)
    {
        MetalAssign(mNative, texture);
    }

    //--------------------------------------------------------------------------

    MetalPixelBuffer2DPtr MetalPixelBuffer2D::create()
    {
        return T3D_NEW MetalPixelBuffer2D();
    }

    MetalPixelBuffer2D::~MetalPixelBuffer2D()
    {
        MetalRelease(mResolveNative);
        MetalRelease(mNative);
    }

    void *MetalPixelBuffer2D::getNativeObject() const
    {
        return mNative;
    }

    void MetalPixelBuffer2D::setNativeObject(void *texture)
    {
        MetalAssign(mNative, texture);
    }

    void MetalPixelBuffer2D::setResolveNative(void *texture)
    {
        MetalAssign(mResolveNative, texture);
    }

    //--------------------------------------------------------------------------

    MetalPixelBuffer3DPtr MetalPixelBuffer3D::create()
    {
        return T3D_NEW MetalPixelBuffer3D();
    }

    MetalPixelBuffer3D::~MetalPixelBuffer3D()
    {
        MetalRelease(mNative);
    }

    void *MetalPixelBuffer3D::getNativeObject() const
    {
        return mNative;
    }

    void MetalPixelBuffer3D::setNativeObject(void *texture)
    {
        MetalAssign(mNative, texture);
    }

    //--------------------------------------------------------------------------

    MetalPixelBufferCubemapPtr MetalPixelBufferCubemap::create()
    {
        return T3D_NEW MetalPixelBufferCubemap();
    }

    MetalPixelBufferCubemap::~MetalPixelBufferCubemap()
    {
        MetalRelease(mNative);
    }

    void *MetalPixelBufferCubemap::getNativeObject() const
    {
        return mNative;
    }

    void MetalPixelBufferCubemap::setNativeObject(void *texture)
    {
        MetalAssign(mNative, texture);
    }

    //--------------------------------------------------------------------------
}
