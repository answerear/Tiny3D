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


#include "T3DVKShader.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    VKVertexShaderPtr VKVertexShader::create()
    {
        return T3D_NEW VKVertexShader();
    }

    //--------------------------------------------------------------------------

    VKVertexShader::~VKVertexShader()
    {
    }

    //--------------------------------------------------------------------------

    void *VKVertexShader::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VKHullShaderPtr VKHullShader::create()
    {
        return T3D_NEW VKHullShader();
    }

    //--------------------------------------------------------------------------

    VKHullShader::~VKHullShader()
    {
    }

    //--------------------------------------------------------------------------

    void *VKHullShader::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VKDomainShaderPtr VKDomainShader::create()
    {
        return T3D_NEW VKDomainShader();
    }

    //--------------------------------------------------------------------------

    VKDomainShader::~VKDomainShader()
    {
    }

    //--------------------------------------------------------------------------

    void *VKDomainShader::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VKGeometryShaderPtr VKGeometryShader::create()
    {
        return T3D_NEW VKGeometryShader();
    }

    //--------------------------------------------------------------------------

    VKGeometryShader::~VKGeometryShader()
    {
    }

    //--------------------------------------------------------------------------

    void *VKGeometryShader::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VKPixelShaderPtr VKPixelShader::create()
    {
        return T3D_NEW VKPixelShader();
    }

    //--------------------------------------------------------------------------

    VKPixelShader::~VKPixelShader()
    {
    }

    //--------------------------------------------------------------------------

    void *VKPixelShader::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VKComputeShaderPtr VKComputeShader::create()
    {
        return T3D_NEW VKComputeShader();
    }

    //--------------------------------------------------------------------------

    VKComputeShader::~VKComputeShader()
    {
    }

    //--------------------------------------------------------------------------

    void *VKComputeShader::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
}
