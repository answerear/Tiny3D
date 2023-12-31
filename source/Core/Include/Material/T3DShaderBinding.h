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

#ifndef __T3D_SHADER_BINDING_H__
#define __T3D_SHADER_BINDING_H__


#include "Kernel/T3DConstant.h"


namespace Tiny3D
{
    /**
     * \brief 着色器变量绑定信息
     */
    struct ShaderVariableBinding
    {
        /// 名称
        String      name;
        /// 变量在缓冲区中的偏移
        uint32_t    offset;
        /// 变量大小
        uint32_t    size;
    };

    using ShaderVariableBindings = TMap<String, ShaderVariableBinding>;

    /**
     * \brief 着色器常量绑定信息
     */
    struct ShaderConstantBinding
    {
        /// 名称
        String      name {};
        /// 绑定点
        uint32_t    binding {0};
        /// 缓冲区大小
        uint32_t    size {0};
        /// 常量缓冲区包含的变量列表
        ShaderVariableBindings  variables;
    };

    /**
     * \brief 着色器纹理绑定信息
     */
    struct ShaderTextureBinding
    {
        /// 名称
        String          name {};
        /// 绑定点
        uint32_t        binding {0};
        /// 绑定数量，用于纹理数组
        uint32_t        bindingCount {0};
        /// 纹理类型
        TEXTURE_TYPE    texType {TEXTURE_TYPE::TT_2D};
    };

    /**
     * \brief 着色器纹理采样器绑定信息
     */
    struct ShaderSamplerBinding
    {
        /// 名称
        String      name {};
        /// 绑定点
        uint32_t    binding {0};
    };

    struct ShaderTexSamplerBinding
    {
        ShaderTextureBinding    texBinding {};
        ShaderSamplerBinding    samplerBinding {};
    };

    using ShaderConstantBindings = TMap<String, ShaderConstantBinding>;
    using ShaderTexSamplerBindings = TMap<String, ShaderTexSamplerBinding>;
}


#endif  /*__T3D_SHADER_BINDING_H__*/
