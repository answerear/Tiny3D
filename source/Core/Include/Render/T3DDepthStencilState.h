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


#ifndef __T3D_DEPTH_STENCIL_STATE_H__
#define __T3D_DEPTH_STENCIL_STATE_H__


#include "Render/T3DRenderResource.h"
#include "Render/T3DRenderConstant.h"


namespace Tiny3D
{
    TSTRUCT()
    struct T3D_ENGINE_API StencilOpDesc
    {
        TPROPERTY()
        CompareFunction StencilFunc = CompareFunction::kAlwaysPass;

        TPROPERTY()
        StencilOp       StencilFailOp = StencilOp::kKeep;

        TPROPERTY()
        StencilOp       StencilDepthFailOp = StencilOp::kKeep;

        TPROPERTY()
        StencilOp       StencilPassOp = StencilOp::kKeep;
    };

    /**
     * \brief 深度缓冲和模板缓冲状态
     */
    TSTRUCT()
    struct T3D_ENGINE_API DepthStencilDesc
    {
        TPROPERTY()
        bool            DepthTestEnable = true;
        
        TPROPERTY()
        bool            DepthWriteEnable = true;
        
        TPROPERTY()
        CompareFunction DepthFunc = CompareFunction::kLess;

        TPROPERTY()
        bool            StencilEnable = false;
        
        TPROPERTY()
        uint32_t        StencilRef = 0;
        
        TPROPERTY()
        uint8_t         StencilReadMask = 0xFF;
        
        TPROPERTY()
        uint8_t         StencilWriteMask = 0xFF;

        // Front face
        TPROPERTY()
        StencilOpDesc   FrontFace {};
        
        // Back face
        TPROPERTY()
        StencilOpDesc   BackFace {};
    };

    template class T3D_ENGINE_API RenderStateResource<DepthStencilDesc, RHIDepthStencilState>;
    
    class T3D_ENGINE_API DepthStencilState : public RenderStateResource<DepthStencilDesc, RHIDepthStencilState>
    {
    public:
        static DepthStencilStatePtr create(const DepthStencilDesc &desc, uint32_t hash = 0);
        
        Type getType() const override;

    protected:
        DepthStencilState(uint32_t hash, const DepthStencilDesc &desc);
        
        ~DepthStencilState() override = default;

        bool onLoad() override;

        bool onUnload() override;
    };
}


#endif  /*__T3D_DEPTH_STENCIL_STATE_H__*/
