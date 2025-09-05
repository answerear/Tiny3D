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
#include "Light/T3DSpotLight.h"
#include "Resource/T3DTextureManager.h"
#include "T3DErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ForwardRenderPipelinePtr ForwardRenderPipeline::create()
    {
        return T3D_NEW ForwardRenderPipeline();
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::init()
    {
        RenderTexturePtr shadowMap = T3D_TEXTURE_MGR.createRenderTexture("__@$ShadowMap$@__", 2048, 2048, PixelFormat::E_PF_D24_UNORM_S8_UINT, 1, 1, 0, true);
        if (shadowMap == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER, "Failed to create shadow map !");
            return T3D_ERR_RENDER_CRATE_SHADOWMAP;
        }

        RenderTexture *colorRT = nullptr;
        mShadowMapRT = RenderTarget::create(colorRT, shadowMap);
        SamplerDesc samplerDesc;
        samplerDesc.MinFilter = FilterOptions::kLinear;
        samplerDesc.MagFilter = FilterOptions::kLinear;
        samplerDesc.MipFilter = FilterOptions::kPoint;
        samplerDesc.AddressU = TextureAddressMode::kBorder;
        samplerDesc.AddressV = TextureAddressMode::kBorder;
        samplerDesc.AddressW = TextureAddressMode::kBorder;
        samplerDesc.CompareFunc = CompareFunction::kLessEqual;
        samplerDesc.IsComparison = true;
        shadowMap->setSamplerDesc(samplerDesc);
        
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    void ForwardRenderPipeline::destroy()
    {
        if (mShadowMapRT != nullptr)
        {
            mShadowMapRT->releaseAllResources();
            mShadowMapRT = nullptr;
        }
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

        
        if (mImportantDirLight == nullptr)
        {
            if (light->getLightType() == LightType::kDirectional)
            {
                // 取第一个方向光作为重要方向光，以产生阴影
                mImportantDirLight = static_cast<DirectionalLight*>(light);
            }
        }
        
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

        mImportantDirLight = nullptr;

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
        TResult ret = T3D_OK;
        for (const auto &camera : mCameras)
        {
            // 渲染阴影贴图
            ret = renderShadowMap(ctx, camera);
            if (T3D_FAILED(ret))
            {
                continue;
            }

            // 前向渲染
            ret = renderForward(ctx, camera);
            if (T3D_FAILED(ret))
            {
                continue;
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::renderShadowMap(RHIContext *ctx, Camera *camera)
    {
        // 只对平行光计算阴影贴图
        ctx->setRenderTarget(mShadowMapRT);

        ctx->clearDepth(1.0f);
        
        if (mImportantDirLight == nullptr)
        {
            // 没有平行光
            ctx->reset();
            return T3D_OK;
        }

        Viewport vp {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
        ctx->setViewport(vp);
        
        // 构造光源的范围
        const Real kShadowDistance = 15.0f;

        const Real kTwo = 2 * REAL_ONE;

        // Transform3D *node = mImportantDirLight->getGameObject()->getComponent<Transform3D>();
        Transform3D *node = static_cast<Transform3D *>(mImportantDirLight->getGameObject()->getTransformNode());
        const Matrix4 &lightMat = node->getLocalToWorldTransform().getAffineMatrix();
        Vector3 lightDir(lightMat[0][2], lightMat[1][2], lightMat[2][2]);
        Vector3 lightPos = node->getLocalToWorldTransform().getTranslation();
        lightPos = lightPos - lightDir * kShadowDistance;
        Vector3 targetPos = lightPos + lightDir * kShadowDistance * kTwo;

        // 把相机放到光源位置，构造 View Matrix
        Matrix4 matView;
#if (T3D_COORDINATION_RH)
        matView.lookAt_RH(lightPos, targetPos, Vector3::UP);
#else
        matView.lookAt_LH(lightPos, targetPos, Vector3::UP);
#endif

        // 平型光，使用正交投影，构造 Projection Matrix
        Real w = kShadowDistance * kTwo;//static_cast<Real>(mShadowMapRT->getDepthStencil()->getWidth());
        Real h = kShadowDistance * kTwo;//static_cast<Real>(mShadowMapRT->getDepthStencil()->getHeight());
        Real zNear = 0.1f;
        Real zFar = kShadowDistance * kTwo;
        Matrix4 matProj;
#if (T3D_COORDINATION_RH)
        matProj.orthographic_RH(w, h, zNear, zFar);
#else
        matProj.orthographic_LH(w, h, zNear, zFar);
#endif

        // 設置 view matrix & projection matrix
        ctx->setViewProjectionTransform(matView, matProj);

        // 经过平台重新计算的 view matrix & projection matrix , 存起来后续 forward pass 使用
        mLightSpaceMatrix = ctx->getProjViewMatrix();

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
                    setupShadowPassMatrices(ctx, material);

                    // 设置光照
                    setupLights(ctx, material);

                    const auto itrPass = tech->getPassInstances().find(ShaderLab::kBuiltinLightModeShadowCaster);
                    if (itrPass == tech->getPassInstances().end())
                    {
                        continue;
                    }
                    
                    PassInstance *pass = itrPass->second;
                    if (pass == nullptr)
                    {
                        continue;
                    }

                    RenderState *renderState = pass->getPass()->getRenderState();
                    if (renderState == nullptr)
                    {
                        // pass 没有设置 render state，使用 technique 的 render state
                        renderState = tech->getTechnique()->getRenderState();
                    }

                    // 设置对应的渲染状态
                    setupRenderState(ctx, renderState);

                    // 设置着色器
                    setupShaders(ctx, material, pass);

                    for (const auto &renderable : renderables)
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
        
        ctx->reset();
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::renderForward(RHIContext *ctx, Camera *camera)
    {
        // Transform3D *xformCamera = camera->getGameObject()->getComponent<Transform3D>();
        Transform3D *xformCamera = static_cast<Transform3D *>(camera->getGameObject()->getTransformNode());
        Vector4 cameraWorldPos(xformCamera->getLocalToWorldTransform().getTranslation(), 1.0f);

        // 设置渲染目标为相机对应纹理
        RenderTarget *rt = camera->getSrcRenderTarget();
        if (rt == nullptr)
        {
            // 直接渲染到屏幕窗口上，而不是渲染到紋理
            rt = camera->getRenderTarget();
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

                    // 设置 shadow map 纹理
                    material->setTexture("shadowMap", mShadowMapRT->getDepthStencil()->getUUID());

                    // 设置 material 对应的矩阵
                    setupForwardPassMatrices(ctx, material);

                    // 设置光照
                    setupLights(ctx, material);

                    // 设置相机世界位置
                    material->setVector("tiny3d_CameraWorldPos", cameraWorldPos);

                    const auto itrPass = tech->getPassInstances().find(ShaderLab::kBuiltinLightModeForwardBase);
                    if (itrPass == tech->getPassInstances().end())
                    {
                        continue;
                    }
                    
                    PassInstance *pass = itrPass->second;
                    if (pass == nullptr)
                    {
                        continue;
                    }

                    RenderState *renderState = pass->getPass()->getRenderState();
                    if (renderState == nullptr)
                    {
                        // pass 没有设置 render state，使用 technique 的 render state
                        renderState = tech->getTechnique()->getRenderState();
                    }

                    // 设置对应的渲染状态
                    setupRenderState(ctx, renderState);

                    // 设置着色器
                    setupShaders(ctx, material, pass);

                    for (const auto &renderable : renderables)
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

        // 把相机渲染纹理渲染到相机对应的渲染目标上
        // if (camera->getRenderTarget()->getType() == RenderTarget::Type::E_RT_WINDOW)
        if (camera->getRenderTarget()->getRenderTexture() != camera->getRenderTexture())
        {
            T3D_ASSERT(rt->getType() == RenderTarget::Type::E_RT_TEXTURE);
            const Viewport &vp = camera->getViewport();
            Real left = Real(rt->getRenderTexture()->getWidth()) * vp.Left;
            Real top = Real(rt->getRenderTexture()->getHeight()) * vp.Top;
            Real width = Real(rt->getRenderTexture()->getWidth()) * vp.Width;
            Real height = Real(rt->getRenderTexture()->getHeight()) * vp.Height;
            Vector3 offset(left, top, 0.0f);
            Vector3 box(width, height, 0.0f);
            
            ctx->blit(rt->getRenderTexture(), camera->getRenderTarget(), offset, box, offset);
            // ctx->blit(rt, camera->getRenderTarget());
        }

        // 重置所有状态
        ctx->reset();
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::setupForwardPassMatrices(RHIContext *ctx, Material *material)
    {
        material->setMatrix("tiny3d_MatrixV", ctx->getViewMatrix());
        material->setMatrix("tiny3d_MatrixP", ctx->getProjMatrix());
        material->setMatrix("tiny3d_MatrixVP", ctx->getProjViewMatrix());
        material->setMatrix("tiny3d_MatrixLightSpaceVP", mLightSpaceMatrix);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::setupShadowPassMatrices(RHIContext *ctx, Material *material)
    {
        material->setMatrix("tiny3d_MatrixLightSpaceVP", ctx->getProjViewMatrix());
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    TResult ForwardRenderPipeline::setupLights(RHIContext *ctx, Material *material)
    {
        int32_t pointLightCount = 0;
        int32_t spotLightCount = 0;
        
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

                    // 颜色和漫反射强度
                    ColorRGBA color = light->getColor();
                    color.alpha() = light->getDiffuseIntensity();
                    material->setColor("tiny3d_DirLightColor", color);
                    // 方向和镜面反射强度
                    // Transform3D *xform = light->getGameObject()->getComponent<Transform3D>();
                    Transform3D *xform = static_cast<Transform3D *>(light->getGameObject()->getTransformNode());
                    const Matrix4 &mat = xform->getLocalTransform().getAffineMatrix();
                    float specularIntensity = light->getDiffuseIntensity();
                    Vector4 dir(mat[0][2], mat[1][2], mat[2][2], specularIntensity);
                    material->setVector("tiny3d_DirLightDir", dir);
                }
                break;
            case LightType::kPoint:
                {
                    if (pointLightCount >= kMaxPointLights)
                    {
                        T3D_LOG_WARNING(LOG_TAG_RENDER, "Too many point lights in scene !");
                        continue;
                    }
                
                    // 设置点光源
                    PointLight *light = static_cast<PointLight *>(item.second);

                    // 颜色和漫反射强度
                    ColorRGBA &lightColor = mPointLightColor[pointLightCount];
                    lightColor.red() = light->getColor().red();
                    lightColor.green() = light->getColor().green();
                    lightColor.blue() = light->getColor().blue();
                    lightColor.alpha() = light->getDiffuseIntensity();

                    // 位置和镜面反射强度
                    Vector4f &lightPos = mPointLightPos[pointLightCount];
                    // Transform3D *xform = light->getGameObject()->getComponent<Transform3D>();
                    Transform3D *xform = static_cast<Transform3D *>(light->getGameObject()->getTransformNode());
                    const Vector3 &pos = xform->getLocalToWorldTransform().getTranslation();
                    lightPos[0] = pos[0];
                    lightPos[1] = pos[1];
                    lightPos[2] = pos[2];
                    lightPos[3] = light->getSpecularIntensity();
                
                    // 光源衰减参数
                    Vector4f &lightAttenuation = mPointLightAttenuation[pointLightCount];
                    lightAttenuation[0] = light->getAttenuationConstant();
                    lightAttenuation[1] = light->getAttenuationLinear();
                    lightAttenuation[2] = light->getAttenuationQuadratic();
                    lightAttenuation[3] = 0.0f;
                    pointLightCount++;
                }
                break;
            case LightType::kSpot:
                {
                    if (spotLightCount >= kMaxSpotLights)
                    {
                        T3D_LOG_WARNING(LOG_TAG_RENDER, "Too many spot lights in scene !");
                        continue;
                    }

                    // 设置聚光灯
                    SpotLight *light = static_cast<SpotLight *>(item.second);

                    // 颜色和漫反射强度
                    ColorRGBA &lightColor = mSpotLightColor[spotLightCount];
                    lightColor.red() = light->getColor().red();
                    lightColor.green() = light->getColor().green();
                    lightColor.blue() = light->getColor().blue();
                    lightColor.alpha() = light->getDiffuseIntensity();

                    // 位置和镜面反射强度
                    Vector4f &lightPos = mSpotLightPos[spotLightCount];
                    // Transform3D *xform = light->getGameObject()->getComponent<Transform3D>();
                    Transform3D *xform = static_cast<Transform3D *>(light->getGameObject()->getTransformNode());
                    const Vector3 &pos = xform->getLocalToWorldTransform().getTranslation();
                    lightPos[0] = pos[0];
                    lightPos[1] = pos[1];
                    lightPos[2] = pos[2];
                    lightPos[3] = light->getSpecularIntensity();

                    // 方向和切角 cos 值
                    Vector4f &lightDir = mSpotLightDir[spotLightCount];
                    const Matrix4 &mat = xform->getLocalTransform().getAffineMatrix();
                    lightDir[0] = mat[0][2];
                    lightDir[1] = mat[1][2];
                    lightDir[2] = mat[2][2];
                    lightDir[3] = Math::cos(light->getInnerCutoffAngle() * 0.5f);

                    // 光源衰减参数
                    Vector4f &attenuation = mSpotLightAttenuation[spotLightCount];
                    attenuation[0] = light->getAttenuationConstant();
                    attenuation[1] = light->getAttenuationLinear();
                    attenuation[2] = light->getAttenuationQuadratic();
                    attenuation[3] = Math::cos(light->getCutoffAngle() * 0.5f);
                
                    spotLightCount++;
                }
                break;
            default:
                break;
            }
        }

        // 点光源
        material->setColorArray("tiny3d_PointLightColor", mPointLightColor);
        material->setVectorArray("tiny3d_PointLightPos", mPointLightPos);
        material->setVectorArray("tiny3d_PointLightAttenuation", mPointLightAttenuation);

        // 聚光灯
        material->setColorArray("tiny3d_SpotLightColor", mSpotLightColor);
        material->setVectorArray("tiny3d_SpotLightPos", mSpotLightPos);
        material->setVectorArray("tiny3d_SpotLightDir", mSpotLightDir);
        material->setVectorArray("tiny3d_SpotLightAttenuation", mSpotLightAttenuation);
        
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
        // Transform3DPtr xformNode = renderable->getGameObject()->getComponent<Transform3D>();
        Transform3D *xformNode = static_cast<Transform3D *>(renderable->getGameObject()->getTransformNode());
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
