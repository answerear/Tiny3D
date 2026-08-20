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


#include "T3DMetalRenderState.h"
#include "T3DMetalNative.h"


namespace Tiny3D
{
    MetalBlendStatePtr MetalBlendState::create()
    {
        return T3D_NEW MetalBlendState();
    }

    MetalBlendState::~MetalBlendState()
    {
    }

    void *MetalBlendState::getNativeObject() const
    {
        return nullptr;
    }

    MetalRasterizerStatePtr MetalRasterizerState::create()
    {
        return T3D_NEW MetalRasterizerState();
    }

    MetalRasterizerState::~MetalRasterizerState()
    {
    }

    void *MetalRasterizerState::getNativeObject() const
    {
        return nullptr;
    }

    MetalDepthStencilStatePtr MetalDepthStencilState::create()
    {
        return T3D_NEW MetalDepthStencilState();
    }

    MetalDepthStencilState::~MetalDepthStencilState()
    {
        MetalRelease(mNative);
    }

    void *MetalDepthStencilState::getNativeObject() const
    {
        return mNative;
    }

    void MetalDepthStencilState::setNativeObject(void *state)
    {
        MetalAssign(mNative, state);
    }

    MetalSamplerStatePtr MetalSamplerState::create()
    {
        return T3D_NEW MetalSamplerState();
    }

    MetalSamplerState::~MetalSamplerState()
    {
        MetalRelease(mNative);
    }

    void *MetalSamplerState::getNativeObject() const
    {
        return mNative;
    }

    void MetalSamplerState::setNativeObject(void *state)
    {
        MetalAssign(mNative, state);
    }
}
