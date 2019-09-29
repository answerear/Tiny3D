/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


namespace Tiny3D
{
    class T3D_ENGINE_API Pass : public Object
    {
    public:
        Pass();

        virtual ~Pass();

    protected:
        String      mName;      /**< Pass名称 */

        ColorARGB   mAmbient;
        ColorARGB   mDiffuse;
        ColorARGB   mSpecular;
        ColorARGB   mEmissive;
        Real        mShininess;

        TrackVertexColorType    mTracking;

        // 混合相关的
        BlendFactor mSrcBlendFactor;
        BlendFactor mDstBlendFactor;
        BlendFactor mSrcBlendFactorAlpha;
        BlendFactor mDstBlendFactorAlpha;

        BlendOperation  mBlendOperation;
        BlendOperation  mAlphaBlendOperation;

        bool    mSeparateBlend;
        bool    mSeparateBlendOperation;

        // Depth buffer 相关的
        bool    mDepthCheck;
        bool    mDepthWrite;
        
        CompareFunction mDepthFunc;

        Real   mDepthBiasConstant;
        Real   mDepthBiasSlopeScale;
        Real   mDepthBiasPerIteration;

        // Alpha rejection 相关
        CompareFunction mAlphaRejectFunc;
        uint8_t         mAlphaRejectVal;
        bool            mAlpha2CoverageEnabled; /// 是否开启A2C

        bool    mLightScissor;                  /// 是否开启光照裁剪
        bool    mLightClipPlanes;               /// 是否开启光照裁剪平面
        bool    mLightingEnabled;               /// 是否打开光照
        bool    mNormalizeNormals;              /// 是否规范化法向量
        bool    mTransparentSorting;            /// 是否半透明纹理排序
        bool    mTransparentSortingForced;      /// 是否强制半透明纹理排序
        bool    mColorWrite;                    /// 是否写颜色值
        bool    mPolygonModeOverrideable;       /// 是否覆盖当前pass的多边形渲染模式

        CullingMode         mCullMode;          /// 背面剔除顶点的顺序
        ManualCullingMode   mManualCullMode;    /// 软件剔除朝向面

        IlluminationStage   mIlluminationStage; /// 

        ShadeMode           mShadeMode;         /// 着色模式
        PolygonMode         mPolygonMode;       /// 多边形渲染模式

        bool        mFogOverride;       /// 当前pass是否覆盖场景的雾效果设置
        FogMode     mFogMode;
        ColorARGB   mFogColor;
        Real        mFogStart;
        Real        mFogEnd;
        Real        mFogDensity;

        uint16_t    mStartLight;
        uint16_t    mMaxLights;
        uint16_t    mLightsPerIteration;
        bool        mIteratePerLight;
        bool        mPointAttenuationEnabled;

        Real        mPointSize;
        Real        mPointMinSize;
        Real        mPointMaxSize;
        Real        mPointSpritesEnabled;
        Real        mPointAttenuationCoeffs[3];
    };
}


#endif  /*__T3D_PASS_H__*/
