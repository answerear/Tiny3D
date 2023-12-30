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
#include "Material/T3DTechnique.h"
#include "Component/T3DCamera.h"
#include "Component/T3DRenderable.h"
#include "Kernel/T3DGameObject.h"
#include "Material/T3DPass.h"
#include "Render/T3DRenderTarget.h"
#include "Render/T3DRenderTexture.h"
#include "RHI/T3DRHIContext.h"
#include "Resource/T3DScene.h"
#include "Resource/T3DMaterial.h"
#include "Resource/T3DShader.h"
#include "Render/T3DVertexDeclaration.h"
#include "Render/T3DVertexBuffer.h"
#include "Render/T3DIndexBuffer.h"
#include "Render/T3DRenderState.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ForwardRenderPipelinePtr ForwardRenderPipeline::create()
    {
        return new ForwardRenderPipeline();
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::addRenderable(Camera *camera, Renderable *renderable)
    {
        auto itCamera = mRenderQueue.find(camera);
        if (itCamera == mRenderQueue.end())
        {
            RenderQueue q;
            const auto rval = mRenderQueue.emplace(camera, q);
            itCamera = rval.first;
        }

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
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::removeRenderable(Renderable *renderable)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::cull(Scene *scene)
    {
        if (scene == nullptr)
        {
            return T3D_ERR_INVALID_PARAM;
        }
        
        mCameras.clear();
        mRenderQueue.clear();
        
        for (auto item : scene->getCameras())
        {
            mCameras.emplace_back(item.second);
            
            for (auto go : scene->getRootGameObjects())
            {
                go->frustumCulling(item.second, this);
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
            // 设置渲染目标为相机对应纹理
            RenderTexturePtr rt = camera->getRenderTexture();
            if (rt != nullptr)
            {
                ctx->setRenderTarget(rt);
            }
            else
            {
                ctx->setRenderTarget(camera->getRenderTarget());
            }

            // 设置 viewport
            ctx->setViewport(camera->getViewport());

            // 清除 color
            ctx->clearColor(camera->getClearColor());
            
            // 清除 depth buffer、stencil buffer
            ctx->clearDepthStencil(camera->getClearDepth(), camera->getClearStencil());

            const auto itr = mRenderQueue.find(camera);

            if (itr != mRenderQueue.end())
            {
                for (auto itemQueue : itr->second)
                {
                    const RenderGroup &group = itemQueue.second;

                    for (auto itemGroup : group)
                    {
                        Material *material = itemGroup.first;
                        const Renderables &renderables = itemGroup.second;

                        ShaderPtr shader = material->getShader();
                        TechniquePtr tech = shader->getCurrentTechnique();

                        // 设置 technique 对应的渲染状态
                        RenderState *renderState = tech->getRenderState();
                        setupRenderState(ctx, renderState);

                        // 遍历渲染每个 Pass
                        for (auto pass : tech->getPasses())
                        {
                            // 设置 pass 对应的渲染状态
                            renderState = pass->getRenderState();
                            setupRenderState(ctx, renderState);

                            ShaderVariant *vertexShader = pass->getCurrentVertexShader();
                            ShaderVariant *hullShader = pass->getCurrentHullShader();
                            ShaderVariant *domainShader = pass->getCurrentDomainShader();
                            ShaderVariant *geometryShader = pass->getCurrentGeometryShader();
                            ShaderVariant *pixelShader = pass->getCurrentPixelShader();
                            
                            // 设置 shader 常量
                            setupShaderConstants(material, vertexShader, hullShader, domainShader, geometryShader, pixelShader);

                            // 设置各 pipeline stage 的 shader
                            ctx->setVertexShader(vertexShader);
                            ctx->setHullShader(hullShader);
                            ctx->setDomainShader(domainShader);
                            ctx->setGeometryShader(geometryShader);
                            ctx->setPixelShader(pixelShader);

                            // 设置使用的纹理

                            for (auto renderable : renderables)
                            {
                                // 设置 vertex declaration
                                ctx->setVertexDeclaration(renderable->getVertexDeclaration());

                                // 设置 vertex buffer
                                ctx->setVertexBuffer(renderable->getVertexBuffer());

                                // 设置 index buffer
                                ctx->setIndexBuffer(renderable->getIndexBuffer());

                                // render
                                ctx->render();
                            }
                        }
                    }
                }
            }

            // 把相机渲染纹理渲染到相机对应的渲染目标上
            if (rt != nullptr)
            {
                const Viewport &vp = camera->getViewport();
                Real left = Real(rt->getWidth()) * vp.Left;
                Real top = Real(rt->getHeight()) * vp.Top;
                Real width = Real(rt->getWidth()) * vp.Width;
                Real height = Real(rt->getHeight()) * vp.Height;
                Vector3 offset(left, top, 0.0f);
                Vector3 box(width, height, 0.0f);
                
                ctx->blit(rt, camera->getRenderTarget(), offset, box, offset);
                // ctx->blit(rt, camera->getRenderTarget());
            }

            // 重置 render target 为空
            ctx->resetRenderTarget();
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::setupRenderState(RHIContext *ctx, RenderState *renderState)
    {
        if (renderState != nullptr)
        {
            // 混合状态
            BlendState *blendState = renderState->getBlendState();
            if (blendState != nullptr)
            {
                ctx->setBlendState(blendState);
            }

            // 光栅化状态
            RasterizerState *rasterState = renderState->getRasterizerState();
            if (rasterState != nullptr)
            {
                ctx->setRasterizerState(rasterState);
            }

            // 深度/模板缓存状态
            DepthStencilState *depthStencilState = renderState->getDepthStencilState();
            if (depthStencilState != nullptr)
            {
                ctx->setDepthStencilState(depthStencilState);
            }
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::setupShaderConstants(Material *material, ShaderVariant *vshader, ShaderVariant *hshader, ShaderVariant *dshader, ShaderVariant *gshader, ShaderVariant *pshader)
    {
        // for (auto param : material->getConstantParams())
        // {
        //     // vertex shader
        //     if (vshader != nullptr)
        //     {
        //         vshader->setParam(param.first, param.second->getData());
        //     }
        //
        //     // hull shader
        //     if (hshader != nullptr)
        //     {
        //         hshader->setParam(param.first, param.second->getData());
        //     }
        //
        //     // domain shader
        //     if (dshader != nullptr)
        //     {
        //         dshader->setParam(param.first, param.second->getData());
        //     }
        //
        //     // geometry shader
        //     if (gshader != nullptr)
        //     {
        //         gshader->setParam(param.first, param.second->getData());
        //     }
        //     
        //     // pixel shader
        //     if (pshader != nullptr)
        //     {
        //         pshader->setParam(param.first, param.second->getData());
        //     }
        // }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
