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
#include "Render/T3DDataBuffer.h"


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
        static RenderStateManagerPtr create()
        {
            return new RenderStateManager();
        }

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

    class T3D_ENGINE_API RenderBufferManager : public RenderResourceManager
    {
    public:
        ~RenderBufferManager() override = default;

        TResult GC() override;
        
    protected:
        RenderBufferManager() = default;

        RenderBufferPtr loadBuffer(const UUID &uuid, const Buffer &buffer, int32_t argc, ...);

        virtual RenderBufferPtr createBuffer(const Buffer &buffer, int32_t argc, va_list args) = 0;

        using BufferCache = TMap<UUID, RenderBufferPtr>;

        BufferCache mBufferCache {};
    };

    class T3D_ENGINE_API VertexBufferManager
        : public RenderBufferManager
        , public Singleton<VertexBufferManager>
    {
    public:
        static VertexBufferManagerPtr create();
        
        ~VertexBufferManager() override = default;

        VertexBufferPtr loadVertexBuffer(const UUID &uuid, const Buffer &buffer);

    protected:
        VertexBufferManager() = default;

        RenderBufferPtr createBuffer(const Buffer &buffer, int32_t argc, va_list args) override;
    };

    class T3D_ENGINE_API IndexBufferManager
        : public RenderBufferManager
        , public Singleton<IndexBufferManager>
    {
    public:
        static IndexBufferManagerPtr create();
        
        ~IndexBufferManager() override = default;

        IndexBufferPtr loadIndexBuffer(const UUID &uuid, const Buffer &buffer);

    protected:
        IndexBufferManager() = default;

        RenderBufferPtr createBuffer(const Buffer &buffer, int32_t argc, va_list args) override;
    };

    class T3D_ENGINE_API PixelBufferManager
        : public RenderBufferManager
        , public Singleton<PixelBufferManager>
    {
    public:
        static PixelBufferManagerPtr create();
        
        ~PixelBufferManager() override = default;

        PixelBufferPtr loadPixelBuffer(const UUID &uuid, const Buffer &buffer);

    protected:
        PixelBufferManager() = default;

        RenderBufferPtr createBuffer(const Buffer &buffer, int32_t argc, va_list args) override;
    };

    class T3D_ENGINE_API DataBufferManager
        : public RenderBufferManager
        , public Singleton<DataBufferManager>
    {
    public:
        static DataBufferManagerPtr create();
        
        ~DataBufferManager() override = default;

        DataBufferPtr loadDataBuffer(const UUID &uuid, const Buffer &buffer);

    protected:
        DataBufferManager() = default;

        RenderBufferPtr createBuffer(const Buffer &buffer, int32_t argc, va_list args) override;
    };
}


#endif  /*__T3D_RENDER_RESOURCE_MANAGER_H__*/
