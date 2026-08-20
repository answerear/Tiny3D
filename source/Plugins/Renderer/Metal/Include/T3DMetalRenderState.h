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

#ifndef __T3D_METAL_RENDER_STATE_H__
#define __T3D_METAL_RENDER_STATE_H__


#include "T3DMetalPrerequisites.h"
#include "Render/T3DBlendState.h"
#include "Render/T3DRasterizerState.h"
#include "Render/T3DDepthStencilState.h"
#include "Render/T3DSamplerState.h"


namespace Tiny3D
{
    class MetalBlendState : public RHIBlendState
    {
    public:
        static MetalBlendStatePtr create();

        ~MetalBlendState() override;

        void *getNativeObject() const override;

        void setDesc(const BlendDesc &desc) { mDesc = desc; }

        const BlendDesc &getDesc() const { return mDesc; }

    protected:
        MetalBlendState() = default;

        BlendDesc mDesc {};
    };

    class MetalRasterizerState : public RHIRasterizerState
    {
    public:
        static MetalRasterizerStatePtr create();

        ~MetalRasterizerState() override;

        void *getNativeObject() const override;

        void setDesc(const RasterizerDesc &desc) { mDesc = desc; }

        const RasterizerDesc &getDesc() const { return mDesc; }

    protected:
        MetalRasterizerState() = default;

        RasterizerDesc mDesc {};
    };

    class MetalDepthStencilState : public RHIDepthStencilState
    {
    public:
        static MetalDepthStencilStatePtr create();

        ~MetalDepthStencilState() override;

        void *getNativeObject() const override;

        void setNativeObject(void *state);

        void setStencilRef(uint32_t ref) { mStencilRef = ref; }

        uint32_t getStencilRef() const { return mStencilRef; }

    protected:
        MetalDepthStencilState() = default;

        void        *mNative {nullptr};
        uint32_t    mStencilRef {0};
    };

    class MetalSamplerState : public RHISamplerState
    {
    public:
        static MetalSamplerStatePtr create();

        ~MetalSamplerState() override;

        void *getNativeObject() const override;

        void setNativeObject(void *state);

    protected:
        MetalSamplerState() = default;

        void *mNative {nullptr};
    };
}


#endif    /*__T3D_METAL_RENDER_STATE_H__*/
