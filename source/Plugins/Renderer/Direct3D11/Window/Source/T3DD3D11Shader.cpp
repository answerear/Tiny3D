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


#include "T3DD3D11Shader.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11VertexShaderPtr D3D11VertexShader::create()
    {
        return T3D_NEW D3D11VertexShader();
    }

    //--------------------------------------------------------------------------

    D3D11VertexShader::~D3D11VertexShader()
    {
        D3D_SAFE_RELEASE(D3DShader);
    }

    //--------------------------------------------------------------------------

    void *D3D11VertexShader::getNativeObject() const
    {
        return D3DShader;
    }

    //--------------------------------------------------------------------------

    D3D11HullShaderPtr D3D11HullShader::create()
    {
        return T3D_NEW D3D11HullShader();
    }

    //--------------------------------------------------------------------------

    D3D11HullShader::~D3D11HullShader()
    {
        D3D_SAFE_RELEASE(D3DShader);
    }
    
    //--------------------------------------------------------------------------

    void *D3D11HullShader::getNativeObject() const
    {
        return D3DShader;
    }
    
    //--------------------------------------------------------------------------

    D3D11DomainShaderPtr D3D11DomainShader::create()
    {
        return T3D_NEW D3D11DomainShader();
    }

    //--------------------------------------------------------------------------

    D3D11DomainShader::~D3D11DomainShader()
    {
        D3D_SAFE_RELEASE(D3DShader);
    }
    
    //--------------------------------------------------------------------------

    void *D3D11DomainShader::getNativeObject() const
    {
        return D3DShader;
    }
    
    //--------------------------------------------------------------------------

    D3D11GeometryShaderPtr D3D11GeometryShader::create()
    {
        return T3D_NEW D3D11GeometryShader();
    }

    //--------------------------------------------------------------------------

    D3D11GeometryShader::~D3D11GeometryShader()
    {
        D3D_SAFE_RELEASE(D3DShader);
    }
    
    //--------------------------------------------------------------------------

    void *D3D11GeometryShader::getNativeObject() const
    {
        return D3DShader;
    }
    
    //--------------------------------------------------------------------------

    D3D11PixelShaderPtr D3D11PixelShader::create()
    {
        return T3D_NEW D3D11PixelShader();
    }

    //--------------------------------------------------------------------------

    D3D11PixelShader::~D3D11PixelShader()
    {
        D3D_SAFE_RELEASE(D3DShader);
    }
    
    //--------------------------------------------------------------------------

    void *D3D11PixelShader::getNativeObject() const
    {
        return D3DShader;
    }
    
    //--------------------------------------------------------------------------

    D3D11ComputeShaderPtr D3D11ComputeShader::create()
    {
        return T3D_NEW D3D11ComputeShader();
    }

    //--------------------------------------------------------------------------

    D3D11ComputeShader::~D3D11ComputeShader()
    {
        D3D_SAFE_RELEASE(D3DShader);
    }
    
    //--------------------------------------------------------------------------

    void *D3D11ComputeShader::getNativeObject() const
    {
        return D3DShader;
    }
    
    //--------------------------------------------------------------------------
}


