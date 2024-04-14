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


#include "Render/T3DForwardRenderPipeline.h"
#include "Material/T3DTechniqueInstance.h"
#include "Material/T3DTechnique.h"
#include "Material/T3DPass.h"
#include "Material/T3DPassInstance.h"
#include "Material/T3DShaderVariant.h"
#include "Material/T3DShaderVariantInstance.h"
#include "Component/T3DCamera.h"
#include "Component/T3DRenderable.h"
#include "Component/T3DTransform3D.h"
#include "Kernel/T3DGameObject.h"
#include "Render/T3DRenderTarget.h"
#include "Render/T3DRenderTexture.h"
#include "RHI/T3DRHIContext.h"
#include "Render/T3DConstantBuffer.h"
#include "Resource/T3DScene.h"
#include "Resource/T3DMaterial.h"
#include "Resource/T3DShader.h"
#include "Render/T3DVertexBuffer.h"
#include "Render/T3DIndexBuffer.h"
#include "Render/T3DRenderState.h"
#include "Render/T3DPixelBuffer.h"



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
        uint32_t queue = material->getCurrentTechnique()->getTechnique()->getRenderQueue();

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

            // 设置 view matrix & projection matrix
            ctx->setViewProjectionTransform(camera->getViewMatrix(), camera->getProjectionMatrix());

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

                        TechniqueInstancePtr tech = material->getCurrentTechnique();

                        // 遍历渲染每个 Pass
                        for (auto pass : tech->getPassInstances())
                        {
                            RenderState *renderState = pass->getPass()->getRenderState();
                            if (renderState != nullptr)
                            {
                                // 设置 pass 对应的渲染状态
                                setupRenderState(ctx, renderState);
                            }
                            else
                            {
                                // 设置 technique 对应的渲染状态
                                renderState = tech->getTechnique()->getRenderState();
                                setupRenderState(ctx, renderState);
                            }

                            ShaderVariantInstance *vertexShader = pass->getCurrentVertexShader();
                            ShaderVariantInstance *hullShader = pass->getCurrentHullShader();
                            ShaderVariantInstance *domainShader = pass->getCurrentDomainShader();
                            ShaderVariantInstance *geometryShader = pass->getCurrentGeometryShader();
                            ShaderVariantInstance *pixelShader = pass->getCurrentPixelShader();
                            
                            // 设置 shader 常量
                            setupShaderConstants(ctx, &RHIContext::setVSConstantBuffers, material, vertexShader);
                            setupShaderConstants(ctx, &RHIContext::setHSConstantBuffers, material, hullShader);
                            setupShaderConstants(ctx, &RHIContext::setDSConstantBuffers, material, domainShader);
                            setupShaderConstants(ctx, &RHIContext::setGSConstantBuffers, material, geometryShader);
                            setupShaderConstants(ctx, &RHIContext::setPSConstantBuffers, material, pixelShader);

                            // 设置 shader 使用的纹理和纹理采样
                            setupShaderTexSamplers(ctx, &RHIContext::setVSSamplers, &RHIContext::setVSPixelBuffers, material, vertexShader);
                            setupShaderTexSamplers(ctx, &RHIContext::setHSSamplers, &RHIContext::setHSPixelBuffers, material, hullShader);
                            setupShaderTexSamplers(ctx, &RHIContext::setDSSamplers, &RHIContext::setDSPixelBuffers, material, domainShader);
                            setupShaderTexSamplers(ctx, &RHIContext::setGSSamplers, &RHIContext::setGSPixelBuffers, material, geometryShader);
                            setupShaderTexSamplers(ctx, &RHIContext::setPSSamplers, &RHIContext::setPSPixelBuffers, material, pixelShader);
                            
                            // 设置各 pipeline stage 的 shader
                            if (vertexShader != nullptr)
                            {
                                ctx->setVertexShader(vertexShader->getShaderVariant());
                            }
                            if (hullShader != nullptr)
                            {
                                ctx->setHullShader(hullShader->getShaderVariant());
                            }
                            if (domainShader != nullptr)
                            {
                                ctx->setDomainShader(domainShader->getShaderVariant());
                            }
                            if (geometryShader != nullptr)
                            {
                                ctx->setGeometryShader(geometryShader->getShaderVariant());
                            }
                            if (pixelShader != nullptr)
                            {
                                ctx->setPixelShader(pixelShader->getShaderVariant());
                            }

                            for (auto renderable : renderables)
                            {
                                // 设置渲染对象的世界变换
                                Transform3DPtr xformNode = renderable->getGameObject()->getComponent<Transform3D>();
                                if (xformNode != nullptr)
                                {
                                    const Transform &xform = xformNode->getLocalToWorldTransform();
                                    ctx->setWorldTransform(xform.getAffineMatrix());
                                }
                                
                                // 设置渲染图元类型
                                ctx->setPrimitiveType(renderable->getPrimitiveType());
                                
                                // 设置 vertex declaration
                                ctx->setVertexDeclaration(renderable->getVertexDeclaration());

                                // 设置 vertex buffer
                                ctx->setVertexBuffers(0, renderable->getVertexBuffers(), renderable->getVertexStrides(), renderable->getVertexOffsets());

                                IndexBuffer *ib = renderable->getIndexBuffer();
                                if (ib != nullptr)
                                {
                                    // 需要索引缓冲区，设置 index buffer
                                    ctx->setIndexBuffer(ib);
                                    
                                    // render
                                    ctx->render(ib->getIndexCount(), 0, 0);
                                }
                                else
                                {
                                    VertexBuffer *vb = renderable->getVertexBuffers().front();
                                    
                                    // 没有索引缓冲区，直接 render
                                    ctx->render(vb->getVertexCount(), 0);
                                }
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

            // 重置所有状态
            ctx->reset();
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

            // 深度/模板缓存状态
            DepthStencilState *depthStencilState = renderState->getDepthStencilState();
            if (depthStencilState != nullptr)
            {
                ctx->setDepthStencilState(depthStencilState);
            }

            // 光栅化状态
            RasterizerState *rasterState = renderState->getRasterizerState();
            if (rasterState != nullptr)
            {
                ctx->setRasterizerState(rasterState);
            }
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::setupShaderConstants(RHIContext *ctx, SetCBuffer setCBuffer, Material *material, ShaderVariantInstance *shader)
    {
        if (material == nullptr || shader == nullptr)
        {
            return T3D_ERR_INVALID_PARAM;
        }

        ConstantBuffers cbuffers(shader->getShaderVariant()->getShaderConstantBindings().size());
        uint32_t i = 0;
        for (const auto &binding : shader->getShaderVariant()->getShaderConstantBindings())
        {
            auto itCB = shader->getConstantBuffers().find(binding.second.name);
            if (itCB == shader->getConstantBuffers().end())
            {
                // 没有对应名字的常量缓冲区
                continue;
            }

            Buffer buffer;
            buffer.Data = new uint8_t[binding.second.size];
            buffer.DataSize = binding.second.size;
                
            for (const auto &param : material->getConstantParams())
            {
                auto itVar = binding.second.variables.find(param.second->getName());
                if (itVar == binding.second.variables.end())
                {
                    // 没有对应名字的变量
                    continue;
                }

                memcpy(buffer.Data + itVar->second.offset, param.second->getData(), itVar->second.size);
            }

            itCB->second->writeData(0, buffer, true);

            cbuffers[i++] = itCB->second;
        }

        if (!cbuffers.empty())
        {
            (ctx->*setCBuffer)(0, cbuffers);
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::setupShaderTexSamplers(RHIContext *ctx, SetSamplerState setSamplerState, SetPixelBuffer setPixelBuffer, Material *material, ShaderVariantInstance *shader)
    {
        if (material == nullptr || shader == nullptr)
        {
            return T3D_ERR_INVALID_PARAM;
        }

        for (const auto &binding : shader->getShaderVariant()->getShaderTexSamplerBindings())
        {
            auto itParam = material->getSamplerParams().find(binding.second.texBinding.name);

            TexturePtr texture = itParam->second->getTexture();
            SamplerState *sampler = texture->getSamplerState();

            PixelBuffers buffers(1);
            buffers[0] = texture->getPixelBuffer();
            (ctx->*setSamplerState)(binding.second.samplerBinding.binding, 1, &sampler);
            (ctx->*setPixelBuffer)(binding.second.texBinding.binding, buffers);
            
            // for (const auto &param : material->getSamplerParams())
            // {
            //     binding.second.samplerBinding.name;
            //     binding.second.samplerBinding.binding;
            //
            //     binding.second.texBinding.name;
            //     binding.second.texBinding.binding;
            //     binding.second.texBinding.bindingCount;
            //     binding.second.texBinding.texType;
            // }
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
