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
#include "Material/T3DShaderConstantParam.h"


namespace Tiny3D
{
    class T3D_ENGINE_API ForwardRenderPipeline : public RenderPipeline
    {
    public:
        static ForwardRenderPipelinePtr create();

        TResult cull(Scene *scene) override;
        
        TResult render(RHIContext *ctx) override;

        TResult addRenderable(Camera *camera, Renderable *renderable) override;

        TResult removeRenderable(Renderable *renderable) override;

    protected:
        TResult setupRenderState(RHIContext *ctx, RenderState *renderState);
        
        TResult setupShaderConstants(Material *material, ShaderVariantInstance *vshader, ShaderVariantInstance *hshader, ShaderVariantInstance *dshader, ShaderVariantInstance *gshader, ShaderVariantInstance *pshade);
        
    protected:
        using Renderables = TList<Renderable*>;
        using RenderGroup = TMap<Material*, Renderables>;        
        using RenderQueue = TMap<uint32_t, RenderGroup>;
        using CameraRenderQueue = TMap<Camera*, RenderQueue>;
        using Cameras = TList<Camera*>;
        
        CameraRenderQueue mRenderQueue {};
        Cameras mCameras {};
    };
}


#endif  /*__T3D_FORWARD_RENDER_PIPELINE_H__*/
