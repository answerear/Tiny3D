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


#include "Render/T3DRenderResourceManager.h"


namespace  Tiny3D
{
    //--------------------------------------------------------------------------

    bool RenderResourceManager::PostLoad(RenderResourcePtr res)
    {
        bool ret = false;
        if (res != nullptr)
        {
            ret = res->onLoad();
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    bool RenderResourceManager::PreUnload(RenderResourcePtr res)
    {
        bool ret = false;
        if (res != nullptr)
        {
            ret = res->onUnload();
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    template<typename STATE_TYPE, typename DESC_TYPE>
    SmartPtr<STATE_TYPE> RenderStateManager::loadState(StateCache &states, const DESC_TYPE &desc, uint32_t hash /* = 0 */)
    {
        SmartPtr<STATE_TYPE> state = STATE_TYPE::create(desc, hash);
        auto rval = states.emplace(state->hash(), state);
        if (rval.second)
        {
            if (!PostLoad(state))
            {
                states.erase(state->hash());
                state = nullptr;
            }
        }

        return state;
    }

    //--------------------------------------------------------------------------

    BlendStatePtr RenderStateManager::loadBlendState(const BlendDesc &desc, uint32_t hash)
    {
        return loadState<BlendState, BlendDesc>(mBlendStates, desc, hash);
    }
    
    //--------------------------------------------------------------------------

    DepthStencilStatePtr RenderStateManager::loadDepthStencilState(const DepthStencilDesc &desc, uint32_t hash)
    {
        return loadState<DepthStencilState, DepthStencilDesc>(mDepthStencilStates, desc, hash);
    }

    //--------------------------------------------------------------------------

    RasterizerStatePtr RenderStateManager::loadRasterizerState(const RasterizerDesc &desc, uint32_t hash)
    {
        return loadState<RasterizerState, RasterizerDesc>(mRasterizerStates, desc, hash);
    }

    //--------------------------------------------------------------------------
    
    SamplerStatePtr RenderStateManager::loadSamplerState(const SamplerDesc &desc, uint32_t hash)
    {
        return loadState<SamplerState, SamplerDesc>(mSamplerStates, desc, hash);
    }
    
    //--------------------------------------------------------------------------
    
    TResult RenderStateManager::GC()
    {
        TResult ret = T3D_OK;

        do
        {
            ret = _GC<StateCache>(mBlendStates);
            if (T3D_FAILED(ret))
            {
                break;
            }

            ret = _GC(mDepthStencilStates);
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            ret = _GC(mRasterizerStates);
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            ret = _GC(mSamplerStates);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult RenderBufferManager::GC()
    {
        return _GC(mBufferCache);
    }

    //--------------------------------------------------------------------------

    RenderBufferPtr RenderBufferManager::loadBuffer(const UUID &uuid, const Buffer &buffer, RenderBuffer::MemoryType memType, RenderBuffer::Usage usage, uint32_t accMode, int32_t argc, ...)
    {
        va_list args;
        va_start(args, argc);
        RenderBufferPtr rb = createBuffer(buffer, memType, usage, accMode, argc, args);
        va_end(args);
        if (!PostLoad(rb))
        {
            rb = nullptr;
        }
        return rb;
    }

    //--------------------------------------------------------------------------

    VertexBufferPtr VertexBufferManager::loadVertexBuffer(const UUID &uuid, const Buffer &buffer, RenderBuffer::MemoryType memType, RenderBuffer::Usage usage, uint32_t accMode)
    {
        return smart_pointer_cast<VertexBuffer>(loadBuffer(uuid, buffer, memType, usage, accMode, 0));
    }

    //--------------------------------------------------------------------------

    RenderBufferPtr VertexBufferManager::createBuffer(const Buffer &buffer, RenderBuffer::MemoryType memType, RenderBuffer::Usage usage, uint32_t accMode, int32_t argc, va_list args)
    {
        return VertexBuffer::create(buffer,memType, usage, accMode);
    }

    //--------------------------------------------------------------------------

    IndexBufferPtr IndexBufferManager::loadIndexBuffer(const UUID &uuid, const Buffer &buffer, RenderBuffer::MemoryType memType, RenderBuffer::Usage usage, uint32_t accMode)
    {
        return smart_pointer_cast<IndexBuffer>(loadBuffer(uuid, buffer, memType, usage, accMode, 0));
    }

    //--------------------------------------------------------------------------

    RenderBufferPtr IndexBufferManager::createBuffer(const Buffer &buffer, RenderBuffer::MemoryType memType, RenderBuffer::Usage usage, uint32_t accMode, int32_t argc, va_list args)
    {
        return IndexBuffer::create(buffer, memType, usage, accMode);
    }
    
    //--------------------------------------------------------------------------

    PixelBufferPtr PixelBufferManager::loadPixelBuffer(const UUID &uuid, const Buffer &buffer, RenderBuffer::MemoryType memType, RenderBuffer::Usage usage, uint32_t accMode)
    {
        return smart_pointer_cast<PixelBuffer>(loadBuffer(uuid, buffer, memType, usage, accMode, 0));
    }

    //--------------------------------------------------------------------------

    RenderBufferPtr PixelBufferManager::createBuffer(const Buffer &buffer, RenderBuffer::MemoryType memType, RenderBuffer::Usage usage, uint32_t accMode, int32_t argc, va_list args)
    {
        return PixelBuffer::create(buffer, memType, usage, accMode);
    }
    
    //--------------------------------------------------------------------------

    DataBufferPtr DataBufferManager::loadDataBuffer(const UUID &uuid, const Buffer &buffer, RenderBuffer::MemoryType memType, RenderBuffer::Usage usage, uint32_t accMode)
    {
        return smart_pointer_cast<DataBuffer>(loadBuffer(uuid, buffer, memType, usage, accMode, 0));
    }

    //--------------------------------------------------------------------------

    RenderBufferPtr DataBufferManager::createBuffer(const Buffer &buffer, RenderBuffer::MemoryType memType, RenderBuffer::Usage usage, uint32_t accMode, int32_t argc, va_list args)
    {
        return DataBuffer::create(buffer, memType, usage, accMode);
    }
    
    //--------------------------------------------------------------------------
}
