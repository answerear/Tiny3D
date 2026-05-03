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


#include "T3DD3D11ConsoleRenderState.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11ConsoleBlendStatePtr D3D11ConsoleBlendState::create()
    {
        return T3D_NEW D3D11ConsoleBlendState();
    }

    //--------------------------------------------------------------------------

    D3D11ConsoleBlendState::~D3D11ConsoleBlendState()
    {
        
    }

    //--------------------------------------------------------------------------

    void *D3D11ConsoleBlendState::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    D3D11ConsoleRasterizerStatePtr D3D11ConsoleRasterizerState::create()
    {
        return T3D_NEW D3D11ConsoleRasterizerState();
    }
    
    //--------------------------------------------------------------------------

    D3D11ConsoleRasterizerState::~D3D11ConsoleRasterizerState()
    {
    }

    //--------------------------------------------------------------------------

    void *D3D11ConsoleRasterizerState::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    D3D11ConsoleDepthStencilStatePtr D3D11ConsoleDepthStencilState::create()
    {
        return T3D_NEW D3D11ConsoleDepthStencilState();
    }

    //--------------------------------------------------------------------------

    D3D11ConsoleDepthStencilState::~D3D11ConsoleDepthStencilState()
    {
    }

    //--------------------------------------------------------------------------

    void *D3D11ConsoleDepthStencilState::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    D3D11ConsoleSamplerStatePtr D3D11ConsoleSamplerState::create()
    {
        return T3D_NEW D3D11ConsoleSamplerState();
    }

    //--------------------------------------------------------------------------

    D3D11ConsoleSamplerState::~D3D11ConsoleSamplerState()
    {
    }

    //--------------------------------------------------------------------------

    void *D3D11ConsoleSamplerState::getNativeObject() const
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------
}


