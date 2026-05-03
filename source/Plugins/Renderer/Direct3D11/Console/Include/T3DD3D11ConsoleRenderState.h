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

#ifndef __T3D_D3D11CONSOLE_RENDERER_H__
#define __T3D_D3D11CONSOLE_RENDERER_H__


#include "T3DD3D11ConsolePrerequisites.h"


namespace Tiny3D
{
    class D3D11ConsoleBlendState : public RHIBlendState
    {
    public:
        static D3D11ConsoleBlendStatePtr create();

        ~D3D11ConsoleBlendState() override;

        void *getNativeObject() const override;
        
    protected:
        D3D11ConsoleBlendState() = default;
    };

    class D3D11ConsoleRasterizerState : public RHIRasterizerState
    {
    public:
        static D3D11ConsoleRasterizerStatePtr create();

        ~D3D11ConsoleRasterizerState() override;

        void *getNativeObject() const override;
        
    protected:
        D3D11ConsoleRasterizerState() = default;
    };

    class D3D11ConsoleDepthStencilState : public RHIDepthStencilState
    {
    public:
        static D3D11ConsoleDepthStencilStatePtr create();

        ~D3D11ConsoleDepthStencilState() override;

        void *getNativeObject() const override;
        
    protected:
        D3D11ConsoleDepthStencilState() = default;
    };

    class D3D11ConsoleSamplerState : public RHISamplerState
    {
    public:
        static D3D11ConsoleSamplerStatePtr create();

        ~D3D11ConsoleSamplerState() override;

        void *getNativeObject() const override;
        
    protected:
        D3D11ConsoleSamplerState() = default;
    };
}


#endif    /*__T3D_D3D11CONSOLE_RENDERER_H__*/
