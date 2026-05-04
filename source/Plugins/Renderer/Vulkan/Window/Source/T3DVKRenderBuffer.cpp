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


#include "T3DVKRenderBuffer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    VKVertexDeclarationPtr VKVertexDeclaration::create()
    {
        return T3D_NEW VKVertexDeclaration();
    }

    //--------------------------------------------------------------------------

    VKVertexDeclaration::~VKVertexDeclaration()
    {
    }

    //--------------------------------------------------------------------------

    void *VKVertexDeclaration::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VKVertexBufferPtr VKVertexBuffer::create()
    {
        return T3D_NEW VKVertexBuffer();
    }

    //--------------------------------------------------------------------------

    VKVertexBuffer::~VKVertexBuffer()
    {
    }

    //--------------------------------------------------------------------------

    void *VKVertexBuffer::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VKIndexBufferPtr VKIndexBuffer::create()
    {
        return T3D_NEW VKIndexBuffer();
    }

    //--------------------------------------------------------------------------

    VKIndexBuffer::~VKIndexBuffer()
    {
    }

    //--------------------------------------------------------------------------

    void *VKIndexBuffer::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VKConstantBufferPtr VKConstantBuffer::create()
    {
        return T3D_NEW VKConstantBuffer();
    }

    //--------------------------------------------------------------------------

    VKConstantBuffer::~VKConstantBuffer()
    {
    }

    //--------------------------------------------------------------------------

    void *VKConstantBuffer::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VKPixelBuffer1DPtr VKPixelBuffer1D::create()
    {
        return T3D_NEW VKPixelBuffer1D();
    }

    //--------------------------------------------------------------------------

    VKPixelBuffer1D::~VKPixelBuffer1D()
    {
    }

    //--------------------------------------------------------------------------

    void *VKPixelBuffer1D::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VKPixelBuffer2DPtr VKPixelBuffer2D::create()
    {
        return T3D_NEW VKPixelBuffer2D();
    }

    //--------------------------------------------------------------------------

    VKPixelBuffer2D::~VKPixelBuffer2D()
    {
    }

    //--------------------------------------------------------------------------

    void *VKPixelBuffer2D::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    void *VKPixelBuffer3D::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
}
