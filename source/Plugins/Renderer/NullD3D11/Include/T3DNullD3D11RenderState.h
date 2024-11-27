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

#ifndef __T3D_NULLD3D11_RENDERER_H__
#define __T3D_NULLD3D11_RENDERER_H__


#include "T3DNullD3D11Prerequisites.h"


namespace Tiny3D
{
    class NullD3D11BlendState : public RHIBlendState
    {
    public:
        static NullD3D11BlendStatePtr create();

        ~NullD3D11BlendState() override;

        void *getNativeObject() const override;
        
    protected:
        NullD3D11BlendState() = default;
    };

    class NullD3D11RasterizerState : public RHIRasterizerState
    {
    public:
        static NullD3D11RasterizerStatePtr create();

        ~NullD3D11RasterizerState() override;

        void *getNativeObject() const override;
        
    protected:
        NullD3D11RasterizerState() = default;
    };

    class NullD3D11DepthStencilState : public RHIDepthStencilState
    {
    public:
        static NullD3D11DepthStencilStatePtr create();

        ~NullD3D11DepthStencilState() override;

        void *getNativeObject() const override;
        
    protected:
        NullD3D11DepthStencilState() = default;
    };

    class NullD3D11SamplerState : public RHISamplerState
    {
    public:
        static NullD3D11SamplerStatePtr create();

        ~NullD3D11SamplerState() override;

        void *getNativeObject() const override;
        
    protected:
        NullD3D11SamplerState() = default;
    };
}


#endif    /*__T3D_NULLD3D11_RENDERER_H__*/
