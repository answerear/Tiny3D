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


#include "Render/T3DForwardRenderPipeline.h"
#include "Component/T3DRenderable.h"
#include "Kernel/T3DGameObject.h"
#include "Render/T3DRenderTarget.h"
#include "Render/T3DRenderWindow.h"
#include "Render/T3DRenderTexture.h"
#include "RHI/T3DRHIContext.h"
#include "Resource/T3DScene.h"
#include "Component/T3DTransformNode.h"
#include "Component/T3DRenderable.h"
#include "Resource/T3DMaterial.h"
#include "Resource/T3DShader.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ForwardRenderPipelinePtr ForwardRenderPipeline::create()
    {
        return new ForwardRenderPipeline();
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::cull(Scene *scene)
    {
        for (auto item : scene->getCameras())
        {
            mCameras.emplace_back(item.second);
            
            for (auto go : scene->getRootGameObjects())
            {
                if (!go->frustumCulling(item.second))
                {
                    auto itCamera = mRenderQueue.find(item.second);
                    if (itCamera == mRenderQueue.end())
                    {
                        RenderQueue q;
                        const auto rval = mRenderQueue.emplace(item.second, q);
                        itCamera = rval.first;
                    }
                    
                    RenderablePtr renderable = go->getComponent<Renderable>();

                    Material *material = renderable->getMaterial();
                    uint32_t queue = material->getShader()->getCurrentTechnique()->getRenderQueue();
                    
                    auto itr = itCamera->second.find(queue);
                    
                    if (itr == itCamera->second.end())
                    {
                        // 没有对应的渲染队列
                        RenderGroup group;
                        Renderables renderables;
                        renderables.emplace_back(renderable);
                        group.emplace(material, renderables);
                        itCamera->second.emplace(queue, group);
                    }
                    else
                    {
                        // 已经存在对应的渲染队列
                        auto it = itr->second.find(material);
                        
                        if (it == itr->second.end())
                        {
                            // 没有对应的材质
                            Renderables renderables;
                            renderables.emplace_back(renderable);
                            itr->second.emplace(material, renderables);
                        }
                        else
                        {
                            // 已有材质，合并 draw call
                            it->second.emplace_back(renderable);
                        }
                    }
                }
            }
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::render(RHIContext *ctx)
    {
        // 先逐个渲染到纹理，然后最后按照顺序把渲染到纹理 blit 到渲染窗口
        for (auto camera : mCameras)
        {
            const auto itr = mRenderQueue.find(camera);
            
            if (itr != mRenderQueue.end())
            {
                
            }
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
}
