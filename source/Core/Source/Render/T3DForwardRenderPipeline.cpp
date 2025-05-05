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
#include "Material/T3DShaderVariantInstance.h"
#include "Component/T3DCamera.h"
#include "Light/T3DLight.h"
#include "Component/T3DRenderable.h"
#include "Component/T3DTransform3D.h"
#include "Kernel/T3DGameObject.h"
#include "Render/T3DRenderTarget.h"
#include "Render/T3DRenderTexture.h"
#include "RHI/T3DRHIContext.h"
#include "Resource/T3DScene.h"
#include "Resource/T3DMaterial.h"
#include "Render/T3DVertexBuffer.h"
#include "Render/T3DIndexBuffer.h"
#include "Render/T3DRenderState.h"
#include "Light/T3DAmbientLight.h"
#include "Light/T3DDirectionalLight.h"
#include "Light/T3DPointLight.h"


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
        // TODO:
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::addLight(Light *light)
    {
        if (light == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER, "Invalid light when adding light !");
            return T3D_ERR_INVALID_PARAM;
        }
        
        const auto itr = mLights.find(light->getUUID());
        if (itr != mLights.end())
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER, "Light in game object [%s] already exists when adding light !", light->getGameObject()->getName().c_str());
            return T3D_ERR_DUPLICATED_ITEM;
        }

        mLights.emplace(light->getUUID(), light);
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::removeLight(Light *light)
    {
        if (light == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER, "Invalid light when removing light !");
            return T3D_ERR_INVALID_PARAM;
        }

        const auto itr = mLights.find(light->getUUID());
        if (itr == mLights.end())
        {
            return T3D_ERR_NOT_FOUND;
        }
        
        mLights.erase(itr);
        
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
        mLights.clear();

        int32_t visitTimes = 0;
        
        GameObjectPtr go;
        
#if defined (T3D_EDITOR)
        Camera *camera = scene->getEditorCamera();
        mCameras.emplace_back(camera);
        go = scene->getEditorGameObject();
        go->frustumCulling(camera, this);
#endif
        
        for (auto item : scene->getCameras())
        {
            mCameras.emplace_back(item.second);

            go = scene->getRootGameObject();
            uint32_t cullingMask = item.second->getCullingMask();
            uint32_t cameraMask = go->getCameraMask();
            if (cullingMask & cameraMask)
            {
                go->frustumCulling(item.second, this);
            }

            if (visitTimes == 0)
            {
                // 添加光源，只做一次
                go->setupLights(this);
            }

            visitTimes++;
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::render(RHIContext *ctx)
    {
        // 先逐个渲染到纹理，然后最后按照顺序把渲染到纹理 blit 到渲染窗口
        for (auto camera : mCameras)
        {
            Transform3D *xformCamera = camera->getGameObject()->getComponent<Transform3D>();
            Vector4 cameraWorldPos(xformCamera->getLocalToWorldTransform().getTranslation(), 1.0f);

            // 设置渲染目标为相机对应纹理
            RenderTexturePtr rt = camera->getRenderTexture();
            if (rt == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_RENDER, "Invalid render texture in camera [%s] when rendering !", camera->getGameObject()->getName().c_str());
                continue;
            }
            ctx->setRenderTarget(rt);
            
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

                        // 设置 material 对应的矩阵
                        setupMatrices(ctx, material);

                        // 设置光照
                        setupLights(ctx, material);

                        // 设置相机世界位置
                        material->setVector("tiny3d_CameraWorldPos", cameraWorldPos);

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
                            
                            // 设置着色器
                            setupShaders(ctx, material, pass);

                            for (auto renderable : renderables)
                            {
                                // 设置渲染对象的世界变换
                                setupWorldMatrix(ctx, renderable, material, pass);
                                
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
            // if (camera->getRenderTarget()->getType() == RenderTarget::Type::E_RT_WINDOW)
            if (camera->getRenderTarget()->getRenderTexture() != camera->getRenderTexture())
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

    TResult ForwardRenderPipeline::setupMatrices(RHIContext *ctx, Material *material)
    {
        material->setMatrix("tiny3d_MatrixV", ctx->getViewMatrix());
        material->setMatrix("tiny3d_MatrixP", ctx->getProjMatrix());
        material->setMatrix("tiny3d_MatrixVP", ctx->getProjViewMatrix());
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::setupLightParams(Material *material, LocalLight *light)
    {
        // 漫反射颜色
        material->setColor("tiny3d_LightDiffuseColor", light->getDiffuseColor());
        // 镜面反射颜色
        material->setColor("tiny3d_LightSpecularColor", light->getSpecularColor());
        // 漫反射强度、镜面反射强度、镜面反射发光值
        Vector4 params[2];
        params[0].x() = light->getDiffuseIntensity(); // 漫反射强度
        params[0].y() = light->getSpecularIntensity(); // 镜面反射强度
        params[0].z() = light->getSpecularShininess(); // 镜面反射发光值
        params[0].w() = 0.0f;
        if (light->getLightType() == LightType::kDirectional)
        {
            // 平行光
        }
        else if (light->getLightType() == LightType::kPoint)
        {
            // 点光源
            PointLight *pointLight = static_cast<PointLight *>(light);
            params[1].x() = pointLight->getAttenuationConstant(); // 衰减常量系数
            params[1].y() = pointLight->getAttenuationLinear(); // 衰减一次系数
            params[1].z() = pointLight->getAttenuationQuadratic(); // 衰减二次系数
            params[1].w() = 0.0f;
        }
        
        Vector4Array paramsArray(params, params+2);
        material->setVectorArray("tiny3d_LightParams", paramsArray);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::setupLights(RHIContext *ctx, Material *material)
    {
        for (const auto &item : mLights)
        {
            switch (item.second->getLightType())
            {
            case LightType::kAmbient:
                {
                    // 设置环境光
                    AmbientLight *light = static_cast<AmbientLight *>(item.second);
                    ColorRGBA color = light->getColor();
                    color.alpha() = light->getIntensity();
                    material->setColor("tiny3d_AmbientLight", color);
                }
                break;
            case LightType::kDirectional:
                {
                    // 设置方向光
                    DirectionalLight *light = static_cast<DirectionalLight *>(item.second);
                    // 方向
                    Transform3D *xform = light->getGameObject()->getComponent<Transform3D>();
                    const Matrix4 &mat = xform->getLocalTransform().getAffineMatrix();
                    Vector4 dir(mat[2][0], mat[2][1], mat[2][2], 0.0f);
                    material->setVector("tiny3d_LightPos", dir);

                    // 光源参数
                    setupLightParams(material, light);
                }
                break;
            case LightType::kPoint:
                {
                    // 设置点光源
                    PointLight *light = static_cast<PointLight *>(item.second);

                    // 光源位置
                    Transform3D *xform = light->getGameObject()->getComponent<Transform3D>();
                    const Vector3 &pos = xform->getLocalToWorldTransform().getTranslation();
                    Vector4 lightPos(pos[0], pos[1], pos[2], 1.0f);
                    material->setVector("tiny3d_LightPos", lightPos);
                
                    // 光源参数
                    setupLightParams(material, light);
                }
                break;
            case LightType::kSpot:
                break;
            default:
                break;
            }
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

    TResult ForwardRenderPipeline::setupWorldMatrix(RHIContext *ctx, Renderable *renderable, Material *material, PassInstance *pass)
    {
        Transform3DPtr xformNode = renderable->getGameObject()->getComponent<Transform3D>();
        if (xformNode != nullptr)
        {
            const Transform &xform = xformNode->getLocalToWorldTransform();
            // ctx->setWorldTransform(xform.getAffineMatrix());
            material->setMatrix("tiny3d_ObjectToWorld", xform.getAffineMatrix());
            Matrix4 matWorld2Obj = xform.getAffineMatrix().inverse();
            material->setMatrix("tiny3d_WorldToObject", matWorld2Obj);

            uint32_t startSlot = 0;
            ShaderVariantInstance *vertexShader = pass->getCurrentVertexShader();
            if (vertexShader != nullptr)
            {
                vertexShader->updateConstantBuffers(startSlot);
            }
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::setupShaders(RHIContext *ctx, Material *material, PassInstance *pass)
    {
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

        return T3D_OK;
    }
    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::setupShaderConstants(RHIContext *ctx, SetCBuffer setCBuffer, Material *material, ShaderVariantInstance *shader)
    {
        if (material == nullptr || shader == nullptr)
        {
            return T3D_ERR_INVALID_PARAM;
        }

#if 0
        ConstantBuffers cbuffers;
        cbuffers.reserve(shader->getShaderVariant()->getShaderConstantBindings().size());
        uint32_t i = 0;
        for (const auto &binding : shader->getShaderVariant()->getShaderConstantBindings())
        {
            auto itCB = shader->getConstantBuffers().find(binding.second.name);
            if (itCB == shader->getConstantBuffers().end())
            {
                // 没有对应名字的常量缓冲区
                T3D_LOG_WARNING(LOG_TAG_RENDER, "No constant buffer : %s !", binding.second.name.c_str());
                continue;
            }

            if (material->getConstantParams().empty())
            {
                T3D_LOG_WARNING(LOG_TAG_RENDER, "No constant variables in material : %s !", material->getName().c_str());
                continue;
            }
            
            Buffer buffer;
            buffer.Data = new uint8_t[binding.second.size];
            buffer.DataSize = binding.second.size;

            bool hasCBuffer = false;

            for (const auto &param : material->getConstantParams())
            {
                auto itVar = binding.second.variables.find(param.second->getName());
                if (itVar == binding.second.variables.end())
                {
                    // 没有对应名字的变量
                    // T3D_LOG_WARNING(LOG_TAG_RENDER, "There is not variable : %s !", param.second->getName().c_str());
                    continue;
                }

                memcpy(buffer.Data + itVar->second.offset, param.second->getData(), itVar->second.size);
                hasCBuffer = true;
            }

            if (hasCBuffer)
            {
                itCB->second->writeData(0, buffer, true);

                // cbuffers[i++] = itCB->second;
                cbuffers.emplace_back(itCB->second);
            }
            else
            {
                buffer.release();
            }
        }

        if (!cbuffers.empty())
        {
            (ctx->*setCBuffer)(0, cbuffers);
        }
#else
        uint32_t startSlot = 0;
        shader->updateConstantBuffers(startSlot);
        
        if (!shader->getConstantBuffers().empty())
        {
            (ctx->*setCBuffer)(startSlot, shader->getConstantBuffers());
        }
#endif
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::setupShaderTexSamplers(RHIContext *ctx, SetSamplerState setSamplerState, SetPixelBuffer setPixelBuffer, Material *material, ShaderVariantInstance *shader)
    {
        if (material == nullptr || shader == nullptr)
        {
            return T3D_ERR_INVALID_PARAM;
        }

#if 0
        for (const auto &binding : shader->getShaderVariant()->getShaderTexSamplerBindings())
        {
            auto itParam = material->getSamplerParams().find(binding.second.texBinding.name);
            T3D_ASSERT(itParam != material->getSamplerParams().end());
            
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
#else
        if (!shader->getSamplers().empty())
        {
            uint32_t startSlot = shader->getSamplerStartSlot();
            (ctx->*setSamplerState)(startSlot, shader->getSamplers());

            startSlot = shader->getPixelBufferStartSlot();
            (ctx->*setPixelBuffer)(startSlot, shader->getPixelBuffers());
        }
#endif
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
