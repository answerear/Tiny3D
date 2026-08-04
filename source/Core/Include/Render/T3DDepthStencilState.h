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


#ifndef __T3D_DEPTH_STENCIL_STATE_H__
#define __T3D_DEPTH_STENCIL_STATE_H__


#include "Render/T3DRenderResource.h"
#include "Render/T3DRenderConstant.h"


namespace Tiny3D
{
    /**
     * \brief 单面（正面或背面）的模板测试与模板操作配置
     */
    TSTRUCT()
    struct T3D_ENGINE_API StencilOpDesc
    {
        /// 模板比较函数
        TPROPERTY()
        CompareFunction StencilFunc = CompareFunction::kAlwaysPass;

        /// 模板测试失败时的操作
        TPROPERTY()
        StencilOp       StencilFailOp = StencilOp::kKeep;

        /// 模板测试通过但深度测试失败时的操作
        TPROPERTY()
        StencilOp       StencilDepthFailOp = StencilOp::kKeep;

        /// 模板与深度测试均通过时的操作
        TPROPERTY()
        StencilOp       StencilPassOp = StencilOp::kKeep;
    };

    /**
     * \brief 深度缓冲与模板缓冲状态描述
     */
    TSTRUCT()
    struct T3D_ENGINE_API DepthStencilDesc
    {
        /// 是否启用深度测试
        TPROPERTY()
        bool            DepthTestEnable = true;
        
        /// 是否写入深度缓冲
        TPROPERTY()
        bool            DepthWriteEnable = true;
        
        /// 深度比较函数
        TPROPERTY()
        CompareFunction DepthFunc = CompareFunction::kLess;

        /// 是否启用模板测试
        TPROPERTY()
        bool            StencilEnable = false;
        
        /// 模板参考值（供 StencilOp::kReplace 等使用）
        TPROPERTY()
        uint32_t        StencilRef = 0;
        
        /// 模板读掩码
        TPROPERTY()
        uint8_t         StencilReadMask = 0xFF;
        
        /// 模板写掩码
        TPROPERTY()
        uint8_t         StencilWriteMask = 0xFF;

        /// 正面模板操作配置
        TPROPERTY()
        StencilOpDesc   FrontFace {};
        
        /// 背面模板操作配置
        TPROPERTY()
        StencilOpDesc   BackFace {};
    };

    template class T3D_ENGINE_API RenderStateResource<DepthStencilDesc, RHIDepthStencilState>;
    
    /**
     * \brief 可加载的 RHI 深度/模板状态资源
     */
    class T3D_ENGINE_API DepthStencilState : public RenderStateResource<DepthStencilDesc, RHIDepthStencilState>
    {
    public:
        /**
         * \brief 创建深度/模板状态对象（尚未创建底层 RHI 资源，需经 RenderResourceManager 加载）
         * \param [in] desc : 深度/模板状态描述
         * \param [in] hash : 缓存哈希；为 0 时由 desc 的 CRC32 自动计算
         * \return 新建的 DepthStencilState 智能指针
         */
        static DepthStencilStatePtr create(const DepthStencilDesc &desc, uint32_t hash = 0);
        
        /**
         * \brief 返回渲染资源类型标识
         * \return Type::kBlendState
         */
        Type getType() const override;

    protected:
        /**
         * \brief 构造深度/模板状态，保存描述与哈希
         * \param [in] hash : 缓存哈希；为 0 时由 desc 自动计算
         * \param [in] desc : 深度/模板状态描述
         */
        DepthStencilState(uint32_t hash, const DepthStencilDesc &desc);
        
        ~DepthStencilState() override = default;

        /**
         * \brief 通过活动 RHI 上下文创建 RHIDepthStencilState
         * \return 始终返回 true
         */
        bool onLoad() override;

        /**
         * \brief 释放 RHI 深度/模板状态引用
         * \return 始终返回 true
         */
        bool onUnload() override;
    };
}


#endif  /*__T3D_DEPTH_STENCIL_STATE_H__*/
