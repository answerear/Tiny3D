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


#include "T3DVKConsoleShader.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    VKConsoleVertexShaderPtr VKConsoleVertexShader::create()
    {
        return T3D_NEW VKConsoleVertexShader();
    }

    //--------------------------------------------------------------------------

    VKConsoleVertexShader::~VKConsoleVertexShader()
    {
    }

    //--------------------------------------------------------------------------

    void *VKConsoleVertexShader::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VKConsoleHullShaderPtr VKConsoleHullShader::create()
    {
        return T3D_NEW VKConsoleHullShader();
    }

    //--------------------------------------------------------------------------

    VKConsoleHullShader::~VKConsoleHullShader()
    {
    }

    //--------------------------------------------------------------------------

    void *VKConsoleHullShader::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VKConsoleDomainShaderPtr VKConsoleDomainShader::create()
    {
        return T3D_NEW VKConsoleDomainShader();
    }

    //--------------------------------------------------------------------------

    VKConsoleDomainShader::~VKConsoleDomainShader()
    {
    }

    //--------------------------------------------------------------------------

    void *VKConsoleDomainShader::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VKConsoleGeometryShaderPtr VKConsoleGeometryShader::create()
    {
        return T3D_NEW VKConsoleGeometryShader();
    }

    //--------------------------------------------------------------------------

    VKConsoleGeometryShader::~VKConsoleGeometryShader()
    {
    }

    //--------------------------------------------------------------------------

    void *VKConsoleGeometryShader::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VKConsolePixelShaderPtr VKConsolePixelShader::create()
    {
        return T3D_NEW VKConsolePixelShader();
    }

    //--------------------------------------------------------------------------

    VKConsolePixelShader::~VKConsolePixelShader()
    {
    }

    //--------------------------------------------------------------------------

    void *VKConsolePixelShader::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VKConsoleComputeShaderPtr VKConsoleComputeShader::create()
    {
        return T3D_NEW VKConsoleComputeShader();
    }

    //--------------------------------------------------------------------------

    VKConsoleComputeShader::~VKConsoleComputeShader()
    {
    }

    //--------------------------------------------------------------------------

    void *VKConsoleComputeShader::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
}
