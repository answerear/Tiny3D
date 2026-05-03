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


#ifndef __T3D_D3D11CONSOLE_SHADER_H__
#define __T3D_D3D11CONSOLE_SHADER_H__


#include "T3DD3D11ConsolePrerequisites.h"


namespace Tiny3D
{
    class D3D11ConsoleShader : public RHIShader
    {
    protected:
        D3D11ConsoleShader() = default;
    };

    class D3D11ConsoleVertexShader : public D3D11ConsoleShader
    {
    public:
        static D3D11ConsoleVertexShaderPtr create();

        ~D3D11ConsoleVertexShader() override;

        void *getNativeObject() const override;
        
    protected:
        D3D11ConsoleVertexShader() = default;
    };

    class D3D11ConsoleHullShader : public D3D11ConsoleShader
    {
    public:
        static D3D11ConsoleHullShaderPtr create();

        ~D3D11ConsoleHullShader() override;

        void *getNativeObject() const override;
        
    protected:
        D3D11ConsoleHullShader() = default;
    };

    class D3D11ConsoleDomainShader : public D3D11ConsoleShader
    {
    public:
        static D3D11ConsoleDomainShaderPtr create();

        ~D3D11ConsoleDomainShader() override;

        void *getNativeObject() const override;
        
    protected:
        D3D11ConsoleDomainShader() = default;
    };

    class D3D11ConsoleGeometryShader : public D3D11ConsoleShader
    {
    public:
        static D3D11ConsoleGeometryShaderPtr create();

        ~D3D11ConsoleGeometryShader() override;

        void *getNativeObject() const override;
        
    protected:
        D3D11ConsoleGeometryShader() = default;
    };

    class D3D11ConsolePixelShader : public D3D11ConsoleShader
    {
    public:
        static D3D11ConsolePixelShaderPtr create();

        ~D3D11ConsolePixelShader() override;

        void *getNativeObject() const override;
        
    protected:
        D3D11ConsolePixelShader() = default;
    };

    class D3D11ConsoleComputeShader : public D3D11ConsoleShader
    {
    public:
        static D3D11ConsoleComputeShaderPtr create();

        ~D3D11ConsoleComputeShader() override;

        void *getNativeObject() const override;
        
    protected:
        D3D11ConsoleComputeShader() = default;
    };
}


#endif  /*__T3D_D3D11CONSOLE_SHADER_H__*/
