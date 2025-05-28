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


#include "T3DD3D11RenderBuffer.h"
#include "T3DD3D11Context.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11VertexDeclarationPtr D3D11VertexDeclaration::create()
    {
        return new D3D11VertexDeclaration();
    }
    
    //--------------------------------------------------------------------------

    D3D11VertexDeclaration::~D3D11VertexDeclaration()
    {
        D3D_SAFE_RELEASE(D3D11InputLayout);
    }

    //--------------------------------------------------------------------------

    void *D3D11VertexDeclaration::getNativeObject() const
    {
        return D3D11InputLayout;
    }
    
    //--------------------------------------------------------------------------

    D3D11VertexBufferPtr D3D11VertexBuffer::create()
    {
        return new D3D11VertexBuffer();
    }
    //--------------------------------------------------------------------------

    D3D11VertexBuffer::~D3D11VertexBuffer()
    {
        D3D_SAFE_RELEASE(D3DBuffer);
    }

    //--------------------------------------------------------------------------

    void *D3D11VertexBuffer::getNativeObject() const
    {
        return D3DBuffer;
    }
    
    //--------------------------------------------------------------------------

    D3D11IndexBufferPtr D3D11IndexBuffer::create()
    {
        return new D3D11IndexBuffer();
    }

    //--------------------------------------------------------------------------

    D3D11IndexBuffer::~D3D11IndexBuffer()
    {
        D3D_SAFE_RELEASE(D3DBuffer);
    }

    //--------------------------------------------------------------------------

    void *D3D11IndexBuffer::getNativeObject() const
    {
        return D3DBuffer;
    }
    
    //--------------------------------------------------------------------------
    
    D3D11ConstantBufferPtr D3D11ConstantBuffer::create()
    {
        return new D3D11ConstantBuffer();
    }

    //--------------------------------------------------------------------------

    D3D11ConstantBuffer::~D3D11ConstantBuffer()
    {
        D3D_SAFE_RELEASE(D3DBuffer);
    }
    
    //--------------------------------------------------------------------------

    void *D3D11ConstantBuffer::getNativeObject() const
    {
        return D3DBuffer;
    }
    
    //--------------------------------------------------------------------------

    D3D11PixelBuffer1DPtr D3D11PixelBuffer1D::create()
    {
        return new D3D11PixelBuffer1D();
    }
    
    //--------------------------------------------------------------------------

    D3D11PixelBuffer1D::~D3D11PixelBuffer1D()
    {
        
    }

    //--------------------------------------------------------------------------

    void *D3D11PixelBuffer1D::getNativeObject() const
    {
        return D3DSRView;
    }
    
    //--------------------------------------------------------------------------

    D3D11PixelBuffer2DPtr D3D11PixelBuffer2D::create()
    {
        return new D3D11PixelBuffer2D();
    }

    //--------------------------------------------------------------------------

    D3D11PixelBuffer2D::~D3D11PixelBuffer2D()
    {
        D3D_SAFE_RELEASE(D3DTexture);
        D3D_SAFE_RELEASE(D3DSRView);
        D3D_SAFE_RELEASE(D3DRTView);
        D3D_SAFE_RELEASE(D3DDSView);
        D3D_SAFE_RELEASE(D3DResolveTex);
    }

    //--------------------------------------------------------------------------

    void *D3D11PixelBuffer2D::getNativeObject() const
    {
        return D3DSRView;
    }
    
    //--------------------------------------------------------------------------
    
    void *D3D11PixelBuffer3D::getNativeObject() const
    {
        return D3DSRView;
    }
    
    //--------------------------------------------------------------------------
}


