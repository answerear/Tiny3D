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

#ifndef __T3D_D3D11_RENDER_STATE_H__
#define __T3D_D3D11_RENDER_STATE_H__


#include "T3DD3D11Prerequisites.h"


namespace Tiny3D
{
    class D3D11BlendState : public RHIBlendState
    {
    public:
        static D3D11BlendStatePtr create();

        ~D3D11BlendState() override;

        ID3D11BlendState    *D3DBlendState {nullptr};
        
    protected:
        D3D11BlendState() = default;
    };

    class D3D11RasterizerState : public RHIRasterizerState
    {
    public:
        static D3D11RasterizerStatePtr create();

        ~D3D11RasterizerState() override;

        ID3D11RasterizerState   *D3DRasterizerState {nullptr};
        
    protected:
        D3D11RasterizerState() = default;
    };

    class D3D11DepthStencilState : public RHIDepthStencilState
    {
    public:
        static D3D11DepthStencilStatePtr create();

        ~D3D11DepthStencilState() override;

        ID3D11DepthStencilState *D3DDepthStencilState {nullptr};
        
    protected:
        D3D11DepthStencilState() = default;
    };

    class D3D11SamplerState : public RHISamplerState
    {
    public:
        static D3D11SamplerStatePtr create();

        ~D3D11SamplerState() override;

        ID3D11SamplerState  *D3DSamplerState {nullptr};
        
    protected:
        D3D11SamplerState() = default;
    };
}


#endif    /*__T3D_D3D11_RENDER_STATE_H__*/
