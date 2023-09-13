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


#include "RHI/T3DRHIStateManager.h"
#include "RHI/T3DRHIBlendState.h"
#include "RHI/T3DRHIDepthStencilState.h"
#include "RHI/T3DRHIRasterizerState.h"
#include "RHI/T3DRHISamplerState.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    RHIStateManagerPtr RHIStateManager::create()
    {
        return new RHIStateManager();
    }

    //--------------------------------------------------------------------------

    RHIBlendStatePtr RHIStateManager::loadBlendState(const BlendState &state)
    {
        RHIBlendStatePtr stat;

        do
        {
            uint32_t hash = calcCRC(state);
        
            if (!lookupCache(mBlendStateCache, hash, stat))
            {
                stat = createBlendState(state);
                insertCache(mBlendStateCache, stat);
            }
        } while (false);

        return stat;
    }

    //--------------------------------------------------------------------------
    
    RHIDepthStencilStatePtr RHIStateManager::loadDepthStencilState(const DepthStencilState &state)
    {
        RHIDepthStencilStatePtr stat;

        do
        {
            uint32_t hash = calcCRC(state);

            if (!lookupCache(mDepthStencilStateCache, hash, stat))
            {
                stat = createDepthStencilState(state);
                insertCache(mDepthStencilStateCache, stat);
            }
        } while (false);

        return stat;
    }

    //--------------------------------------------------------------------------
    
    RHIRasterizerStatePtr RHIStateManager::loadRasterizerState(const RasterizerState &state)
    {
        RHIRasterizerStatePtr stat;

        do
        {
            uint32_t hash = calcCRC(state);

            if (!lookupCache(mRasterizerStateCache, hash, stat))
            {
                stat = createRasterizerState(state);
                insertCache(mRasterizerStateCache, stat);
            }
        } while (false);

        return stat;
    }

    //--------------------------------------------------------------------------
    
    RHISamplerStatePtr RHIStateManager::loadSamplerState(const SamplerState &state)
    {
        RHISamplerStatePtr stat;

        do
        {
            uint32_t hash = calcCRC(state);

            if (!lookupCache(mSamplerStateCache, hash, stat))
            {
                stat = createSamplerState(state);
                insertCache(mSamplerStateCache, stat);
            }
        } while (false);

        return stat;
    }
    
    //--------------------------------------------------------------------------

    RHIBlendStatePtr RHIStateManager::createBlendState(const BlendState &state)
    {
        RHIBlendStatePtr RHIState;
        
        if (mImpl != nullptr)
        {
            RHIState = mImpl->createBlendState(state);
        }
        
        return RHIState;
    }

    //--------------------------------------------------------------------------

    RHIRasterizerStatePtr RHIStateManager::createRasterizerState(const RasterizerState &state)
    {
        RHIRasterizerStatePtr RHIState;

        if (mImpl != nullptr)
        {
            RHIState = mImpl->createRasterizerState(state);
        }

        return RHIState;
    }
    
    //--------------------------------------------------------------------------

    RHIDepthStencilStatePtr RHIStateManager::createDepthStencilState(const DepthStencilState &state)
    {
        RHIDepthStencilStatePtr RHIState;

        if (mImpl != nullptr)
        {
            RHIState = mImpl->createDepthStencilState(state);
        }

        return RHIState;
    }
    
    //--------------------------------------------------------------------------

    RHISamplerStatePtr RHIStateManager::createSamplerState(const SamplerState &state)
    {
        RHISamplerStatePtr RHIState;

        if (mImpl != nullptr)
        {
            RHIState = mImpl->createSamplerState(state);
        }

        return RHIState;
    }
    
    //--------------------------------------------------------------------------

    bool RHIStateManager::lookupCache(RHIStateCache &cache, uint32_t hash, RHIState *state)
    {
        bool ret = true;

        auto itr = cache.find(hash);
        if (itr == cache.end())
        {
            // Could not found
            ret = false;
        }
        else
        {
            state = itr->second;
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    bool RHIStateManager::insertCache(RHIStateCache &cache, RHIState *state)
    {
        auto rval = cache.emplace(state->hash(), state);
        return rval.second;
    }

    //--------------------------------------------------------------------------
}

