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

#ifndef __T3D_PASS_INSTANCE_H__
#define __T3D_PASS_INSTANCE_H__


#include "T3DTypedef.h"
#include "Material/T3DShaderKeyword.h"


namespace Tiny3D
{
    class T3D_ENGINE_API PassInstance : public Object
    {
    public:
        static PassInstancePtr create(PassPtr pass);

        virtual ~PassInstance() = default;

        PassPtr getPass() const { return mPass; }

        TResult switchKeywords(const StringArray &enableKeys, const StringArray &disableKeys);
        
        ShaderVariantInstance *getCurrentVertexShader() const { return mCurrentVS; }

        ShaderVariantInstance *getCurrentHullShader() const { return mCurrentHS; }

        ShaderVariantInstance *getCurrentDomainShader() const { return mCurrentDS; }

        ShaderVariantInstance *getCurrentGeometryShader() const { return mCurrentGS; }

        ShaderVariantInstance *getCurrentPixelShader() const { return mCurrentPS; }
        
    protected:
        PassInstance(PassPtr pass);

        TResult switchShaderVariants(const ShaderKeyword &keyword, const ShaderVariants &shaderVariants, ShaderVariantInstancePtr &currentVariant);
        
    protected:
        /// pass 实例对应的 pass
        PassPtr             mPass {nullptr};
        
        /// 当前生效的关键字 
        ShaderKeyword       mCurrentKeyword {};
        
        /// 当前生效的 Vertex Shader 变体
        ShaderVariantInstancePtr    mCurrentVS {nullptr};
        /// 当前生效的 Pixel Shader 变体
        ShaderVariantInstancePtr    mCurrentPS {nullptr};
        /// 当前生效的 Geometry Shader 变体
        ShaderVariantInstancePtr    mCurrentGS {nullptr};
        /// 当前生效的 Hull Shader 变体
        ShaderVariantInstancePtr    mCurrentHS {nullptr};
        /// 当前生效的 Domain Shader 变体
        ShaderVariantInstancePtr    mCurrentDS {nullptr};
    };
}


#endif  /*__T3D_PASS_INSTANCE_H__*/
