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


#include "T3DD3D11ConsoleRenderBuffer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11ConsoleVertexDeclarationPtr D3D11ConsoleVertexDeclaration::create()
    {
        return T3D_NEW D3D11ConsoleVertexDeclaration();
    }
    
    //--------------------------------------------------------------------------

    D3D11ConsoleVertexDeclaration::~D3D11ConsoleVertexDeclaration()
    {
        
    }

    //--------------------------------------------------------------------------

    void *D3D11ConsoleVertexDeclaration::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    D3D11ConsoleVertexBufferPtr D3D11ConsoleVertexBuffer::create()
    {
        return T3D_NEW D3D11ConsoleVertexBuffer();
    }
    //--------------------------------------------------------------------------

    D3D11ConsoleVertexBuffer::~D3D11ConsoleVertexBuffer()
    {
        
    }

    //--------------------------------------------------------------------------

    void *D3D11ConsoleVertexBuffer::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    D3D11ConsoleIndexBufferPtr D3D11ConsoleIndexBuffer::create()
    {
        return T3D_NEW D3D11ConsoleIndexBuffer();
    }

    //--------------------------------------------------------------------------

    D3D11ConsoleIndexBuffer::~D3D11ConsoleIndexBuffer()
    {
        
    }

    //--------------------------------------------------------------------------

    void *D3D11ConsoleIndexBuffer::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------
    
    D3D11ConsoleConstantBufferPtr D3D11ConsoleConstantBuffer::create()
    {
        return T3D_NEW D3D11ConsoleConstantBuffer();
    }

    //--------------------------------------------------------------------------

    D3D11ConsoleConstantBuffer::~D3D11ConsoleConstantBuffer()
    {
        
    }
    
    //--------------------------------------------------------------------------

    void *D3D11ConsoleConstantBuffer::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    D3D11ConsolePixelBuffer1DPtr D3D11ConsolePixelBuffer1D::create()
    {
        return T3D_NEW D3D11ConsolePixelBuffer1D();
    }
    
    //--------------------------------------------------------------------------

    D3D11ConsolePixelBuffer1D::~D3D11ConsolePixelBuffer1D()
    {
        
    }

    //--------------------------------------------------------------------------

    void *D3D11ConsolePixelBuffer1D::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    D3D11ConsolePixelBuffer2DPtr D3D11ConsolePixelBuffer2D::create()
    {
        return T3D_NEW D3D11ConsolePixelBuffer2D();
    }

    //--------------------------------------------------------------------------

    D3D11ConsolePixelBuffer2D::~D3D11ConsolePixelBuffer2D()
    {
    }

    //--------------------------------------------------------------------------

    void *D3D11ConsolePixelBuffer2D::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------
    
    void *D3D11ConsolePixelBuffer3D::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------
}


