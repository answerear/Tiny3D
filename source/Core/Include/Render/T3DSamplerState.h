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


#ifndef __T3D_SAMPLER_STATE_H__
#define __T3D_SAMPLER_STATE_H__


#include "Render/T3DRenderResource.h"
#include "Render/T3DRenderConstant.h"


namespace Tiny3D
{
    /**
     * \brief 纹理采样器状态描述，对应 RHI 采样器配置
     */
    TSTRUCT()
    struct T3D_ENGINE_API SamplerDesc
    {
        /// 缩小过滤方式
        TPROPERTY()
        FilterOptions       MinFilter {FilterOptions::kLinear};
        
        /// 放大过滤方式
        TPROPERTY()
        FilterOptions       MagFilter {FilterOptions::kLinear};
        
        /// Mipmap 过滤方式
        TPROPERTY()
        FilterOptions       MipFilter {FilterOptions::kLinear};
        
        /// U 方向寻址模式
        TPROPERTY()
        TextureAddressMode  AddressU {TextureAddressMode::kWrap};
        
        /// V 方向寻址模式
        TPROPERTY()
        TextureAddressMode  AddressV {TextureAddressMode::kWrap};
        
        /// W 方向寻址模式
        TPROPERTY()
        TextureAddressMode  AddressW {TextureAddressMode::kWrap};
        
        /// Mipmap LOD 偏移
        TPROPERTY()
        Real                MipLODBias {0.0f};
        
        /// 各向异性过滤最大采样数
        TPROPERTY()
        uint32_t            MaxAnisotropy {1};
        
        /// 比较采样函数（IsComparison 为 true 时生效）
        TPROPERTY()
        CompareFunction     CompareFunc {CompareFunction::kAlwaysFail};
        
        /// 边界颜色
        TPROPERTY()
        ColorRGBA           BorderColor {ColorRGBA::WHITE};
        
        /// 最小 LOD 限制
        TPROPERTY()
        Real                MinLOD {0.0f};
        
        /// 最大 LOD 限制
        TPROPERTY()
        Real                MaxLOD {FLT_MAX};

        /// 是否启用比较采样（阴影贴图等）
        TPROPERTY()
        bool                IsComparison {false};
    };

    template class T3D_ENGINE_API RenderStateResource<SamplerDesc, RHISamplerState>;
    
    /**
     * \brief 可加载的 RHI 纹理采样器状态资源
     */
    class T3D_ENGINE_API SamplerState : public RenderStateResource<SamplerDesc, RHISamplerState>
    {
    public:
        /**
         * \brief 创建采样器状态对象（尚未创建底层 RHI 资源，需经 RenderResourceManager 加载）
         * \param [in] desc : 采样器描述
         * \param [in] hash : 缓存键；为 0 时对 desc 做 CRC32
         * \return 新创建的 SamplerState 指针
         */
        static SamplerStatePtr create(const SamplerDesc &desc, uint32_t hash = 0);
        
        /**
         * \brief 返回渲染资源类型标识
         * \return 资源类型枚举值
         */
        Type getType() const override;
        
    protected:
        /**
         * \brief 构造采样器状态，保存描述与哈希
         * \param [in] hash : 缓存键
         * \param [in] desc : 采样器描述
         */
        SamplerState(uint32_t hash, const SamplerDesc &desc);

        ~SamplerState() override = default;
        
        /**
         * \brief 通过活动 RHI 上下文创建 RHISamplerState
         * \return 始终返回 true
         */
        bool onLoad() override;

        /**
         * \brief 释放 RHI 采样器状态引用
         * \return 始终返回 true
         */
        bool onUnload() override;
    };
}


#endif  /*__T3D_SAMPLER_STATE_H__*/
