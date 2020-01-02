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


#ifndef __T3D_PASS_H__
#define __T3D_PASS_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "T3DObject.h"
#include "Kernel/T3DBlendMode.h"
#include "Kernel/T3DCommon.h"
#include "Scene/T3DSceneLight.h"
#include "Resource/T3DGPUProgram.h"
#include "Resource/T3DGPUConstBuffer.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Pass : public Object
    {
    public:
        typedef TArray<GPUConstBufferPtr>       GPUConstBuffers;
        typedef GPUConstBuffers::iterator       GPUConstBuffersItr;
        typedef GPUConstBuffers::const_iterator GPUConstBuffersConstItr;

        typedef TArray<TextureUnitPtr>          TextureUnits;
        typedef TextureUnits::iterator          TextureUnitsItr;
        typedef TextureUnits::const_iterator    TextureUnitsConstItr;

        /**
         * @fn  static PassPtr Pass::create(const String &name, 
         *      Technique *tech);
         * @brief   创建 Pass 对象
         * @param [in]  name    Pass 名称.
         * @param [in]  tech    Pass 所属的 Technique 对象.
         * @return  调用成功返回一个 Pass 对象.
         */
        static PassPtr create(const String &name, Technique *tech);

        /**
         * @fn  virtual Pass::~Pass();
         * @brief   析构函数
         */
        virtual ~Pass();

        /**
         * @fn  const String Pass::&getName() const
         * @brief   獲取 Pass 名稱
         * @returns 返回 Pass 名称.
         */
        const String &getName() const;

        /**
         * @fn  TResult Pass::setGPUProgram(GPUProgramRefPtr program);
         * @brief   设置 Pass 使用的 GPU 程序对象
         * @param [in]  program GPU 程序对象.
         * @returns 调用成功返回 T3D_OK.
         */
        TResult setGPUProgram(GPUProgramRefPtr program);

        /**
         * @fn  GPUProgramPtr Pass::getGPUProgram() const;
         * @brief   获取 GPU 程序对象
         * @returns 返回 GPU 程序对象.
         */
        GPUProgramPtr getGPUProgram() const;

        /**
         * @fn  GPUConstBuffers Pass::getGPUConstBuffers() const;
         * @brief   获取 GPU 程序绑定的所有常量缓冲区对象
         * @returns 返回 GPU 程序绑定的所有常量缓冲区对象.
         */
        GPUConstBuffers getGPUConstBuffers() const;

        /**
         * @fn  GPUConstBufferPtr Pass::getGPUConstBuffer(uint32_t slot) const
         * @brief   Gets GPU constant buffer
         * @param   slot    The slot.
         * @return  The GPU constant buffer.
         */
        GPUConstBufferPtr getGPUConstBuffer(uint32_t slot) const;

        /**
         * @fn  size_t Pass::getGPUConstBufferCount() const;
         * @brief   Gets GPU constant buffer count
         * @returns The GPU constant buffer count.
         */
        size_t getGPUConstBufferCount() const;

        /**
         * @brief   获取纹理单元数量
         */
        size_t getTextureUnitsCount() const;

        /**
         * @brief   是否有顶点着色器
         */
        bool hasVertexShader() const;

        /**
         * @brief   是否有像素着色器
         */
        bool hasPixelShader() const;

        /**
         * @brief   是否有几何着色器
         */
        bool hasGeometryShader() const;

        /**
         * @brief   是否有计算着色器
         */
        bool hasComputeShader() const;

        /**
         * @brief   是否有细分曲面控制着色器
         */
        bool hasHullShader() const;

        /**
         * @brief   是否有细分曲面计算着色器
         */
        bool hasDomainShader() const;

        /**
         * @brief   获取环境光颜色
         */
        const ColorRGBA &getAmbient() const;

        /**
         * @brief   设置环境光颜色
         */
        void setAmbient(const ColorRGBA &color);

        /**
         * @brief   设置环境光颜色
         */
        void setAmbient(Real red, Real green, Real blue);

        /**
         * @brief   获取漫反射光颜色
         */
        const ColorRGBA &getDiffuse() const;

        /**
         * @brief   设置漫反射光颜色
         */
        void setDiffuse(const ColorRGBA &color);

        /**
         * @brief   设置漫反射光颜色
         */
        void setDiffuse(Real red, Real green, Real blue);

        /**
         * @brief   获取镜面反射光颜色
         */
        const ColorRGBA &getSpecular() const;

        /**
         * @brief   设置镜面反射光颜色
         */
        void setSpecular(const ColorRGBA &color);

        /**
         * @brief   设置镜面反射光颜色
         */
        void setSpecular(Real red, Real green, Real blue);

        /**
         * @brief   获取镜面反射的亮度
         */
        Real getShininess() const;

        /**
         * @brief   设置镜面反射的亮度
         */
        void setShininess(Real value);

        /**
         * @brief   获取自发光颜色
         */
        const ColorRGBA &getEmissive() const;

        /**
         * @brief   设置自发光颜色
         */
        void setEmissive(const ColorRGBA &color);

        /**
         * @brief   设置自发光颜色
         */
        void setEmissive(Real red, Real green, Real blue);

        /**
         * @brief   获取顶点颜色用于光的类型
         */
        TrackVertexColorType getVertexColorTracking() const;

        /**
         * @brief   设置顶点颜色用于光的类型
         */
        void setVertexColorTracking(TrackVertexColorType tracking);

        /**
         * @brief   获取源混合因子
         */
        BlendFactor getSourceBlendFactor() const;

        /**
         * @brief   获取目标混合因子
         */
        BlendFactor getDestBlendFactor() const;

        /**
         * @brief   获取 alpha 源混合因子
         */
        BlendFactor getSourceBlendFactorAlpha() const;

        /**
         * @brief   获取 alpha 目标混合因子
         */
        BlendFactor getDestBlendFactorAlpha() const;

        /**
         * @brief   设置混合类型
         */
        void setSceneBlending(BlendType bt);

        /**
         * @brief   设置混合因子
         */
        void setSceneBlending(BlendFactor src, BlendFactor dst);

        /**
         * @brief   分别设置混合类型
         */
        void setSeparateSceneBlending(BlendType bt, BlendType bta);

        /**
         * @brief   分别设置混合因子
         */
        void setSeparateSceneBlending(BlendFactor src, BlendFactor dst, 
            BlendFactor srcAlpha, BlendFactor dstAlpha);

        /**
         * @brief   获取混合操作
         */
        BlendOperation getSceneBlendingOperation() const;

        /**
         * @brief   获取 Alpha 混合操作
         */
        BlendOperation getSceneBlendingOperationAlpha() const;

        /**
         * @brief   设置混合操作
         */
        void setSceneBlendingOperation(BlendOperation op);

        /**
         * @brief   分别设置颜色部分和 Alpha 部分混合操作
         */
        void setSeparateSceneBlendingOperation(BlendOperation op, 
            BlendOperation alphaOp);

        /**
         * @brief   获取是否开启深度检测
         */
        bool isDepthCheckEnabled() const;

        /**
         * @brief   设置是否开启深度检测
         */
        void setDepthCheckEnabled(bool enabled);

        /**
         * @brief   获取是否写深度值
         */
        bool isDepthWriteEnabled() const;

        /**
         * @brief   设置是否写深度值
         */
        void setDepthWriteEnabled(bool enabled);

        /**
         * @brief   获取深度比较函数
         */
        CompareFunction getDepthFunction() const;

        /**
         * @brief   设置深度比较函数
         */
        void setDepthFunction(CompareFunction func);

        /**
         * @brief   获取深度偏移常量
         */
        Real getDepthBiasConstant() const;

        /**
         * @brief   获取深度坡度偏移
         */
        Real getDepthBiasSlopeScale() const;

        /**
         * @brief   设置深度偏移常量和坡度偏移
         * @remarks $$finalBias = maxSlope * slopeScaleBias + constantBias$$
         */
        void setDepthBias(Real constantBias, Real slopeScaleBias = 0.0f);

        /**
         * @brief   获取深度迭代偏移
         */
        Real getDepthBiasPerIteration() const;

        /**
         * @brief   设置深度迭代偏移
         * @remarks $$finalBias = constantBias + biasPerIteration * iteration$$
         */
        void setDepthBiasPerIteration(Real biasPerIteration);

        CompareFunction getAlphaRejectFunction() const;

        void setAlphaRejectFunction(CompareFunction func);

        uint8_t getAlphaRejectValue() const;

        void setAlphaRejectValue(uint8_t val);

        bool isAlphaToCoverageEnabled() const;

        void setAlphaToCoverageEnabled(bool enabled);

        void setAlphaRejectSettings(CompareFunction func, uint8_t val, 
            bool alphaToCoverageEnabled = false);

        bool isLightScissoringEnabled() const;

        void setLightScissoringEnabled(bool enabled);

        bool isLightClipPlanesEnabled() const;

        void setLightClipPlanesEnabled(bool enabled);

        bool isLightEnabled() const;

        void setLightEnabled(bool enabled);

        bool isNormalizeNormalsEnabled() const;

        void setNormalizeNormalsEnabled(bool enabled);

        bool isTransparentSortingEnabled() const;

        void setTransparentSortingEnabled(bool enabled);

        bool isTransparentSortingForced() const;

        void setTransparentSortingForced(bool enabled);

        bool isColorWriteEnabled() const;

        void setColorWriteEnabled(bool enabled);

        bool isPolygonModeOverrideable() const;

        void setPolygonModeOverrideable(bool overrideable);

        CullingMode getCullingMode() const;

        void setCullingMode(CullingMode mode);

        ManualCullingMode getManualCullingMode() const;

        void setManualCullingMode(ManualCullingMode mode);

        IlluminationStage getIlluminationStage() const;

        void setIlluminationStage(IlluminationStage stage);

        ShadingMode getShadingMode() const;

        void setShadingMode(ShadingMode mode);

        PolygonMode getPolygonMode() const;

        void setPolygonMode(PolygonMode mode);

        bool isFogOverrideable() const;

        FogMode getFogMode() const;

        const ColorRGBA &getFogColor() const;

        Real getFogStart() const;

        Real getFogEnd() const;

        Real getFogDensity() const;

        void setFog(bool overrideable, FogMode mode = FogMode::NONE, 
            const ColorRGBA &color = ColorRGBA::WHITE, Real density = 0.001f, 
            Real linearStart = 0.0f, Real linearEnd = 1.0f);

        uint16_t getStartLight() const;

        void setStartLight(uint16_t startLight);

        uint16_t getLightCountPerIteration() const;

        void setLightCountPerIteration(uint16_t val);

        SceneLight::LightType getOnlyLightType() const;

        bool isIteratePerLight() const;

        bool isRunOnlyForOneLightType() const;

        void setIteratePerLight(bool enabled, bool onlyForOneLightType = true,
            SceneLight::LightType lightType = SceneLight::LightType::E_LT_POINT);

        bool isPointSpritesEnabled() const;

        void setPointSpritesEnabled(bool enabled);

        Real getPointSize() const;

        void setPointSize(Real val);

        bool isPointAttenuationEnabled() const;

        Real getPointAttenuationConstant() const;

        Real getPointAttenuationLinear() const;

        Real getPointAttenuationQuadratic() const;

        void setPointAttenuation(bool enabled, Real constant = 0.0f, 
            Real linear = 1.0f, Real quadratic = 0.0f);

        Real getPointMinSize() const;

        void setPointMinSize(Real val);

        Real getPointMaxSize() const;

        void setPointMaxSize(Real val);

    protected:
        /**
         * @fn  Pass::Pass(const String &name, Technique *tech);
         * @brief   构造函数
         * @param [in]  name    The name.
         * @param [in]  tech    If non-null, the technology.
         */
        Pass(const String &name, Technique *tech);

    protected:
        GPUProgramPtr   mGPUProgram;    /**< 绑定到当前 Pass 的 GPU 程序对象 */
        GPUConstBuffers mConstBuffers;  /**< 常量缓冲区对象集合 */
        TextureUnits    mTextureUnits;  /**< TextureUnit 对象集合 */

        Technique   *mParent;   /**< 擁有該 Pass 對象的 Technique 對象 */
        String      mName;      /**< Pass 名稱 */

        //---------------------------------------
        // Command : ambient
        // Usage : ambient <red> <green> <blue> [<alpha>]|vertexColor
        // Parameters : vertexColor 存储在 mTracking
        ColorRGBA   mAmbient;

        //---------------------------------------
        // Command : diffuse
        // Usage : diffuse <red> <green> <blue> [<alpha>]|vertexColor
        // Parameters : vertexColor 存储在 mTracking
        ColorRGBA   mDiffuse;

        //---------------------------------------
        // Command : specular
        // Usage : specular <red> <green> <blue> [<alpha>]|vertexColor <shininess>
        // Parameters : vertexColor 存储在 mTracking
        ColorRGBA   mSpecular;
        Real        mShininess;

        //---------------------------------------
        // Command : emissive
        // Usage : emissive <red> <green> <blue> [<alpha>]|vertexColor
        // Parameters : vertexColor 存储在 mTracking
        ColorRGBA   mEmissive;

        TrackVertexColorType    mTracking;

        //---------------------------------------
        // Command : scene_blend
        // Usage : 
        //  #1 scene_blend <add|modulate|alpha_blend|color_blend>
        //  #2 scene_blend <src_factor> <dest_factor>
        // Parameters : 区分scene_blend和separate_scene_blend，存储在 mSeparateBlend
        BlendFactor mSrcBlendFactor;
        BlendFactor mDstBlendFactor;

        //---------------------------------------
        // Command : separate_scene_blend
        // Usage :
        //  #1 separate_scene_blend add modulate
        //  #2 separate_scene_blend <color_src_factor> <color_dest_factor> <alpha_src_factor> <alpha_dest_factor>
        // Parameters : 
        //  #1 color_src_factor 存储在 mSrcBlendFactor
        //  #2 color_dest_factor 存储在 mDstBlendFactor
        //  #3 区分scene_blend和separate_scene_blend，存储在 mSeparateBlend
        BlendFactor mSrcBlendFactorAlpha;
        BlendFactor mDstBlendFactorAlpha;

        //---------------------------------------
        // Command : scene_blend_op
        // Usage : scene_blend_op <add|subtract|reverse_subtract|min|max>
        // Parameters : 
        //  #1 区分scene_blend_op和separate_scene_blend_op，存储在 mSeparateBlendOperation
        BlendOperation  mBlendOperation;

        //---------------------------------------
        // Command : separate_scene_blend_op
        // Usage : separate_scene_blend_op <colorOp> <alphaOp>
        // Parameters : 
        //  #1 colorOp 存储在 mBlendOperation
        //  #2 区分scene_blend和separate_scene_blend，存储在 mSeparateBlend
        BlendOperation  mAlphaBlendOperation;

        bool    mSeparateBlend;
        bool    mSeparateBlendOperation;

        //---------------------------------------
        // Command : depth_check
        // Usage : depth_check <on|off>
        bool    mDepthCheck;

        //---------------------------------------
        // Command : depth_write
        // Usage : depth_write <on|off>
        bool    mDepthWrite;
        
        //---------------------------------------
        // Command : depth_func
        // Usage : depth_func <always_fail|always_pass|less|less_equal|equal|not_equal|greater_equal|greater>
        CompareFunction mDepthFunc;

        //---------------------------------------
        // Command : depth_bias
        // Usage : depth_bias <constant_bias> [<slopescale_bias>]
        Real   mDepthBiasConstant;
        Real   mDepthBiasSlopeScale;

        //---------------------------------------
        // Command : iteration_depth_bias
        // Usage : iteration_depth_bias <bias_per_iteration>
        Real   mDepthBiasPerIteration;

        //---------------------------------------
        // Command : alpha_rejection
        // Usage : alpha_rejection <function> <value>
        // Parameters : 
        //  #1 <function> 跟 depth_func的参数相同类型
        CompareFunction mAlphaRejectFunc;
        uint8_t         mAlphaRejectVal;

        //---------------------------------------
        // Command : alpha_to_coverage
        // Usage : alpha_to_coverage <on|off>
        bool            mAlpha2CoverageEnabled; /**< / 是否开启A2C */

        //---------------------------------------
        // Command : light_scissor
        // Usage : light_scissor <on|off>
        bool    mLightScissor;  /**< / 是否开启光照裁剪 */

        //---------------------------------------
        // Command : light_clip_planes
        // Usage : light_clip_planes <on|off>
        bool    mLightClipPlanes;   /**< / 是否开启光照裁剪平面 */

        //---------------------------------------
        // Command : lighting
        // Usage : lighting <on|off>
        bool    mLightingEnabled;   /**< / 是否打开光照 */

        //---------------------------------------
        // Command : normalize_normals
        // Usage : normalize_normals <on|off>
        bool    mNormalizeNormals;  /**< / 是否规范化法向量 */

        //---------------------------------------
        // Command : transparent_sorting
        // Usage : transparent_sorting <on|off|force>
        bool    mTransparentSorting;    /**< / 是否半透明纹理排序 */
        bool    mTransparentSortingForced;  /**< / 是否强制半透明纹理排序 */

        //---------------------------------------
        // Command : color_write
        // Usage : color_write <on|off>
        bool    mColorWrite;    /**< 是否写颜色值 */

        //---------------------------------------
        // Command : polygon_mode_overrideable
        // Usage : polygon_mode_overrideable <true|false>
        bool    mPolygonModeOverrideable;   /**< 是否覆盖当前pass的多边形渲染模式 */

        //---------------------------------------
        // Command : cull_hardware
        // Usage : cull_hardware <clockwise|anticlockwise|none>
        CullingMode         mCullMode;  /**< 背面剔除顶点的顺序 */

        //---------------------------------------
        // Command : cull_software
        // Usage : cull_software <back|front|none>
        ManualCullingMode   mManualCullMode;    /**< 软件剔除朝向面 */

        //---------------------------------------
        // Command : illumination_stage
        // Usage illumination_stage <ambient|per_light|decal>
        IlluminationStage   mIlluminationStage; /// 

        //---------------------------------------
        // Command : shading
        // Usage : shading <flat|gouraud|phong>
        ShadingMode           mShadingMode; /**< 着色模式 */

        //---------------------------------------
        // Command : polygon_mode
        // Usage : polygon_mode <solid|wireframe|points>
        PolygonMode         mPolygonMode;   /**< 多边形渲染模式 */

        //---------------------------------------
        // Command : fog_override
        // Usage : fog_override <true|false> [<type> <color> <density> <start> <end>]
        bool        mFogOverride;   /**< 当前pass是否覆盖场景的雾效果设置 */
        FogMode     mFogMode;
        ColorRGBA   mFogColor;
        Real        mFogStart;
        Real        mFogEnd;
        Real        mFogDensity;

        //---------------------------------------
        // Command : start_light
        // Usage : start_light <number>
        uint16_t    mStartLight;

        //---------------------------------------
        // Command : max_lights
        // Usage : max_lights <number>
        uint16_t    mMaxLights;

        //---------------------------------------
        // Command : iteration
        // Usage :
        //  #1 iteration <once|once_per_light> [lightType]
        //  #2 iteration <number> [per_light> [lightType]]
        //  #3 iteration <number> [<per_n_lights> <num_lights> [lightType]]
        uint16_t                mLightsPerIteration;
        SceneLight::LightType   mOnlyLightType;
        bool                    mIteratePerLight;
        bool                    mRunOnlyForOneLightType;

        //---------------------------------------
        // Command : point_sprites
        // Usage : point_sprites <on|off>
        bool        mPointSpritesEnabled;

        //---------------------------------------
        // Command : point_size
        // Usage : point_size <size>
        Real        mPointSize;

        //---------------------------------------
        // Command : point_size_attenuation
        // Usage : point_size_attenuation <on|off> [constant linear quadratic]
        // Parameters :
        //  #1 constant 存储在 mPointAttenuationCoeffs[0]
        //  #2 linear 存储在 mPointAttenuationCoeffs[1]
        //  #3 quadratic 存储在 mPointAttenuationCoeffs[2]
        bool        mPointAttenuationEnabled;
        Real        mPointAttenuationCoeffs[3];

        //---------------------------------------
        // Command : point_size_min
        // Usage : point_size_min <size>
        Real        mPointMinSize;

        //---------------------------------------
        // Command : point_size_max
        // Usage : point_size_max <size>
        Real        mPointMaxSize;
    };
}


#include "T3DPass.inl"


#endif  /*__T3D_PASS_H__*/
