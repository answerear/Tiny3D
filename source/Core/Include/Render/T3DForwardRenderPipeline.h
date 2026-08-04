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
    /**
     * \brief 前向渲染管线：阴影贴图 + 按队列/材质分组的前向光照绘制
     */
    class T3D_ENGINE_API ForwardRenderPipeline : public RenderPipeline
    {
    public:
        /**
         * \brief 创建前向渲染管线实例
         * \return 新建的 ForwardRenderPipeline 智能指针
         */
        static ForwardRenderPipelinePtr create();

        /**
         * \brief 创建 2048×2048 阴影深度 RenderTexture 及对应 RenderTarget，并配置比较采样器
         * \return 调用成功返回 T3D_OK；阴影贴图创建失败时返回 T3D_ERR_RENDER_CRATE_SHADOWMAP
         */
        TResult init() override;
        
        /**
         * \brief 清空内部队列，收集场景相机/光源并对可见 Renderable 做视锥剔除
         * \param [in] scene : 待渲染场景；为 nullptr 时返回 T3D_ERR_INVALID_PARAM
         * \return 调用成功返回 T3D_OK；根 GameObject 为空时仍返回 T3D_OK
         * \note 相机来自场景登记表并额外校验 isEnabled 与 activeInHierarchy；
         *       光源与相机无关，整场景只收集一次
         */
        TResult cull(Scene *scene) override;
        
        /**
         * \brief 对 mCameras 中每台相机依次执行阴影 pass 与前向 pass
         * \param [in] ctx : RHI 渲染上下文
         * \return 最后一台相机的渲染结果；单台失败时跳过并继续
         */
        TResult render(RHIContext *ctx) override;

        /**
         * \brief 将 Renderable 按相机 → 渲染队列 → 材质三级结构归入 mRenderQueue
         * \param [in] camera : 目标相机；首次出现时自动创建空队列
         * \param [in] renderable : 待加入对象
         * \return 调用成功返回 T3D_OK；材质或当前 Technique 为空时静默跳过
         */
        TResult addRenderable(Camera *camera, Renderable *renderable) override;

        /**
         * \brief 从渲染队列移除 Renderable（当前为 TODO 空实现）
         * \param [in] renderable : 待移除对象
         * \return 当前始终返回 T3D_OK
         */
        TResult removeRenderable(Renderable *renderable) override;

        /**
         * \brief 登记光源；首个方向光设为 mImportantDirLight 用于阴影
         * \param [in] light : 待登记光源
         * \return 见 RenderPipeline::addLight
         */
        TResult addLight(Light *light) override;

        /**
         * \brief 从 mLights 移除光源
         * \param [in] light : 待移除光源
         * \return 见 RenderPipeline::removeLight
         */
        TResult removeLight(Light *light) override;

        /**
         * \brief 释放阴影 RT 与天空盒绘制缓存（VB、VertexDeclaration 等）
         */
        void destroy() override;

    protected:
        using Lights = TUnorderedMap<UUID, Light*, UUIDHash, UUIDEqual>;
        using Renderables = TList<Renderable*>;
        using RenderGroup = TMap<Material*, Renderables>;
        using RenderQueue = TMap<uint32_t, RenderGroup>;
        using CameraRenderQueue = TMap<Camera*, RenderQueue>;
        using Cameras = TList<Camera*>;
        
        /**
         * \brief 动态合批预处理（头文件已声明，当前无 .cpp 实现）
         */
        TResult setupBatch();

        /**
         * \brief 为 ShadowCaster Pass 设置光空间 VP 矩阵到材质
         * \param [in] ctx : RHI 上下文（读取当前 ProjView 矩阵）
         * \param [in] material : 目标材质
         * \return 调用成功返回 T3D_OK
         */
        TResult setupShadowPassMatrices(RHIContext *ctx, Material *material);
        
        /**
         * \brief 为 ForwardBase Pass 设置 V/P/VP、光空间 VP 及投影翻转参数
         * \param [in] ctx : RHI 上下文
         * \param [in] material : 目标材质
         * \return 调用成功返回 T3D_OK
         */
        TResult setupForwardPassMatrices(RHIContext *ctx, Material *material);

        /**
         * \brief 遍历 mLights，将环境光/方向光/点光/聚光参数写入材质常量
         * \param [in] ctx : RHI 上下文（当前未直接使用）
         * \param [in] material : 目标材质
         * \return 调用成功返回 T3D_OK
         * \note 点光最多 kMaxPointLights、聚光最多 kMaxSpotLights，超出时打 warning 并跳过
         */
        TResult setupLights(RHIContext *ctx, Material *material);

        /**
         * \brief 将 RenderState 中的混合/深度模板/光栅化状态绑定到 RHI 上下文
         * \param [in] ctx : RHI 上下文
         * \param [in] renderState : 渲染状态；为 nullptr 时不设置任何状态
         * \return 调用成功返回 T3D_OK
         */
        TResult setupRenderState(RHIContext *ctx, RenderState *renderState);

        /**
         * \brief 绑定 Pass 各 stage 的 shader、常量缓冲、采样器与纹理
         * \param [in] ctx : RHI 上下文
         * \param [in] material : 目标材质
         * \param [in] pass : Pass 实例
         * \return 调用成功返回 T3D_OK
         */
        TResult setupShaders(RHIContext *ctx, Material *material, PassInstance *pass);

        /**
         * \brief 设置 Renderable 的世界/逆世界矩阵并刷新 VS 常量缓冲
         * \param [in] ctx : RHI 上下文（当前未直接使用）
         * \param [in] renderable : 可渲染对象
         * \param [in] material : 目标材质
         * \param [in] pass : Pass 实例（用于获取 VS 变体）
         * \return 调用成功返回 T3D_OK；无 Transform3D 时不写矩阵
         */
        TResult setupWorldMatrix(RHIContext *ctx, Renderable *renderable, Material *material, PassInstance *pass);

        using SetCBuffer = TResult (RHIContext::*)(uint32_t, const ConstantBuffers &);

        /**
         * \brief 更新 shader 常量缓冲并绑定到指定 pipeline stage
         * \param [in] ctx : RHI 上下文
         * \param [in] setCBuffer : RHI 常量缓冲绑定函数指针
         * \param [in] material : 目标材质
         * \param [in] shader : shader 变体实例；为 nullptr 时返回 T3D_ERR_INVALID_PARAM
         * \return 调用成功返回 T3D_OK
         */
        TResult setupShaderConstants(RHIContext *ctx, SetCBuffer setCBuffer, Material *material, ShaderVariantInstance *shader);

        using SetSamplerState = TResult (RHIContext::*)(uint32_t, const Samplers &);
        using SetPixelBuffer = TResult (RHIContext::*)(uint32_t, const PixelBuffers &);

        /**
         * \brief 绑定 shader 变体实例中已解析的采样器与 PixelBuffer
         * \param [in] ctx : RHI 上下文
         * \param [in] setSamplerState : RHI 采样器绑定函数指针
         * \param [in] setPixelBuffer : RHI 纹理绑定函数指针
         * \param [in] material : 目标材质
         * \param [in] shader : shader 变体实例；为 nullptr 时返回 T3D_ERR_INVALID_PARAM
         * \return 调用成功返回 T3D_OK
         */
        TResult setupShaderTexSamplers(RHIContext *ctx, SetSamplerState setSamplerState, SetPixelBuffer setPixelBuffer, Material *material, ShaderVariantInstance *shader);

        /**
         * \brief 对指定相机渲染方向光阴影贴图（ShadowCaster Pass）
         * \param [in] ctx : RHI 上下文
         * \param [in] camera : 使用该相机队列中的 Renderable 作为阴影投射体
         * \return 调用成功返回 T3D_OK；无方向光时清空深度后直接返回 T3D_OK
         */
        TResult renderShadowMap(RHIContext *ctx, Camera *camera);

        /**
         * \brief 对指定相机执行前向渲染（ForwardBase Pass），含清屏、天空盒与 blit
         * \param [in] ctx : RHI 上下文
         * \param [in] camera : 目标相机
         * \return 调用成功返回 T3D_OK
         */
        TResult renderForward(RHIContext *ctx, Camera *camera);

        /**
         * \brief 解析本相机使用的天空盒材质
         * \param [in] camera : 目标相机
         * \return 相机 Skybox 组件材质；否则场景根 Skybox 材质；均无则 nullptr
         * \remarks 相机 Skybox 优先于场景全局 Skybox；调用方在无材质时需回退为 SolidColor 清屏
         */
        Material *resolveSkyboxMaterial(Camera *camera) const;

        /**
         * \brief 用全屏三角形绘制天空盒（ForwardBase Pass）
         * \param [in] ctx : RHI 上下文
         * \param [in] camera : 目标相机（提供相机世界位置）
         * \param [in] skyboxMaterial : 天空盒材质；为 nullptr 时直接返回 T3D_OK
         * \return 调用成功返回 T3D_OK；VB 或 VertexDeclaration 创建失败时返回 T3D_ERR_RES_LOAD_FAILED
         * \remarks 在不透明队列之后、透明队列之前调用，被遮挡像素靠已有深度剔除
         */
        TResult renderSkybox(RHIContext *ctx, Camera *camera, Material *skyboxMaterial);
        
    protected:
        /**
         * \brief 光照常量数组容量上限
         */
        enum LightParam : uint32_t
        {
            kMaxPointLights = 4,
            kMaxSpotLights = 4,
        };

        /// 按相机 → 队列 → 材质分组的渲染队列
        CameraRenderQueue mRenderQueue {};
        /// cull 阶段收集的活跃相机列表
        Cameras mCameras {};
        /// cull 阶段收集的场景光源表（UUID → Light*）
        Lights mLights {};

        /// 首个登记的方向光，用于阴影贴图投射
        DirectionalLight *mImportantDirLight {nullptr};

        /// 2048×2048 阴影深度 RenderTarget
        RenderTargetPtr mShadowMapRT {nullptr};
        
        /// 光空间 VP 矩阵（含深度重映射），供 forward pass 阴影采样
        Matrix4 mLightSpaceMatrix { false};

        /// 阴影贴图渲染时投影 Y 是否翻转（OpenGL RTT 路径）
        bool mShadowMapFlipped {false};

        /// 场景根节点上的全局 Skybox 组件
        Skybox *mSceneSkybox {nullptr};
        /// 天空盒全屏三角形 VB，首次绘制时懒加载
        VertexBufferPtr mSkyboxVB {nullptr};
        /// 天空盒顶点声明；InputLayout 依赖 VS 字节码，变体切换时需重建
        VertexDeclarationPtr mSkyboxVertexDecl {nullptr};
        /// 上一次生成 mSkyboxVertexDecl 所用的 VS 变体
        ShaderVariant *mSkyboxVertexDeclShader {nullptr};

        /// 点光源颜色 + 漫反射强度（alpha 通道）
        ColorArray mPointLightColor {kMaxPointLights, ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f)};
        /// 点光源位置 + 镜面反射强度（w 通道）
        Vector4Array mPointLightPos {kMaxPointLights, Vector4f::ZERO};
        /// 点光源衰减参数（constant, linear, quadratic, 0）
        Vector4Array mPointLightAttenuation {kMaxPointLights, Vector4f(1.0f, 0.0f, 0.0f, 0.0f)};

        /// 聚光灯颜色 + 漫反射强度
        ColorArray mSpotLightColor {kMaxSpotLights, ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f)};
        /// 聚光灯位置 + 镜面反射强度
        Vector4Array mSpotLightPos {kMaxSpotLights, Vector4f::ZERO};
        /// 聚光灯方向 + 内切角 cos 值
        Vector4Array mSpotLightDir {kMaxSpotLights, Vector4f::ZERO};
        /// 聚光灯衰减参数 + 外切角 cos 值
        Vector4Array mSpotLightAttenuation {kMaxSpotLights, Vector4f(1.0f, 0.0f, 0.0f, 0.0f)};
    };
}


#endif  /*__T3D_FORWARD_RENDER_PIPELINE_H__*/
