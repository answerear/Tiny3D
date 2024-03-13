/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#ifndef __T3D_RENDER_RESOURCE_MANAGER_H__
#define __T3D_RENDER_RESOURCE_MANAGER_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Render/T3DRenderConstant.h"


namespace Tiny3D
{
    class T3D_ENGINE_API RenderResourceManager : public Object
    {
    public:
        ~RenderResourceManager() override = default;

        virtual TResult GC() = 0;
        
    protected:
        RenderResourceManager() = default;

        bool postLoad(RenderResourcePtr res);

        bool preUnload(RenderResourcePtr res);

        template<typename CONTAINER_T>
        TResult _GC(CONTAINER_T &container);

        template<typename CONTAINER_T>
        void clear(CONTAINER_T &container);
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

        ~RenderStateManager() override;
        
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

        VertexDeclarationPtr addVertexDeclaration(VertexDeclarationPtr decl);
        
        VertexBufferPtr loadVertexBuffer(size_t vertexSize, size_t vertexCount, 
            const Buffer &buffer, MemoryType memType, Usage usage, CPUAccessMode accMode, const UUID &uuid = UUID::INVALID);

        IndexBufferPtr loadIndexBuffer(IndexType indexType, size_t indexCount,
            const Buffer &buffer, MemoryType memType, Usage usage, CPUAccessMode accMode, const UUID &uuid = UUID::INVALID);

        ConstantBufferPtr loadConstantBuffer(const Buffer &buffer, MemoryType memType, Usage usage, CPUAccessMode accMode, const UUID &uuid = UUID::INVALID);

        PixelBuffer1DPtr loadPixelBuffer1D(PixelBuffer1DDesc *desc, MemoryType memType, Usage usage, CPUAccessMode accMode, const UUID &uuid = UUID::INVALID);
        
        PixelBuffer2DPtr loadPixelBuffer2D(PixelBuffer2DDesc *desc, MemoryType memType, Usage usage, CPUAccessMode accMode, const UUID &uuid = UUID::INVALID);
        
        PixelBuffer3DPtr loadPixelBuffer3D(PixelBuffer3DDesc *desc, MemoryType memType, Usage usage, CPUAccessMode accMode, const UUID &uuid = UUID::INVALID);

        PixelBuffer2DPtr loadRenderTexture(PixelBuffer2DDesc *desc, MemoryType memType, Usage usage, CPUAccessMode accMode, const UUID &uuid = UUID::INVALID);
        
        TResult GC() override;
        
    protected:
        RenderBufferManager() = default;

        ~RenderBufferManager() override;

        using BufferCache = TMap<UUID, RenderBufferPtr>;
        
        template<typename BUFFER_TYPE, typename CREATOR, typename ...ARGS>
        SmartPtr<BUFFER_TYPE> loadBuffer(BufferCache &buffers, const UUID &uuid, CREATOR creator, ARGS ...args);

        using VertexDeclarations = TMap<uint32_t, VertexDeclarationPtr>;

        /// 顶点缓冲区对象池
        BufferCache mVBufferCache {};
        /// 索引缓冲区对象池
        BufferCache mIBufferCache {};
        /// 像素缓冲区对象池
        BufferCache mPBufferCache {};
        /// 常量缓冲区对象池
        BufferCache mCBufferCache {};
        /// 顶点声明对象池
        VertexDeclarations  mVertexDeclarations {};
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
