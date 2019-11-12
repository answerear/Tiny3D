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


#ifndef __T3D_D3D11_VERTEX_SHADER_H__
#define __T3D_D3D11_VERTEX_SHADER_H__


#include "T3DD3D11Shader.h"


namespace Tiny3D
{
    /**
     * @brief DirectX 11 顶点着色器
     */
    class D3D11VertexShader : public D3D11Shader
    {
    public:
        /**
         * @brief 创建 D3D11VertexShader 对象
         * @param [in] name : Shader 名称
         * @param [in] content : 着色器内容，可以为空
         * @return 返回一个新建的D3D11着色器对象
         */
        static D3D11VertexShaderPtr create(
            const String &name, const String &content);

        /**
         * @brief 析构函数
         */
        virtual ~D3D11VertexShader();

        /**
         * @brief 重写 Shader::getShaderType() 接口
         */
        virtual ShaderType getShaderType() const override;

        /**
         * @brief 重写 Shader::compile() 接口
         */
        virtual TResult compile() override;

    protected:
        /**
         * @brief 构造函数
         */
        D3D11VertexShader(const String &name, const String &content);

        /**
         * @brief 重写 Resource::load() 接口
         */
        virtual TResult load() override;

        /**
         * @brief 重写 Resource::unload() 接口
         */
        virtual TResult unload() override;

        /**
         * @brief 重写 Resource::clone() 接口
         */
        virtual ResourcePtr clone() const override;

    protected:
        ID3D11VertexShader  *mD3DVertexShader;  /**< D3D11 顶点着色器对象 */
    };
}


#endif  /*__T3D_D3D11_VERTEX_SHADER_H__*/