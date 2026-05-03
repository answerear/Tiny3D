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


#ifndef __T3D_NULLD3D11_SHADER_H__
#define __T3D_NULLD3D11_SHADER_H__


#include "T3DNullD3D11Prerequisites.h"


namespace Tiny3D
{
    class NullD3D11Shader : public RHIShader
    {
    protected:
        NullD3D11Shader() = default;
    };

    class NullD3D11VertexShader : public NullD3D11Shader
    {
    public:
        static NullD3D11VertexShaderPtr create();

        ~NullD3D11VertexShader() override;

        void *getNativeObject() const override;
        
    protected:
        NullD3D11VertexShader() = default;
    };

    class NullD3D11HullShader : public NullD3D11Shader
    {
    public:
        static NullD3D11HullShaderPtr create();

        ~NullD3D11HullShader() override;

        void *getNativeObject() const override;
        
    protected:
        NullD3D11HullShader() = default;
    };

    class NullD3D11DomainShader : public NullD3D11Shader
    {
    public:
        static NullD3D11DomainShaderPtr create();

        ~NullD3D11DomainShader() override;

        void *getNativeObject() const override;
        
    protected:
        NullD3D11DomainShader() = default;
    };

    class NullD3D11GeometryShader : public NullD3D11Shader
    {
    public:
        static NullD3D11GeometryShaderPtr create();

        ~NullD3D11GeometryShader() override;

        void *getNativeObject() const override;
        
    protected:
        NullD3D11GeometryShader() = default;
    };

    class NullD3D11PixelShader : public NullD3D11Shader
    {
    public:
        static NullD3D11PixelShaderPtr create();

        ~NullD3D11PixelShader() override;

        void *getNativeObject() const override;
        
    protected:
        NullD3D11PixelShader() = default;
    };

    class NullD3D11ComputeShader : public NullD3D11Shader
    {
    public:
        static NullD3D11ComputeShaderPtr create();

        ~NullD3D11ComputeShader() override;

        void *getNativeObject() const override;
        
    protected:
        NullD3D11ComputeShader() = default;
    };
}


#endif  /*__T3D_NULLD3D11_SHADER_H__*/
