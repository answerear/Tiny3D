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
