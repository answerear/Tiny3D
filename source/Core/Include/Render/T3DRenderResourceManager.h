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


#ifndef __T3D_RENDER_RESOURCE_MANAGER_H__
#define __T3D_RENDER_RESOURCE_MANAGER_H__


#include "Render/T3DBlendState.h"
#include "Render/T3DDepthStencilState.h"
#include "Render/T3DRasterizerState.h"
#include "Render/T3DSamplerState.h"
#include "Render/T3DVertexBuffer.h"
#include "Render/T3DIndexBuffer.h"
#include "Render/T3DPixelBuffer.h"
#include "Render/T3DConstantBuffer.h"


namespace Tiny3D
{
    class T3D_ENGINE_API RenderResourceManager : public Object
    {
    public:
        ~RenderResourceManager() override = default;

        virtual TResult GC() = 0;
        
    protected:
        RenderResourceManager() = default;

        bool PostLoad(RenderResourcePtr res);

        bool PreUnload(RenderResourcePtr res);

        template<typename MAP_TYPE>
        TResult _GC(MAP_TYPE &container)
        {
            auto itr = container.begin();
            
            while (itr != container.end())
            {
                RenderResourcePtr res = itr->second;
                if (res->referCount() == 1)
                {
                    // 没有外部引用，删掉
                    if (PreUnload(res))
                    {
                        itr = container.erase(itr);
                    }
                    else
                    {
                        ++itr;
                    }
                }
                else
                {
                    ++itr;
                }
            }
            
            return T3D_OK;
        }
    };

    
    class RenderStateManager
        : public RenderResourceManager
        , public Singleton<RenderStateManager>
    {
    public:
        static RenderStateManagerPtr create();

        ~RenderStateManager() override = default;

        BlendStatePtr loadBlendState(const BlendDesc &desc, uint32_t hash = 0);

        DepthStencilStatePtr loadDepthStencilState(const DepthStencilDesc &desc, uint32_t hash = 0);

        RasterizerStatePtr loadRasterizerState(const RasterizerDesc &desc, uint32_t hash = 0);

        SamplerStatePtr loadSamplerState(const SamplerDesc &desc, uint32_t hash = 0);
        
        TResult GC() override;
        
    protected:
        RenderStateManager() = default;

        using StateCache = TMap<uint32_t, RenderResourcePtr>;

        template<typename STATE_TYPE, typename DESC_TYPE> 
        SmartPtr<STATE_TYPE> loadState(StateCache &states, const DESC_TYPE &desc, uint32_t hash = 0);

        StateCache  mBlendStates {};
        StateCache  mDepthStencilStates {};
        StateCache  mRasterizerStates {};
        StateCache  mSamplerStates {};
    };

    #define T3D_RENDER_STATE_MGR (RenderStateManager::getInstance())

    
    class T3D_ENGINE_API RenderBufferManager
        : public RenderResourceManager
        , public Singleton<RenderBufferManager>
    {
    public:
        static RenderBufferManagerPtr create();
        
        ~RenderBufferManager() override = default;

        VertexBufferPtr loadVertexBuffer(const Buffer &buffer, size_t vertexSize, size_t vertexCount, 
            RenderBuffer::MemoryType memType, RenderBuffer::Usage usage, uint32_t accMode, const UUID &uuid = UUID::INVALID);

        IndexBufferPtr loadIndexBuffer(const Buffer &buffer, IndexBuffer::IndexType indexType, size_t indexCount,
            RenderBuffer::MemoryType memType, RenderBuffer::Usage usage, uint32_t accMode, const UUID &uuid = UUID::INVALID);

        PixelBufferPtr loadPixelBuffer(const Buffer &buffer, size_t width, size_t height, PixelFormat format, size_t mipmaps,
            RenderBuffer::MemoryType memType, RenderBuffer::Usage usage, uint32_t accMode, const UUID &uuid = UUID::INVALID);
        
        TResult GC() override;
        
    protected:
        RenderBufferManager() = default;

        // RenderBufferPtr loadBuffer(const UUID &uuid, const Buffer &buffer, RenderBuffer::MemoryType memType, RenderBuffer::Usage usage, uint32_t accMode, int32_t argc, ...);
        //
        // virtual RenderBufferPtr createBuffer(const Buffer &buffer, RenderBuffer::MemoryType memType, RenderBuffer::Usage usage, uint32_t accMode, int32_t argc, va_list args) = 0;

        using BufferCache = TMap<UUID, RenderBufferPtr>;
        
        template<typename BUFFER_TYPE, typename CREATOR, typename ...ARGS>
        SmartPtr<BUFFER_TYPE> loadBuffer(BufferCache &buffers, const UUID &uuid, CREATOR creator, ARGS ...args);
        
        BufferCache mVBufferCache {};
        BufferCache mIBufferCache {};
        BufferCache mPBufferCache {};
        BufferCache mDBufferCache {};
    };

    #define T3D_RENDER_BUFFER_mgr   (RenderBufferManager::getInstance())
}


#endif  /*__T3D_RENDER_RESOURCE_MANAGER_H__*/
