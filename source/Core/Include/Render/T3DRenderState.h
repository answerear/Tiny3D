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


#ifndef __T3D_RENDER_STATE_H__
#define __T3D_RENDER_STATE_H__


#include "Render/T3DBlendState.h"
#include "Render/T3DDepthStencilState.h"
#include "Render/T3DRasterizerState.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API RenderState : public Object
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        static RenderStatePtr create();
        
        BlendStatePtr getBlendState() const { return mBlendState; }

        DepthStencilStatePtr getDepthStencilState() const { return mDepthStencilState; }

        RasterizerStatePtr getRasterizerState() const { return mRasterizerState; }
        
        TPROPERTY(RTTRFuncName="BlendDesc", RTTRFuncType="getter")
        const BlendDesc &getBlendDesc() const { return mBlendDesc; }

        TPROPERTY(RTTRFuncName="BlendDesc", RTTRFuncType="setter")
        void setBlendDesc(const BlendDesc &desc);

        TPROPERTY(RTTRFuncName="DepthStencilDesc", RTTRFuncType="getter")
        const DepthStencilDesc &getDepthStencilDesc() const { return mDepthStencilDesc; }

        TPROPERTY(RTTRFuncName="DepthStencilDesc", RTTRFuncType="setter")
        void setDepthStencilDesc(const DepthStencilDesc &desc);

        TPROPERTY(RTTRFuncName="RasterizerDesc", RTTRFuncType="getter")
        const RasterizerDesc &getRasterizerDesc() const { return mRasterizerDesc; }

        TPROPERTY(RTTRFuncName="RasterizerDesc", RTTRFuncType="setter")
        void setRasterizerDesc(const RasterizerDesc &desc);

    protected:
        BlendDesc           mBlendDesc {};
        DepthStencilDesc    mDepthStencilDesc {};
        RasterizerDesc      mRasterizerDesc{};

        BlendStatePtr           mBlendState {nullptr};
        DepthStencilStatePtr    mDepthStencilState {nullptr};
        RasterizerStatePtr      mRasterizerState {nullptr};
    };
}


#endif  /*__T3D_RENDER_STATE_H__*/
