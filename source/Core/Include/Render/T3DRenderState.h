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
