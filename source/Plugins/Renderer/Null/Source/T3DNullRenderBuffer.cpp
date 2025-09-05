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


#include "T3DNullRenderBuffer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    NullVertexDeclarationPtr NullVertexDeclaration::create()
    {
        return T3D_NEW NullVertexDeclaration();
    }
    
    //--------------------------------------------------------------------------

    NullVertexDeclaration::~NullVertexDeclaration()
    {
        
    }

    //--------------------------------------------------------------------------

    void *NullVertexDeclaration::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    NullVertexBufferPtr NullVertexBuffer::create()
    {
        return T3D_NEW NullVertexBuffer();
    }
    //--------------------------------------------------------------------------

    NullVertexBuffer::~NullVertexBuffer()
    {
        
    }

    //--------------------------------------------------------------------------

    void *NullVertexBuffer::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    NullIndexBufferPtr NullIndexBuffer::create()
    {
        return T3D_NEW NullIndexBuffer();
    }

    //--------------------------------------------------------------------------

    NullIndexBuffer::~NullIndexBuffer()
    {
        
    }

    //--------------------------------------------------------------------------

    void *NullIndexBuffer::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------
    
    NullConstantBufferPtr NullConstantBuffer::create()
    {
        return T3D_NEW NullConstantBuffer();
    }

    //--------------------------------------------------------------------------

    NullConstantBuffer::~NullConstantBuffer()
    {
        
    }
    
    //--------------------------------------------------------------------------

    void *NullConstantBuffer::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    NullPixelBuffer1DPtr NullPixelBuffer1D::create()
    {
        return T3D_NEW NullPixelBuffer1D();
    }
    
    //--------------------------------------------------------------------------

    NullPixelBuffer1D::~NullPixelBuffer1D()
    {
        
    }

    //--------------------------------------------------------------------------

    void *NullPixelBuffer1D::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    NullPixelBuffer2DPtr NullPixelBuffer2D::create()
    {
        return T3D_NEW NullPixelBuffer2D();
    }

    //--------------------------------------------------------------------------

    NullPixelBuffer2D::~NullPixelBuffer2D()
    {
    }

    //--------------------------------------------------------------------------

    void *NullPixelBuffer2D::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------
    
    void *NullPixelBuffer3D::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------
}


