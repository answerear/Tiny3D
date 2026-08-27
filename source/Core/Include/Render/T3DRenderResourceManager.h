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
    /**
     * \brief 渲染资源管理器基类，提供加载/卸载辅助与 GC 框架
     */
    class T3D_ENGINE_API RenderResourceManager : public Object
    {
    public:
        ~RenderResourceManager() override = default;

        /**
         * \brief 回收仅被管理器持有的渲染资源
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult GC() = 0;
        
    protected:
        RenderResourceManager() = default;

        /**
         * \brief 调用资源的 onLoad 创建 RHI 对象
         * \param [in] res : 待加载资源；为 nullptr 时返回 false
         * \return onLoad 的返回值；res 为 nullptr 时返回 false
         */
        bool postLoad(RenderResourcePtr res);

        /**
         * \brief 调用资源的 onUnload 释放 RHI 对象
         * \param [in] res : 待卸载资源；为 nullptr 时返回 false
         * \return onUnload 的返回值；res 为 nullptr 时返回 false
         */
        bool preUnload(RenderResourcePtr res);

        /**
         * \brief 遍历容器，卸载并移除 referCount 为 1 的条目
         * \param [in,out] container : 以 hash/UUID 为键的资源缓存容器
         * \return 调用成功返回 T3D_OK
         */
        template<typename CONTAINER_T>
        TResult _GC(CONTAINER_T &container);

        /**
         * \brief 卸载容器中全部资源并清空
         * \param [in,out] container : 资源缓存容器
         */
        template<typename CONTAINER_T>
        void clear(CONTAINER_T &container);
    };

    /**
     * \brief 渲染状态（混合/深度模板/光栅化/采样器）的单例缓存管理器
     */
    class T3D_ENGINE_API RenderStateManager
        : public RenderResourceManager
        , public Singleton<RenderStateManager>
    {
    public:
        /**
         * \brief 创建 RenderStateManager 实例
         * \return 新创建的管理器指针
         */
        static RenderStateManagerPtr create();

        /**
         * \brief 按描述加载或复用混合状态
         * \param [in] desc : 混合状态描述
         * \param [in] hash : 缓存键；为 0 时对 desc 做 CRC32
         * \return 加载成功返回 BlendState；postLoad 失败时返回 nullptr
         */
        BlendStatePtr loadBlendState(const BlendDesc &desc, uint32_t hash = 0);

        /**
         * \brief 按描述加载或复用深度/模板状态
         * \param [in] desc : 深度/模板状态描述
         * \param [in] hash : 缓存键；为 0 时对 desc 做 CRC32
         * \return 加载成功返回 DepthStencilState；postLoad 失败时返回 nullptr
         */
        DepthStencilStatePtr loadDepthStencilState(const DepthStencilDesc &desc, uint32_t hash = 0);

        /**
         * \brief 按描述加载或复用光栅化状态
         * \param [in] desc : 光栅化状态描述
         * \param [in] hash : 缓存键；为 0 时对 desc 做 CRC32
         * \return 加载成功返回 RasterizerState；postLoad 失败时返回 nullptr
         */
        RasterizerStatePtr loadRasterizerState(const RasterizerDesc &desc, uint32_t hash = 0);

        /**
         * \brief 按描述加载或复用采样器状态
         * \param [in] desc : 采样器描述
         * \param [in] hash : 缓存键；为 0 时对 desc 做 CRC32
         * \return 加载成功返回 SamplerState；postLoad 失败时返回 nullptr
         */
        SamplerStatePtr loadSamplerState(const SamplerDesc &desc, uint32_t hash = 0);
        
        /**
         * \brief 依次对四类状态缓存执行 _GC
         * \return 调用成功返回 T3D_OK
         */
        TResult GC() override;
        
    protected:
        RenderStateManager() = default;

        ~RenderStateManager() override;
        
        using StateCache = TMap<uint32_t, RenderResourcePtr>;

        /**
         * \brief 通用状态加载：缓存命中则复用，否则创建并 postLoad
         * \param [in,out] states : 状态缓存容器
         * \param [in] desc : 状态描述
         * \param [in] hash : 缓存键；为 0 时对 desc 做 CRC32
         * \return 加载成功返回状态对象；postLoad 失败时返回 nullptr
         */
        template<typename STATE_TYPE, typename DESC_TYPE> 
        SmartPtr<STATE_TYPE> loadState(StateCache &states, const DESC_TYPE &desc, uint32_t hash = 0);

        /// 混合状态缓存（键为 CRC 哈希）
        StateCache  mBlendStates {};
        /// 深度/模板状态缓存
        StateCache  mDepthStencilStates {};
        /// 光栅化状态缓存
        StateCache  mRasterizerStates {};
        /// 采样器状态缓存
        StateCache  mSamplerStates {};
    };

    #define T3D_RENDER_STATE_MGR (RenderStateManager::getInstance())

    T3D_EXTERN_SINGLETON(RenderStateManager)

    /**
     * \brief 渲染缓冲（顶点/索引/像素/常量）与顶点声明的单例缓存管理器
     */
    class T3D_ENGINE_API RenderBufferManager
        : public RenderResourceManager
        , public Singleton<RenderBufferManager>
    {
    public:
        /**
         * \brief 创建 RenderBufferManager 实例
         * \return 新创建的管理器指针
         */
        static RenderBufferManagerPtr create();

        /**
         * \brief 按属性列表与顶点着色器加载或复用顶点声明
         * \param [in] attributes : 顶点属性数组
         * \param [in] vertexShader : 关联的顶点着色器变体
         * \return 加载成功返回 VertexDeclaration；postLoad 失败时返回 nullptr
         */
        VertexDeclarationPtr addVertexDeclaration(const VertexAttributes &attributes, ShaderVariant *vertexShader);
        
        /**
         * \brief 按 UUID 加载或复用顶点缓冲
         * \param [in] vertexSize : 单个顶点字节数
         * \param [in] vertexCount : 顶点数量
         * \param [in] buffer : 初始 CPU 数据
         * \param [in] memType : 内存类型
         * \param [in] usage : 缓冲用途
         * \param [in] accMode : CPU 访问模式
         * \param [in] uuid : 缓存键；为 UUID::INVALID 时自动生成
         * \return 加载成功返回 VertexBuffer；postLoad 失败时返回 nullptr
         */
        VertexBufferPtr loadVertexBuffer(uint32_t vertexSize, uint32_t vertexCount, 
            const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode, const UUID &uuid = UUID::INVALID);

        /**
         * \brief 按 UUID 加载或复用索引缓冲
         * \param [in] indexType : 索引类型
         * \param [in] indexCount : 索引数量
         * \param [in] buffer : 初始 CPU 数据
         * \param [in] memType : 内存类型
         * \param [in] usage : 缓冲用途
         * \param [in] accMode : CPU 访问模式
         * \param [in] uuid : 缓存键；为 UUID::INVALID 时自动生成
         * \return 加载成功返回 IndexBuffer；postLoad 失败时返回 nullptr
         */
        IndexBufferPtr loadIndexBuffer(IndexType indexType, uint32_t indexCount,
            const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode, const UUID &uuid = UUID::INVALID);

        /**
         * \brief 按 UUID 加载或复用常量缓冲
         * \param [in] name : 常量缓冲名称
         * \param [in] binding : 着色器绑定槽
         * \param [in] buffer : 初始 CPU 数据
         * \param [in] memType : 内存类型
         * \param [in] usage : 缓冲用途
         * \param [in] accMode : CPU 访问模式
         * \param [in] uuid : 缓存键；为 UUID::INVALID 时自动生成
         * \return 加载成功返回 ConstantBuffer；postLoad 失败时返回 nullptr
         */
        ConstantBufferPtr loadConstantBuffer(const String &name, uint32_t binding, const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode, const UUID &uuid = UUID::INVALID);

        /**
         * \brief 按 UUID 加载或复用一维像素缓冲
         * \param [in] desc : 像素缓冲描述
         * \param [in] memType : 内存类型
         * \param [in] usage : 缓冲用途
         * \param [in] accMode : CPU 访问模式
         * \param [in] uuid : 缓存键；为 UUID::INVALID 时自动生成
         * \return 加载成功返回 PixelBuffer1D；postLoad 失败时返回 nullptr
         */
        PixelBuffer1DPtr loadPixelBuffer1D(PixelBuffer1DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode, const UUID &uuid = UUID::INVALID);
        
        /**
         * \brief 按 UUID 加载或复用二维像素缓冲（非渲染纹理）
         * \param [in] desc : 像素缓冲描述
         * \param [in] memType : 内存类型
         * \param [in] usage : 缓冲用途
         * \param [in] accMode : CPU 访问模式
         * \param [in] uuid : 缓存键；为 UUID::INVALID 时自动生成
         * \return 加载成功返回 PixelBuffer2D；postLoad 失败时返回 nullptr
         */
        PixelBuffer2DPtr loadPixelBuffer2D(PixelBuffer2DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode, const UUID &uuid = UUID::INVALID);
        
        /**
         * \brief 按 UUID 加载或复用三维像素缓冲
         * \param [in] desc : 像素缓冲描述
         * \param [in] memType : 内存类型
         * \param [in] usage : 缓冲用途
         * \param [in] accMode : CPU 访问模式
         * \param [in] uuid : 缓存键；为 UUID::INVALID 时自动生成
         * \return 加载成功返回 PixelBuffer3D；postLoad 失败时返回 nullptr
         */
        PixelBuffer3DPtr loadPixelBuffer3D(PixelBuffer3DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode, const UUID &uuid = UUID::INVALID);

        /**
         * \brief 按 UUID 加载或复用立方体贴图像素缓冲
         * \param [in] desc : 像素缓冲描述（二维面参数）
         * \param [in] memType : 内存类型
         * \param [in] usage : 缓冲用途
         * \param [in] accMode : CPU 访问模式
         * \param [in] uuid : 缓存键；为 UUID::INVALID 时自动生成
         * \return 加载成功返回 PixelBufferCubemap；postLoad 失败时返回 nullptr
         */
        PixelBufferCubemapPtr loadPixelBufferCubemap(PixelBuffer2DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode, const UUID &uuid = UUID::INVALID);

        /**
         * \brief 按 UUID 加载或复用渲染纹理（PixelBuffer2D，shaderReadable=true）
         * \param [in] desc : 像素缓冲描述
         * \param [in] memType : 内存类型
         * \param [in] usage : 缓冲用途
         * \param [in] accMode : CPU 访问模式
         * \param [in] uuid : 缓存键；为 UUID::INVALID 时自动生成
         * \return 加载成功返回 PixelBuffer2D；postLoad 失败时返回 nullptr
         */
        PixelBuffer2DPtr loadRenderTexture(PixelBuffer2DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode, const UUID &uuid = UUID::INVALID);

        /**
         * \brief 按 UUID 加载或复用结构化缓冲
         * \param [in] desc : 结构化缓冲描述
         * \param [in] memType : 内存类型
         * \param [in] usage : 缓冲用途；带 kGPUUnorderedAccess 时必须为 Usage::kStatic
         * \param [in] accMode : CPU 访问模式
         * \param [in] gpuAccess : GPU 侧附加访问权限（GPUAccessFlags 组合）
         * \param [in] uuid : 缓存键；为 UUID::INVALID 时自动生成
         * \return 加载成功返回 StructuredBuffer；描述非法或 postLoad 失败时返回 nullptr
         */
        StructuredBufferPtr loadStructuredBuffer(const StructuredBufferDesc &desc, MemoryType memType,
            Usage usage, uint32_t accMode, uint32_t gpuAccess, const UUID &uuid = UUID::INVALID);
        
        /**
         * \brief 依次对顶点声明与各类缓冲缓存执行 _GC
         * \return 调用成功返回 T3D_OK
         */
        TResult GC() override;
        
    protected:
        RenderBufferManager() = default;

        ~RenderBufferManager() override;

        using BufferCache = TMap<UUID, RenderBufferPtr>;
        
        /**
         * \brief 通用缓冲加载：UUID 命中则复用，否则创建并 postLoad
         * \param [in,out] buffers : 缓冲缓存容器
         * \param [in] uuid : 缓存键
         * \param [in] creator : 创建函数
         * \param [in] args : 传递给 creator 的参数
         * \return 加载成功返回缓冲对象；postLoad 失败时返回 nullptr
         */
        template<typename BUFFER_TYPE, typename CREATOR, typename ...ARGS>
        SmartPtr<BUFFER_TYPE> loadBuffer(BufferCache &buffers, const UUID &uuid, CREATOR creator, ARGS ...args);

        using VertexDeclarations = TMap<uint32_t, VertexDeclarationPtr>;

        /// 顶点缓冲缓存（键为 UUID）
        BufferCache mVBufferCache {};
        /// 索引缓冲缓存
        BufferCache mIBufferCache {};
        /// 像素缓冲缓存
        BufferCache mPBufferCache {};
        /// 常量缓冲缓存
        BufferCache mCBufferCache {};
        /// 结构化缓冲缓存
        BufferCache mSBufferCache {};
        /// 顶点声明缓存（键为属性 CRC 哈希）
        VertexDeclarations  mVertexDeclarations {};
    };

    #define T3D_RENDER_BUFFER_MGR   (RenderBufferManager::getInstance())

    T3D_EXTERN_SINGLETON(RenderBufferManager)


    /**
     * \brief 渲染目标管理器占位单例（当前 GC 为空操作）
     */
    class T3D_ENGINE_API RenderTargetManager
        : public RenderResourceManager
        , public Singleton<RenderTargetManager>
    {
    public:
        /**
         * \brief 创建 RenderTargetManager 实例
         * \return 新创建的管理器指针
         */
        static RenderTargetManagerPtr create();

        /**
         * \brief 回收渲染目标资源（当前实现为空操作）
         * \return 调用成功返回 T3D_OK
         */
        TResult GC() override;
        
    protected:
        RenderTargetManager() = default;

        ~RenderTargetManager() override = default;
    };

    T3D_EXTERN_SINGLETON(RenderTargetManager)
}


#endif  /*__T3D_RENDER_RESOURCE_MANAGER_H__*/
