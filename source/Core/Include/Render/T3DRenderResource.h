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


#ifndef __T3D_RENDER_RESOURCE_H__
#define __T3D_RENDER_RESOURCE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "RHI/T3DRHIResource.h"


namespace Tiny3D
{
    /**
     * \brief 渲染资源基类，封装 RHI 资源的加载/卸载生命周期
     */
    TCLASS()
    class T3D_ENGINE_API RenderResource : public Object
    {
        friend class RenderResourceManager;
        
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND

    public:
        /**
         * \brief 渲染资源类型标识
         */
        enum class Type : uint32_t
        {
            /// 未分类/初始值
            kNone = 0,
            /// 颜色混合状态
            kBlendState,
            /// 深度/模板测试状态
            kDepthStencilState,
            /// 光栅化状态
            kRasterizerState,
            /// 纹理采样器状态
            kSamplerState,
            /// 顶点缓冲
            kVertexBuffer,
            /// 索引缓冲
            kIndexBuffer,
            /// 一维像素缓冲
            kPixelBuffer1D,
            /// 二维像素缓冲
            kPixelBuffer2D,
            /// 三维像素缓冲
            kPixelBuffer3D,
            /// 立方体贴图像素缓冲
            kPixelBufferCubemap,
            /// 常量缓冲
            kConstantBuffer,
            /// 顶点声明
            kVertexDeclaration,
            // 新增类型一律追加在末尾，避免既有日志里已出现过的数值含义漂移
            /// 结构化缓冲（Structured / ByteAddress / Typed）
            kStructuredBuffer,
        };
        
        ~RenderResource() override = default;

        /**
         * \brief 返回本资源的类型标识
         * \return 资源类型枚举值
         */
        virtual Type getType() const = 0;

        /// 返回已创建的底层 RHI 资源；未加载时为 nullptr
        RHIResourcePtr getRHIResource() const { return mRHIResource; }

    protected:
        RenderResource() = default;

        /**
         * \brief 创建底层 RHI 资源（由 RenderResourceManager::postLoad 调用）
         * \return 加载成功返回 true
         */
        virtual bool onLoad() = 0;

        /**
         * \brief 释放底层 RHI 资源引用（由 RenderResourceManager::preUnload 调用）
         * \return 卸载成功返回 true
         */
        virtual bool onUnload() = 0;
        
        /// 底层 RHI 资源指针，onLoad 创建、onUnload 置空
        RHIResourcePtr mRHIResource {nullptr};
    };
    
    /**
     * \brief 可哈希缓存的渲染状态资源基类，保存描述与 CRC 键
     * \tparam DESC_TYPE : 状态描述结构体类型
     * \tparam RHI_STATE_TYPE : 对应的 RHI 状态类型
     */
    template<typename DESC_TYPE, typename RHI_STATE_TYPE>
    class T3D_ENGINE_API RenderStateResource : public RenderResource
    {
    public:
        using desc_t = DESC_TYPE;
        using rhi_state_t = RHI_STATE_TYPE;

        /// 返回用于缓存查找的 CRC 哈希值
        uint32_t hash() const { return mHash; }

        /// 返回状态描述副本
        const DESC_TYPE &getStateDesc() const { return mDesc; }

        /// 返回类型转换后的 RHI 状态对象；未加载时为 nullptr
        SmartPtr<rhi_state_t> getRHIState() const { return smart_pointer_cast<rhi_state_t>(mRHIResource); }
        
    protected:
        /**
         * \brief 构造状态资源，保存描述与哈希
         * \param [in] hash : 缓存键；为 0 时对 desc 做 CRC32 计算
         * \param [in] desc : 状态描述
         */
        RenderStateResource(uint32_t hash, const desc_t &desc)
            : mHash(hash)
            , mDesc(desc)
        {
            if (mHash == 0)
            {
                mHash = CRC::crc32((uint8_t*)&desc, sizeof(desc));
            }
        }

        ~RenderStateResource() override = default;

        /// 缓存查找键
        uint32_t                mHash {0};
        /// 状态描述
        desc_t                  mDesc {};
    };
}


#endif  /*__T3D_RENDER_RESOURCE_H__*/
