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


#include "T3DD3D11ConsoleShader.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11ConsoleVertexShaderPtr D3D11ConsoleVertexShader::create()
    {
        return T3D_NEW D3D11ConsoleVertexShader();
    }

    //--------------------------------------------------------------------------

    D3D11ConsoleVertexShader::~D3D11ConsoleVertexShader()
    {
        
    }

    //--------------------------------------------------------------------------

    void *D3D11ConsoleVertexShader::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    D3D11ConsoleHullShaderPtr D3D11ConsoleHullShader::create()
    {
        return T3D_NEW D3D11ConsoleHullShader();
    }

    //--------------------------------------------------------------------------

    D3D11ConsoleHullShader::~D3D11ConsoleHullShader()
    {
        
    }
    
    //--------------------------------------------------------------------------

    void *D3D11ConsoleHullShader::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    D3D11ConsoleDomainShaderPtr D3D11ConsoleDomainShader::create()
    {
        return T3D_NEW D3D11ConsoleDomainShader();
    }

    //--------------------------------------------------------------------------

    D3D11ConsoleDomainShader::~D3D11ConsoleDomainShader()
    {
    }
    
    //--------------------------------------------------------------------------

    void *D3D11ConsoleDomainShader::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    D3D11ConsoleGeometryShaderPtr D3D11ConsoleGeometryShader::create()
    {
        return T3D_NEW D3D11ConsoleGeometryShader();
    }

    //--------------------------------------------------------------------------

    D3D11ConsoleGeometryShader::~D3D11ConsoleGeometryShader()
    {
    }
    
    //--------------------------------------------------------------------------

    void *D3D11ConsoleGeometryShader::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    D3D11ConsolePixelShaderPtr D3D11ConsolePixelShader::create()
    {
        return T3D_NEW D3D11ConsolePixelShader();
    }

    //--------------------------------------------------------------------------

    D3D11ConsolePixelShader::~D3D11ConsolePixelShader()
    {
    }
    
    //--------------------------------------------------------------------------

    void *D3D11ConsolePixelShader::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    D3D11ConsoleComputeShaderPtr D3D11ConsoleComputeShader::create()
    {
        return T3D_NEW D3D11ConsoleComputeShader();
    }

    //--------------------------------------------------------------------------

    D3D11ConsoleComputeShader::~D3D11ConsoleComputeShader()
    {
    }
    
    //--------------------------------------------------------------------------

    void *D3D11ConsoleComputeShader::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------
}


