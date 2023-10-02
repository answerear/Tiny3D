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


#ifndef __T3D_DEPTH_STENCIL_STATE_H__
#define __T3D_DEPTH_STENCIL_STATE_H__


#include "Render/T3DRenderResource.h"
#include "Render/T3DRenderConstant.h"
#include "RHI/T3DRHIDepthStencilState.h"


namespace Tiny3D
{
    /**
     * \brief 深度缓冲和模板缓冲状态
     */
    TSTRUCT()
    struct T3D_ENGINE_API DepthStencilDesc
    {
        TSTRUCT()
        struct T3D_ENGINE_API StencilOpDesc
        {
            TPROPERTY()
            CompareFunction StencilFunc;

            TPROPERTY()
            StencilOp       StencilFailOp;

            TPROPERTY()
            StencilOp       StencilDepthFailOp;

            TPROPERTY()
            StencilOp       StencilPassOp;
        };

        TPROPERTY()
        bool            DepthTestEnable = true;
        
        TPROPERTY()
        bool            DepthWriteEnable = true;
        
        TPROPERTY()
        CompareFunction DepthFunc;

        TPROPERTY()
        bool            StencilEnable;
        
        TPROPERTY()
        uint32_t        StencilRef;
        
        TPROPERTY()
        uint8_t         StencilReadMask;
        
        TPROPERTY()
        uint8_t         StencilWriteMask;

        // Front face
        TPROPERTY()
        StencilOpDesc   FrontFace;
        
        // Back face
        TPROPERTY()
        StencilOpDesc   BackFace;
    };

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
