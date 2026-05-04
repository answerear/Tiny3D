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


#include "T3DVKConsoleRenderState.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    VKConsoleBlendStatePtr VKConsoleBlendState::create()
    {
        return T3D_NEW VKConsoleBlendState();
    }

    //--------------------------------------------------------------------------

    VKConsoleBlendState::~VKConsoleBlendState()
    {
    }

    //--------------------------------------------------------------------------

    void *VKConsoleBlendState::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VKConsoleRasterizerStatePtr VKConsoleRasterizerState::create()
    {
        return T3D_NEW VKConsoleRasterizerState();
    }

    //--------------------------------------------------------------------------

    VKConsoleRasterizerState::~VKConsoleRasterizerState()
    {
    }

    //--------------------------------------------------------------------------

    void *VKConsoleRasterizerState::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VKConsoleDepthStencilStatePtr VKConsoleDepthStencilState::create()
    {
        return T3D_NEW VKConsoleDepthStencilState();
    }

    //--------------------------------------------------------------------------

    VKConsoleDepthStencilState::~VKConsoleDepthStencilState()
    {
    }

    //--------------------------------------------------------------------------

    void *VKConsoleDepthStencilState::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    VKConsoleSamplerStatePtr VKConsoleSamplerState::create()
    {
        return T3D_NEW VKConsoleSamplerState();
    }

    //--------------------------------------------------------------------------

    VKConsoleSamplerState::~VKConsoleSamplerState()
    {
    }

    //--------------------------------------------------------------------------

    void *VKConsoleSamplerState::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
}
