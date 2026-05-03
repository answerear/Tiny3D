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


#include "T3DNullD3D11RenderBuffer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    NullD3D11VertexDeclarationPtr NullD3D11VertexDeclaration::create()
    {
        return T3D_NEW NullD3D11VertexDeclaration();
    }
    
    //--------------------------------------------------------------------------

    NullD3D11VertexDeclaration::~NullD3D11VertexDeclaration()
    {
        
    }

    //--------------------------------------------------------------------------

    void *NullD3D11VertexDeclaration::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    NullD3D11VertexBufferPtr NullD3D11VertexBuffer::create()
    {
        return T3D_NEW NullD3D11VertexBuffer();
    }
    //--------------------------------------------------------------------------

    NullD3D11VertexBuffer::~NullD3D11VertexBuffer()
    {
        
    }

    //--------------------------------------------------------------------------

    void *NullD3D11VertexBuffer::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    NullD3D11IndexBufferPtr NullD3D11IndexBuffer::create()
    {
        return T3D_NEW NullD3D11IndexBuffer();
    }

    //--------------------------------------------------------------------------

    NullD3D11IndexBuffer::~NullD3D11IndexBuffer()
    {
        
    }

    //--------------------------------------------------------------------------

    void *NullD3D11IndexBuffer::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------
    
    NullD3D11ConstantBufferPtr NullD3D11ConstantBuffer::create()
    {
        return T3D_NEW NullD3D11ConstantBuffer();
    }

    //--------------------------------------------------------------------------

    NullD3D11ConstantBuffer::~NullD3D11ConstantBuffer()
    {
        
    }
    
    //--------------------------------------------------------------------------

    void *NullD3D11ConstantBuffer::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    NullD3D11PixelBuffer1DPtr NullD3D11PixelBuffer1D::create()
    {
        return T3D_NEW NullD3D11PixelBuffer1D();
    }
    
    //--------------------------------------------------------------------------

    NullD3D11PixelBuffer1D::~NullD3D11PixelBuffer1D()
    {
        
    }

    //--------------------------------------------------------------------------

    void *NullD3D11PixelBuffer1D::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    NullD3D11PixelBuffer2DPtr NullD3D11PixelBuffer2D::create()
    {
        return T3D_NEW NullD3D11PixelBuffer2D();
    }

    //--------------------------------------------------------------------------

    NullD3D11PixelBuffer2D::~NullD3D11PixelBuffer2D()
    {
    }

    //--------------------------------------------------------------------------

    void *NullD3D11PixelBuffer2D::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------
    
    void *NullD3D11PixelBuffer3D::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------
}


