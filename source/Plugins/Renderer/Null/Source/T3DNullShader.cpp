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


#include "T3DNullShader.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    NullVertexShaderPtr NullVertexShader::create()
    {
        return T3D_NEW NullVertexShader();
    }

    //--------------------------------------------------------------------------

    NullVertexShader::~NullVertexShader()
    {
        
    }

    //--------------------------------------------------------------------------

    void *NullVertexShader::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    NullHullShaderPtr NullHullShader::create()
    {
        return T3D_NEW NullHullShader();
    }

    //--------------------------------------------------------------------------

    NullHullShader::~NullHullShader()
    {
        
    }
    
    //--------------------------------------------------------------------------

    void *NullHullShader::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    NullDomainShaderPtr NullDomainShader::create()
    {
        return T3D_NEW NullDomainShader();
    }

    //--------------------------------------------------------------------------

    NullDomainShader::~NullDomainShader()
    {
    }
    
    //--------------------------------------------------------------------------

    void *NullDomainShader::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    NullGeometryShaderPtr NullGeometryShader::create()
    {
        return T3D_NEW NullGeometryShader();
    }

    //--------------------------------------------------------------------------

    NullGeometryShader::~NullGeometryShader()
    {
    }
    
    //--------------------------------------------------------------------------

    void *NullGeometryShader::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    NullPixelShaderPtr NullPixelShader::create()
    {
        return T3D_NEW NullPixelShader();
    }

    //--------------------------------------------------------------------------

    NullPixelShader::~NullPixelShader()
    {
    }
    
    //--------------------------------------------------------------------------

    void *NullPixelShader::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    NullComputeShaderPtr NullComputeShader::create()
    {
        return T3D_NEW NullComputeShader();
    }

    //--------------------------------------------------------------------------

    NullComputeShader::~NullComputeShader()
    {
    }
    
    //--------------------------------------------------------------------------

    void *NullComputeShader::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------
}


