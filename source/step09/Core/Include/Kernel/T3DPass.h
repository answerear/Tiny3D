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

        float32_t   mDepthBiasConstant;
        float32_t   mDepthBiasSlopeScale;
        float32_t   mDepthBiasPerIteration;

        // Alpha rejection 相关
        CompareFunction mAlphaRejectFunc;
        uint8_t         mAlphaRejectVal;
        bool            mAlpha2CoverageEnabled;

        bool    mLightingEnabled;   /// 是否打开光照
    };
}


#endif  /*__T3D_PASS_H__*/
