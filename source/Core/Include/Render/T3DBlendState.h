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


#ifndef __T3D_BLEND_STATE_H__
#define __T3D_BLEND_STATE_H__


#include "Render/T3DRenderResource.h"
#include "Render/T3DRenderConstant.h"


namespace Tiny3D
{
    /**
     * \brief 颜色混合状态描述，对应 RHI 混合管线配置
     */
    TSTRUCT()
    struct T3D_ENGINE_API BlendDesc
    {
        enum
        {
            kMaxRenderTarget = T3D_MAX_RENDER_TARGET
        };

        /**
         * \brief 单个渲染目标（MRT 槽位）的混合配置
         */
        TSTRUCT()
        struct T3D_ENGINE_API RTBlendDesc
        {
            /// 是否启用该 RT 的颜色混合
            TPROPERTY()
            bool            BlendEnable = false;
            
            /// RGB 混合的源因子
            TPROPERTY()
            BlendFactor     SrcBlend = BlendFactor::kSrcAlpha;
            
            /// RGB 混合的目标因子
            TPROPERTY()
            BlendFactor     DestBlend = BlendFactor::kOneMinusSrcAlpha;
            
            /// RGB 混合运算
            TPROPERTY()
            BlendOperation  BlendOp = BlendOperation::kAdd;
            
            /// Alpha 混合的源因子
            TPROPERTY()
            BlendFactor     SrcBlendAlpha = BlendFactor::kOne;
            
            /// Alpha 混合的目标因子
            TPROPERTY()
            BlendFactor     DstBlendAlpha = BlendFactor::kZero;
            
            /// Alpha 混合运算
            TPROPERTY()
            BlendOperation  BlendOpAlpha = BlendOperation::kAdd;
            
            /// 颜色通道写掩码（BlendColorWriteMask 组合）
            TPROPERTY()
            uint8_t         ColorMask = kWriteMaskAll;
        };

        /// 是否启用 Alpha-to-Coverage（MSAA 透明抗锯齿）
        TPROPERTY()
        bool    AlphaToCoverageEnable = false;
        
        /// 是否为各 RT 独立配置混合（否则所有 RT 共用 RenderTargetStates[0]）
        TPROPERTY()
        bool    IndependentBlendEnable = false;
        
        /// 各 MRT 槽位的混合配置，最多 kMaxRenderTarget 个
        TPROPERTY()
        RTBlendDesc    RenderTargetStates[kMaxRenderTarget]{};
    };
    
    /**
     * \brief 可加载的 RHI 颜色混合状态资源
     */
    class T3D_ENGINE_API BlendState : public RenderStateResource<BlendDesc, RHIBlendState>
    {
    public:
        /**
         * \brief 创建混合状态对象（尚未创建底层 RHI 资源，需经 RenderResourceManager 加载）
         * \param [in] desc : 混合状态描述
         * \param [in] hash : 缓存哈希；为 0 时由 desc 的 CRC32 自动计算
         * \return 新建的 BlendState 智能指针
         */
        static BlendStatePtr create(const BlendDesc &desc, uint32_t hash = 0);

        /**
         * \brief 返回渲染资源类型标识
         * \return Type::kBlendState
         */
        Type getType() const override;
        
    protected:
        /**
         * \brief 构造混合状态，保存描述与哈希
         * \param [in] hash : 缓存哈希；为 0 时由 desc 自动计算
         * \param [in] desc : 混合状态描述
         */
        BlendState(uint32_t hash, const BlendDesc &desc);

        ~BlendState() override = default;
        
        /**
         * \brief 通过活动 RHI 上下文创建 RHIBlendState
         * \return 始终返回 true
         */
        bool onLoad() override;

        /**
         * \brief 释放 RHI 混合状态引用
         * \return 始终返回 true
         */
        bool onUnload() override;
    };

    template class T3D_ENGINE_API RenderStateResource<BlendDesc, RHIBlendState>;
}


#endif  /*__T3D_BLEND_STATE_H__*/
