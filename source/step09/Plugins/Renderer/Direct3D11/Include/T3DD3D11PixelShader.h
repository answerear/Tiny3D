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


#ifndef __T3D_D3D11_PIXEL_SHADER_H__
#define __T3D_D3D11_PIXEL_SHADER_H__


#include "T3DD3D11Shader.h"


namespace Tiny3D
{
    /**
     * @brief DirectX 11 像素着色器
     */
    class D3D11PixelShader : public D3D11Shader
    {
    public:
        /**
         * @brief 创建 D3D11PixelShader 对象
         * @param [in] name : Shader名称
         * @param [in] content : 着色器内容，可以为空
         * @return 返回一个新建的D3D11着色器对象
         */
        static D3D11PixelShaderPtr create(
            const String &name, const String &content);

        /**
         * @brief 析构函数
         */
        virtual ~D3D11PixelShader();

        /**
         * @brief 重写 Shader::getShaderType() 接口
         */
        virtual ShaderType getShaderType() const override;

        /**
         * @brief 重写 Shader::compile() 接口
         */
        virtual TResult compile() override;

        /**
         * @fn  ID3D11PixelShader D3D11PixelShader::*getD3DShader() const
         * @brief   Gets d 3D shader
         * @returns Null if it fails, else the d 3D shader.
         */
        ID3D11PixelShader *getD3DShader() const { return mD3DPixelShader; }

    protected:
        /**
         * @brief 构造函数
         */
        D3D11PixelShader(const String &name, const String &content);

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
        ID3D11PixelShader  *mD3DPixelShader;  /**< D3D11 像素着色器对象 */
    };
}


#endif  /*__T3D_D3D11_PIXEL_SHADER_H__*/