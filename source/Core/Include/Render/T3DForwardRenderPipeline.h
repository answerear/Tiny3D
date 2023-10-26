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
    class T3D_ENGINE_API ForwardRenderPipeline : public RenderPipeline
    {
    public:
        static ForwardRenderPipelinePtr create();
        
        TResult render(RHIContextPtr ctx) override;

    //     TResult attachRenderTarget(RenderTargetPtr target, uint32_t group = DEFAULT_GROUP, uint32_t order = DEFAULT_ORDER) override;
    //
    //     TResult attachRenderTexture(RenderTexturePtr texture) override;
    //     
    //     TResult detachRenderTarget(const String &name) override;
    //
    //     TResult detachAllRenderTargets() override;
    //
    //     RenderTargetPtr getRenderTarget(const String &name) const override;
    //
    // protected:
    //     ForwardRenderPipeline() = default;
    //
    //     ~ForwardRenderPipeline() override = default;
    //
    //     TResult attachRenderTexture(RenderTexturePtr texture, uint32_t group, uint32_t order);
    //
    //     TResult attachRenderWindow(RenderWindowPtr window, uint32_t group, uint32_t order);
    //
    //     void addRenderTarget(RenderTargetPtr target);
    //
    //     void addBindingMap(const String &name, uint32_t group, uint32_t order);
    //     
    //     /// 所有的渲染目标集合
    //     using RenderTargets = TUnorderedMap<String, RenderTargetPtr>;
    //
    //     /// 渲染纹理集合，uint32_t 是 order 既渲染顺序
    //     using RenderTextures = TMap<uint32_t, RenderTexturePtr>;
    //
    //     /**
    //      * \brief 渲染目标分组，每个分组对应一个渲染窗口，以及渲染到该渲染窗口的渲染纹理
    //      */
    //     struct RenderTargetGroup
    //     {
    //         /// 所有的渲染纹理集合
    //         RenderTextures  renderTextures {};
    //         /// 渲染窗口
    //         RenderWindowPtr renderWindow {nullptr};
    //     };
    //
    //     /// 渲染分组集合
    //     using RenderTargetGroups = TMap<uint32_t, RenderTargetGroup>;
    //
    //     /// 渲染目标绑定关系集合，一个渲染目标可以绑定到多个不同的 group 里
    //     using RenderTargetBindings = TUnorderedMap<uint32_t, uint32_t>;
    //
    //     /// 渲染目标名称和分组的绑定映射
    //     using RenderTargetBindingMap = TUnorderedMap<String, RenderTargetBindings>;
    //
    //     /// 所有渲染目标集合
    //     RenderTargets           mRenderTargets {};
    //     /// 渲染顺序用的渲染目标分组的集合
    //     RenderTargetGroups      mRenderTargetGroups {};
    //     /// 渲染目标和渲染分组之间的映射关系
    //     RenderTargetBindingMap  mRenderTargetBindings {};
    //     /// 当前渲染目标
    //     RenderTargetPtr         mCurrentRenderTarget {nullptr};
    };
}


#endif  /*__T3D_FORWARD_RENDER_PIPELINE_H__*/
