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


#include "T3DD3D11RenderState.h"
#include "T3DD3D11Context.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11BlendStatePtr D3D11BlendState::create()
    {
        return T3D_NEW D3D11BlendState();
    }

    //--------------------------------------------------------------------------

    D3D11BlendState::~D3D11BlendState()
    {
        D3D_SAFE_RELEASE(D3DBlendState);
    }

    //--------------------------------------------------------------------------

    void *D3D11BlendState::getNativeObject() const
    {
        return D3DBlendState;
    }

    //--------------------------------------------------------------------------

    D3D11RasterizerStatePtr D3D11RasterizerState::create()
    {
        return T3D_NEW D3D11RasterizerState();
    }
    
    //--------------------------------------------------------------------------

    D3D11RasterizerState::~D3D11RasterizerState()
    {
        D3D_SAFE_RELEASE(D3DRasterizerState);
    }

    //--------------------------------------------------------------------------

    void *D3D11RasterizerState::getNativeObject() const
    {
        return D3DRasterizerState;
    }
    
    //--------------------------------------------------------------------------

    D3D11DepthStencilStatePtr D3D11DepthStencilState::create()
    {
        return T3D_NEW D3D11DepthStencilState();
    }

    //--------------------------------------------------------------------------

    D3D11DepthStencilState::~D3D11DepthStencilState()
    {
        D3D_SAFE_RELEASE(D3DDepthStencilState);
    }

    //--------------------------------------------------------------------------

    void *D3D11DepthStencilState::getNativeObject() const
    {
        return D3DDepthStencilState;
    }
    
    //--------------------------------------------------------------------------

    D3D11SamplerStatePtr D3D11SamplerState::create()
    {
        return T3D_NEW D3D11SamplerState();
    }

    //--------------------------------------------------------------------------

    D3D11SamplerState::~D3D11SamplerState()
    {
        D3D_SAFE_RELEASE(D3DSamplerState);
    }

    //--------------------------------------------------------------------------

    void *D3D11SamplerState::getNativeObject() const
    {
        return D3DSamplerState;
    }
    
    //--------------------------------------------------------------------------
}


