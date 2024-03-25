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

#pragma once


#include "T3DD3D11Prerequisites.h"


namespace Tiny3D
{
    class D3D11Shader : public RHIShader
    {
    protected:
        D3D11Shader() = default;
    };

    class D3D11VertexShader : public D3D11Shader
    {
    public:
        static D3D11VertexShaderPtr create();

        ~D3D11VertexShader() override;

        ID3D11VertexShader  *D3DShader {nullptr};
    protected:
        D3D11VertexShader() = default;
    };

    class D3D11HullShader : public D3D11Shader
    {
    public:
        static D3D11HullShaderPtr create();

        ~D3D11HullShader() override;

        ID3D11HullShader    *D3DShader {nullptr};
    protected:
        D3D11HullShader() = default;
    };

    class D3D11DomainShader : public D3D11Shader
    {
    public:
        static D3D11DomainShaderPtr create();

        ~D3D11DomainShader() override;

        ID3D11DomainShader  *D3DShader {nullptr};
    protected:
        D3D11DomainShader() = default;
    };

    class D3D11GeometryShader : public D3D11Shader
    {
    public:
        static D3D11GeometryShaderPtr create();

        ~D3D11GeometryShader() override;

        ID3D11GeometryShader    *D3DShader {nullptr};
    protected:
        D3D11GeometryShader() = default;
    };

    class D3D11PixelShader : public D3D11Shader
    {
    public:
        static D3D11PixelShaderPtr create();

        ~D3D11PixelShader() override;

        ID3D11PixelShader   *D3DShader {nullptr};
    protected:
        D3D11PixelShader() = default;
    };

    class D3D11ComputeShader : public D3D11Shader
    {
    public:
        static D3D11ComputeShaderPtr create();

        ~D3D11ComputeShader() override;

        ID3D11ComputeShader *D3DShader {nullptr};
    protected:
        D3D11ComputeShader() = default;
    };
}
