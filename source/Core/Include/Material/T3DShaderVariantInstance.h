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

#ifndef __T3D_SHADER_VARIANT_INSTANCE_H__
#define __T3D_SHADER_VARIANT_INSTANCE_H__


#include "T3DTypedef.h"


namespace Tiny3D
{
    class T3D_ENGINE_API ShaderVariantInstance : public Object
    {
    public:
        static ShaderVariantInstancePtr create(ShaderVariantPtr shaderVariant);

        virtual ~ShaderVariantInstance() = default;

        ShaderVariantPtr getShaderVariant() const { return mShaderVariant; }

        const ConstantBuffers &getConstantBuffers() const { return mConstantBuffers; }

    protected:
        ShaderVariantInstance(ShaderVariantPtr shaderVariant);
        
    protected:
        /// shader variant 实例对应的 shader variant
        ShaderVariantPtr mShaderVariant {nullptr};
        /// 常量缓冲区
        ConstantBuffers mConstantBuffers {};
    };
}


#endif  /*__T3D_SHADER_VARIANT_INSTANCE_H__*/
