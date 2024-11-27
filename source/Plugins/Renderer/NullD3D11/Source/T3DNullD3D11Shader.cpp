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


#include "T3DNullD3D11Shader.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    NullD3D11VertexShaderPtr NullD3D11VertexShader::create()
    {
        return new NullD3D11VertexShader();
    }

    //--------------------------------------------------------------------------

    NullD3D11VertexShader::~NullD3D11VertexShader()
    {
        
    }

    //--------------------------------------------------------------------------

    void *NullD3D11VertexShader::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    NullD3D11HullShaderPtr NullD3D11HullShader::create()
    {
        return new NullD3D11HullShader();
    }

    //--------------------------------------------------------------------------

    NullD3D11HullShader::~NullD3D11HullShader()
    {
        
    }
    
    //--------------------------------------------------------------------------

    void *NullD3D11HullShader::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    NullD3D11DomainShaderPtr NullD3D11DomainShader::create()
    {
        return new NullD3D11DomainShader();
    }

    //--------------------------------------------------------------------------

    NullD3D11DomainShader::~NullD3D11DomainShader()
    {
    }
    
    //--------------------------------------------------------------------------

    void *NullD3D11DomainShader::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    NullD3D11GeometryShaderPtr NullD3D11GeometryShader::create()
    {
        return new NullD3D11GeometryShader();
    }

    //--------------------------------------------------------------------------

    NullD3D11GeometryShader::~NullD3D11GeometryShader()
    {
    }
    
    //--------------------------------------------------------------------------

    void *NullD3D11GeometryShader::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    NullD3D11PixelShaderPtr NullD3D11PixelShader::create()
    {
        return new NullD3D11PixelShader();
    }

    //--------------------------------------------------------------------------

    NullD3D11PixelShader::~NullD3D11PixelShader()
    {
    }
    
    //--------------------------------------------------------------------------

    void *NullD3D11PixelShader::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    NullD3D11ComputeShaderPtr NullD3D11ComputeShader::create()
    {
        return new NullD3D11ComputeShader();
    }

    //--------------------------------------------------------------------------

    NullD3D11ComputeShader::~NullD3D11ComputeShader()
    {
    }
    
    //--------------------------------------------------------------------------

    void *NullD3D11ComputeShader::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------
}


