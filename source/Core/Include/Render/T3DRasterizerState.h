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


#ifndef __T3D_RASTERIZER_STATE_H__
#define __T3D_RASTERIZER_STATE_H__


#include "T3DConfig.h"
#include "Render/T3DRenderResource.h"
#include "Render/T3DRenderConstant.h"


namespace Tiny3D
{
    /**
     * \brief 光栅化状态描述，控制填充模式、剔除、深度偏移等
     */
    TSTRUCT()
    struct T3D_ENGINE_API RasterizerDesc
    {
        /// 多边形填充模式（点/线框/实体）
        TPROPERTY()
        PolygonMode FillMode = PolygonMode::kSolid;
        
        /// 面剔除模式
        TPROPERTY()
        CullingMode CullMode = CullingMode::kBack;

        /// 是否将逆时针三角形视为正面（RH 坐标系默认为 true）
        TPROPERTY()
#if (T3D_COORDINATION_RH)
        bool        FrontAnticlockwise = true;
#else
        bool        FrontAnticlockwise = false;
#endif
        
        /// 深度偏移常量项
        TPROPERTY()
        Real        DepthBias = 0.0f;
        
        /// 深度偏移钳制值
        TPROPERTY()
        Real        DepthBiasClamp = 0.0f;
        
        /// 基于多边形斜率的深度偏移缩放
        TPROPERTY()
        Real        SlopeScaledDepthBias = 0.0f;
        
        /// 是否启用深度裁剪
        TPROPERTY()
        bool        DepthClipEnable = true;
        
        /// 是否启用裁剪矩形（Scissor）
        TPROPERTY()
        bool        ScissorEnable = false;
        
        /// 是否启用多重采样
        TPROPERTY()
        bool        MultisampleEnable = false;
        
        /// 是否对线框启用抗锯齿
        TPROPERTY()
        bool        AntialiasedLineEnable = false;

        /// 是否启用保守光栅化
        TPROPERTY()
        bool        Conservative = false;
    };

    template class T3D_ENGINE_API RenderStateResource<RasterizerDesc, RHIRasterizerState>;
    
    /**
     * \brief 可加载的 RHI 光栅化状态资源
     */
    class T3D_ENGINE_API RasterizerState : public RenderStateResource<RasterizerDesc, RHIRasterizerState>
    {
    public:
        /**
         * \brief 创建光栅化状态对象（尚未创建底层 RHI 资源，需经 RenderResourceManager 加载）
         * \param [in] desc : 光栅化状态描述
         * \param [in] hash : 缓存哈希；为 0 时由 desc 的 CRC32 自动计算
         * \return 新建的 RasterizerState 智能指针
         */
        static RasterizerStatePtr create(const RasterizerDesc &desc, uint32_t hash = 0);
        
        /**
         * \brief 返回渲染资源类型标识
         * \return Type::kBlendState
         */
        Type getType() const override;

    protected:
        /**
         * \brief 构造光栅化状态，保存描述与哈希
         * \param [in] hash : 缓存哈希；为 0 时由 desc 自动计算
         * \param [in] desc : 光栅化状态描述
         */
        RasterizerState(uint32_t hash, const RasterizerDesc &desc);

        ~RasterizerState() override = default;
        
        /**
         * \brief 通过活动 RHI 上下文创建 RHIRasterizerState
         * \return 始终返回 true
         */
        bool onLoad() override;

        /**
         * \brief 释放 RHI 光栅化状态引用
         * \return 始终返回 true
         */
        bool onUnload() override;
    };
}


#endif  /*__T3D_RASTERIZER_STATE_H__*/
