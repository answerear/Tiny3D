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


#ifndef __T3D_BLEND_STATE_H__
#define __T3D_BLEND_STATE_H__


#include "Render/T3DRenderResource.h"
#include "Render/T3DRenderConstant.h"


namespace Tiny3D
{
    /**
     * \brief 颜色混合状态
     */
    TSTRUCT()
    struct T3D_ENGINE_API BlendDesc
    {
        enum
        {
            kMaxRenderTarget = T3D_MAX_RENDER_TARGET
        };

        TSTRUCT()
        struct T3D_ENGINE_API RTBlendDesc
        {
            TPROPERTY()
            bool            BlendEnable = false;
            
            TPROPERTY()
            BlendFactor     SrcBlend = BlendFactor::kSrcAlpha;
            
            TPROPERTY()
            BlendFactor     DestBlend = BlendFactor::kOneMinusSrcAlpha;
            
            TPROPERTY()
            BlendOperation  BlendOp = BlendOperation::kAdd;
            
            TPROPERTY()
            BlendFactor     SrcBlendAlpha = BlendFactor::kOne;
            
            TPROPERTY()
            BlendFactor     DstBlendAlpha = BlendFactor::kZero;
            
            TPROPERTY()
            BlendOperation  BlendOpAlpha = BlendOperation::kAdd;
            
            TPROPERTY()
            uint8_t         ColorMask = kWriteMaskAll;
        };

        TPROPERTY()
        bool    AlphaToCoverageEnable = false;
        
        TPROPERTY()
        bool    IndependentBlendEnable = false;
        
        TPROPERTY()
        RTBlendDesc    RenderTargetStates[kMaxRenderTarget]{};
    };

    template class T3D_ENGINE_API RenderStateResource<BlendDesc, RHIBlendState>;
    
    class T3D_ENGINE_API BlendState : public RenderStateResource<BlendDesc, RHIBlendState>
    {
    public:
        static BlendStatePtr create(const BlendDesc &desc, uint32_t hash = 0);

        Type getType() const override;
        
    protected:
        BlendState(uint32_t hash, const BlendDesc &desc);

        ~BlendState() override = default;
        
        bool onLoad() override;

        bool onUnload() override;
    };
}


#endif  /*__T3D_BLEND_STATE_H__*/
