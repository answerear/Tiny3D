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

        /**
         * @fn  static PassPtr Pass::create(const String &name, 
         *      Technique *tech);
         * @brief   创建 Pass 对象
         * @param           name    The name.
         * @param [in,out]  tech    If non-null, the technology.
         * @return  A PassPtr.
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
         * @returns The name.
         */
        const String &getName() const { return mName; }

        /**
         * @fn  TResult Pass::setGPUProgram(GPUProgramRefPtr program);
         * @brief   Sets program reference
         * @param   program The program.
         * @returns A TResult.
         */
        TResult setGPUProgram(GPUProgramRefPtr program);

        /**
         * @fn  GPUProgramPtr Pass::getGPUProgram() const;
         * @brief   Gets GPU program
         * @returns The GPU program.
         */
        GPUProgramPtr getGPUProgram() const { return mGPUProgram; }

        /**
         * @fn  GPUConstBuffers Pass::getGPUConstBuffers() const;
         * @brief   Gets GPU constant buffers
         * @returns The GPU constant buffers.
         */
        GPUConstBuffers getGPUConstBuffers() const { return mConstBuffers; }

        /**
         * @fn  GPUConstBufferPtr Pass::getGPUConstBuffer(uint32_t slot) const
         * @brief   Gets GPU constant buffer
         * @param   slot    The slot.
         * @return  The GPU constant buffer.
         */
        GPUConstBufferPtr getGPUConstBuffer(uint32_t slot) const
        {
            return mConstBuffers[slot];
        }

        /**
         * @fn  size_t Pass::getGPUConstBufferCount() const;
         * @brief   Gets GPU constant buffer count
         * @returns The GPU constant buffer count.
         */
        size_t getGPUConstBufferCount() const { return mConstBuffers.size(); }

    protected:
        /**
         * @fn  Pass::Pass(const String &name, Technique *tech);
         * @brief   构造函数
         * @param           name    The name.
         * @param [in,out]  tech    If non-null, the technology.
         */
        Pass(const String &name, Technique *tech);

    protected:
        GPUProgramPtr   mGPUProgram;    /**< The GPU program */
        GPUConstBuffers mConstBuffers;  /**< The constant buffers */

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
        bool    mColorWrite;    /**< / 是否写颜色值 */

        //---------------------------------------
        // Command : polygon_mode_overrideable
        // Usage : polygon_mode_overrideable <true|false>
        bool    mPolygonModeOverrideable;   /**< / 是否覆盖当前pass的多边形渲染模式 */

        //---------------------------------------
        // Command : cull_hardware
        // Usage : cull_hardware <clockwise|anticlockwise|none>
        CullingMode         mCullMode;  /**< / 背面剔除顶点的顺序 */

        //---------------------------------------
        // Command : cull_software
        // Usage : cull_software <back|front|none>
        ManualCullingMode   mManualCullMode;    /**< / 软件剔除朝向面 */

        //---------------------------------------
        // Command : illumination_stage
        // Usage illumination_stage <ambient|per_light|decal>
        IlluminationStage   mIlluminationStage; /// 

        //---------------------------------------
        // Command : shading
        // Usage : shading <flat|gouraud|phong>
        ShadeMode           mShadeMode; /**< / 着色模式 */

        //---------------------------------------
        // Command : polygon_mode
        // Usage : polygon_mode <solid|wireframe|points>
        PolygonMode         mPolygonMode;   /**< / 多边形渲染模式 */

        //---------------------------------------
        // Command : fog_override
        // Usage : fog_override <true|false> [<type> <color> <density> <start> <end>]
        bool        mFogOverride;   /**< / 当前pass是否覆盖场景的雾效果设置 */
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
        uint16_t            mLightsPerIteration;
        SceneLight::LightType  mOnlyLightType;
        bool                mIteratePerLight;
        bool                mRunOnlyForOneLightType;

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


#endif  /*__T3D_PASS_H__*/
