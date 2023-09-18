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


#ifndef __T3D_RENDER_STATE_H__
#define __T3D_RENDER_STATE_H__


#include "Render/T3DBlendState.h"
#include "Render/T3DDepthStencilState.h"
#include "Render/T3DRasterizerState.h"


namespace Tiny3D
{
    TSTRUCT()
    struct T3D_ENGINE_API RenderState
    {
        TPROPERTY(RTTRFuncName="BlendState", RTTRFuncType="getter")
        const BlendState &getBlendState() const { return Blend; }

        TPROPERTY(RTTRFuncName="BlendState", RTTRFuncType="setter")
        void setBlendState(const BlendState &state) { Blend = state; }

        TPROPERTY(RTTRFuncName="DepthStencilState", RTTRFuncType="getter")
        const DepthStencilState &getDepthStencil() const { return DepthStencil; }

        TPROPERTY(RTTRFuncName="DepthStencilState", RTTRFuncType="setter")
        void setDepthStencilState(const DepthStencilState &state) { DepthStencil = state; }

        TPROPERTY(RTTRFuncName="RasterizerState", RTTRFuncType="getter")
        const RasterizerState &getRasterizerState() const { return Rasterizer; }

        TPROPERTY(RTTRFuncName="RasterizerState", RTTRFuncType="setter")
        void setRasterizerState(const RasterizerState &state) { Rasterizer = state; }
        
        BlendState        Blend {};
        DepthStencilState DepthStencil {};
        RasterizerState   Rasterizer{};
    };
}


#endif  /*__T3D_RENDER_STATE_H__*/
