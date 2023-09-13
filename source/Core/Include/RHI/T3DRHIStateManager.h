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


#ifndef __T3D_RHI_STATE_MANAGER_H__
#define __T3D_RHI_STATE_MANAGER_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "RHI/T3DRHIBlendState.h"
#include "RHI/T3DRHIRasterizerState.h"
#include "RHI/T3DRHISamplerState.h"
#include "RHI/T3DRHIDepthStencilState.h"


namespace Tiny3D
{
    class T3D_ENGINE_API RHIStateManagerImpl : public Object
    {
    public:
        virtual RHIBlendStatePtr createBlendState(const BlendState &state) = 0;

        virtual RHIRasterizerStatePtr createRasterizerState(const RasterizerState &state) = 0;

        virtual RHIDepthStencilStatePtr createDepthStencilState(const DepthStencilState &state) = 0;

        virtual RHISamplerStatePtr createSamplerState(const SamplerState &state) = 0;
    };
    
    class T3D_ENGINE_API RHIStateManager
        : public Singleton< RHIStateManager>
        , public RHIStateManagerImpl
    {
    public:
        static RHIStateManagerPtr create();

        void setRHIStateManagerImpl(RHIStateManagerImpl *impl)
        {
            mImpl = impl;
        }

        RHIBlendStatePtr loadBlendState(const BlendState &state);

        RHIDepthStencilStatePtr loadDepthStencilState(const DepthStencilState &state);

        RHIRasterizerStatePtr loadRasterizerState(const RasterizerState &state);

        RHISamplerStatePtr loadSamplerState(const SamplerState &state);

    protected:
        RHIBlendStatePtr createBlendState(const BlendState &state) override;

        RHIRasterizerStatePtr createRasterizerState(const RasterizerState &state) override;

        RHIDepthStencilStatePtr createDepthStencilState(const DepthStencilState &state) override;

        RHISamplerStatePtr createSamplerState(const SamplerState &state) override;

        using RHIStateCache = TMap<uint32_t, RHIStatePtr>;

        bool lookupCache(RHIStateCache &cache, uint32_t hash, RHIState *state);
        
        bool insertCache(RHIStateCache &cache, RHIState *state);

        RHIStateManagerImplPtr  mImpl {nullptr};

        RHIStateCache   mBlendStateCache {};
        RHIStateCache   mDepthStencilStateCache {};
        RHIStateCache   mRasterizerStateCache {};
        RHIStateCache   mSamplerStateCache {};
    };

    #define T3D_RHI_STATE_MGR   (RHIStateManager::getInstance())
}


#endif  /*__T3D_RHI_STATE_MANAGER_H__*/
