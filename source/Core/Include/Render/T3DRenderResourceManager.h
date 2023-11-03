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
#include "Render/T3DTextureDesc.h"


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

        BlendStatePtr loadBlendState(const BlendDesc &desc, uint32_t hash = 0);

        DepthStencilStatePtr loadDepthStencilState(const DepthStencilDesc &desc, uint32_t hash = 0);

        RasterizerStatePtr loadRasterizerState(const RasterizerDesc &desc, uint32_t hash = 0);

        SamplerStatePtr loadSamplerState(const SamplerDesc &desc, uint32_t hash = 0);
        
        TResult GC() override;
        
    protected:
        RenderStateManager() = default;

        ~RenderStateManager() override = default;
        
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
        
        VertexBufferPtr loadVertexBuffer(size_t vertexSize, size_t vertexCount, 
            const Buffer &buffer, MemoryType memType, Usage usage, CPUAccessMode accMode, const UUID &uuid = UUID::INVALID);

        IndexBufferPtr loadIndexBuffer(IndexType indexType, size_t indexCount,
            const Buffer &buffer, MemoryType memType, Usage usage, CPUAccessMode accMode, const UUID &uuid = UUID::INVALID);

        PixelBuffer1DPtr loadPixelBuffer1D(PixelBuffer1DDesc *desc, MemoryType memType, Usage usage, CPUAccessMode accMode, const UUID &uuid = UUID::INVALID);
        
        PixelBuffer2DPtr loadPixelBuffer2D(PixelBuffer2DDesc *desc, MemoryType memType, Usage usage, CPUAccessMode accMode, const UUID &uuid = UUID::INVALID);
        
        PixelBuffer3DPtr loadPixelBuffer3D(PixelBuffer3DDesc *desc, MemoryType memType, Usage usage, CPUAccessMode accMode, const UUID &uuid = UUID::INVALID);
        
        TResult GC() override;
        
    protected:
        RenderBufferManager() = default;

        ~RenderBufferManager() override = default;

        using BufferCache = TMap<UUID, RenderBufferPtr>;
        
        template<typename BUFFER_TYPE, typename CREATOR, typename ...ARGS>
        SmartPtr<BUFFER_TYPE> loadBuffer(BufferCache &buffers, const UUID &uuid, CREATOR creator, ARGS ...args);
        
        BufferCache mVBufferCache {};
        BufferCache mIBufferCache {};
        BufferCache mPBufferCache {};
        BufferCache mCBufferCache {};
    };

    #define T3D_RENDER_BUFFER_MGR   (RenderBufferManager::getInstance())


    class T3D_ENGINE_API RenderTargetManager
        : public RenderResourceManager
        , public Singleton<RenderTargetManager>
    {
    public:
        static RenderTargetManagerPtr create();

        TResult GC() override;
        
    protected:
        RenderTargetManager() = default;

        ~RenderTargetManager() override = default;
    };
}


#endif  /*__T3D_RENDER_RESOURCE_MANAGER_H__*/
