/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#ifndef __T3D_BLEND_STATE_H__
#define __T3D_BLEND_STATE_H__


#include "Render/T3DRenderResource.h"
#include "Render/T3DRenderConstant.h"
#include "RHI/T3DRHIBlendState.h"


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
            kMaxRenderTarget = 8
        };

        TSTRUCT()
        struct T3D_ENGINE_API RTBlendDesc
        {
            TPROPERTY()
            bool            BlendEnable = false;
            
            TPROPERTY()
            BlendFactor     SrcBlend = BlendFactor::kOne;
            
            TPROPERTY()
            BlendFactor     DestBlend = BlendFactor::kZero;
            
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
