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


#ifndef __T3D_FORWARD_RENDER_PIPELINE_H__
#define __T3D_FORWARD_RENDER_PIPELINE_H__


#include "T3DRenderPipeline.h"
#include "Material/T3DPassInstance.h"
#include "Material/T3DShaderConstantParam.h"
#include "Material/T3DShaderVariantInstance.h"


namespace Tiny3D
{
    class T3D_ENGINE_API ForwardRenderPipeline : public RenderPipeline
    {
    public:
        static ForwardRenderPipelinePtr create();

        TResult init() override;
        
        TResult cull(Scene *scene) override;
        
        TResult render(RHIContext *ctx) override;

        TResult addRenderable(Camera *camera, Renderable *renderable) override;

        TResult removeRenderable(Renderable *renderable) override;

        TResult addLight(Light *light) override;

        TResult removeLight(Light *light) override;

        void destroy() override;

    protected:
        using Lights = TUnorderedMap<UUID, Light*, UUIDHash, UUIDEqual>;
        using Renderables = TList<Renderable*>;
        using RenderGroup = TMap<Material*, Renderables>;
        using RenderQueue = TMap<uint32_t, RenderGroup>;
        using CameraRenderQueue = TMap<Camera*, RenderQueue>;
        using Cameras = TList<Camera*>;
        
        TResult setupBatch();
        
        TResult setupMatrices(RHIContext *ctx, Material *material);

        TResult setupLights(RHIContext *ctx, Material *material);

        TResult setupRenderState(RHIContext *ctx, RenderState *renderState);

        TResult setupShaders(RHIContext *ctx, Material *material, PassInstance *pass);

        TResult setupWorldMatrix(RHIContext *ctx, Renderable *renderable, Material *material, PassInstance *pass);

        using SetCBuffer = TResult (RHIContext::*)(uint32_t, const ConstantBuffers &);

        TResult setupShaderConstants(RHIContext *ctx, SetCBuffer setCBuffer, Material *material, ShaderVariantInstance *shader);

        using SetSamplerState = TResult (RHIContext::*)(uint32_t, const Samplers &);
        using SetPixelBuffer = TResult (RHIContext::*)(uint32_t, const PixelBuffers &);

        TResult setupShaderTexSamplers(RHIContext *ctx, SetSamplerState setSamplerState, SetPixelBuffer setPixelBuffer, Material *material, ShaderVariantInstance *shader);

        TResult renderShadowMap(RHIContext *ctx, Camera *camera);

        TResult renderForward(RHIContext *ctx, Camera *camera);
        
    protected:
        enum LightParam
        {
            kMaxPointLights = 4,
            kMaxSpotLights = 4,
        };

        /// 渲染队列
        CameraRenderQueue mRenderQueue {};
        /// 场景中所有相机
        Cameras mCameras {};
        /// 场景中所有光源
        Lights mLights {};

        /// 重要的平行光
        DirectionalLight *mImportantDirLight {nullptr};

        /// 阴影贴图
        RenderTexturePtr mShadowMap {nullptr};

        /// 点光源颜色 + 漫反射强度
        ColorArray mPointLightColor {kMaxPointLights, ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f)};
        /// 点光源位置 + 镜面反射强度
        Vector4Array mPointLightPos {kMaxPointLights, Vector4f::ZERO};
        /// 点光源衰减参数
        Vector4Array mPointLightAttenuation {kMaxPointLights, Vector4f(1.0f, 0.0f, 0.0f, 0.0f)};

        /// 聚光灯颜色 + 漫反射强度
        ColorArray mSpotLightColor {kMaxSpotLights, ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f)};
        /// 聚光灯位置 + 镜面反射强度
        Vector4Array mSpotLightPos {kMaxSpotLights, Vector4f::ZERO};
        /// 聚光灯方向 + 切角 cos 值
        Vector4Array mSpotLightDir {kMaxSpotLights, Vector4f::ZERO};
        /// 聚光灯衰减参数 + 外切角 cos 值
        Vector4Array mSpotLightAttenuation {kMaxSpotLights, Vector4f(1.0f, 0.0f, 0.0f, 0.0f)};
    };
}


#endif  /*__T3D_FORWARD_RENDER_PIPELINE_H__*/
