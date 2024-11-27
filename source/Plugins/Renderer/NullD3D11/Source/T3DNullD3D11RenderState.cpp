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


#include "T3DNUllD3D11RenderState.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    NullD3D11BlendStatePtr NullD3D11BlendState::create()
    {
        return new NullD3D11BlendState();
    }

    //--------------------------------------------------------------------------

    NullD3D11BlendState::~NullD3D11BlendState()
    {
        
    }

    //--------------------------------------------------------------------------

    void *NullD3D11BlendState::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    NullD3D11RasterizerStatePtr NullD3D11RasterizerState::create()
    {
        return new NullD3D11RasterizerState();
    }
    
    //--------------------------------------------------------------------------

    NullD3D11RasterizerState::~NullD3D11RasterizerState()
    {
    }

    //--------------------------------------------------------------------------

    void *NullD3D11RasterizerState::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    NullD3D11DepthStencilStatePtr NullD3D11DepthStencilState::create()
    {
        return new NullD3D11DepthStencilState();
    }

    //--------------------------------------------------------------------------

    NullD3D11DepthStencilState::~NullD3D11DepthStencilState()
    {
    }

    //--------------------------------------------------------------------------

    void *NullD3D11DepthStencilState::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    NullD3D11SamplerStatePtr NullD3D11SamplerState::create()
    {
        return new NullD3D11SamplerState();
    }

    //--------------------------------------------------------------------------

    NullD3D11SamplerState::~NullD3D11SamplerState()
    {
    }

    //--------------------------------------------------------------------------

    void *NullD3D11SamplerState::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------
}


