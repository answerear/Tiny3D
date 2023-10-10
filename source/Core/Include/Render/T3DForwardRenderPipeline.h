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


#ifndef __T3D_FORWARD_RENDER_PIPELINE_H__
#define __T3D_FORWARD_RENDER_PIPELINE_H__


#include "T3DRenderPipeline.h"


namespace Tiny3D
{
    class T3D_ENGINE_API ForwardRenderPipeline
        : public RenderPipeline
        , public Singleton<ForwardRenderPipeline>
    {
    public:
        static ForwardRenderPipelinePtr create();
        
        TResult render(RHIContextPtr ctx) override;

        TResult attachRenderTarget(RenderTargetPtr target, uint32_t group = DEFAULT_GROUP, uint32_t order = DEFAULT_ORDER) override;

        TResult detachRenderTarget(const String &name) override;

        RenderTargetPtr getRenderTarget(const String &name) const override;

    protected:
        ForwardRenderPipeline() = default;

        ~ForwardRenderPipeline() override = default;
        
        /// 所有的渲染目标集合
        using RenderTargets = TUnorderedMap<String, RenderTargetPtr>;

        /// 渲染纹理集合
        using RenderTextures = TUnorderedMap<uint32_t, RenderTargetPtr>;

        /**
         * \brief 渲染目标分组，每个分组对应一个渲染窗口，以及渲染到该渲染窗口的渲染纹理
         */
        struct RenderTargetGroup
        {
            /// 所有的渲染纹理集合
            RenderTextures  renderTextures {};
            /// 渲染窗口
            RenderTargetPtr renderWindow {nullptr};
        };

        /// 渲染分组集合
        using RenderTargetGroups = TUnorderedMap<uint32_t, RenderTargetGroup>;

        /**
         * \brief 渲染目标绑定，用于根据渲染名称查找渲染分组和渲染分组里的顺序
         */
        struct RenderTargetBinding
        {
            /// 渲染目标分组
            uint32_t    group;
            /// 渲染目标在分组里面的渲染顺序
            uint32_t    order;
        };

        /// 渲染目标绑定关系集合，一个渲染目标可以绑定到多个不同的 group 里
        using RenderTargetBindings = TList<RenderTargetBinding>;

        /// 渲染目标名称和分组的绑定映射
        using RenderTargetBindingMap = TUnorderedMap<String, RenderTargetBindings>;

        /// 渲染目标列表
        RenderTargets       mRenderTargets;
        /// 当前渲染目标
        RenderTargetPtr     mCurrentRenderTarget;
    };
}


#endif  /*__T3D_FORWARD_RENDER_PIPELINE_H__*/
