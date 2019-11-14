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


#ifndef __T3D_GPU_PROGRAM_H__
#define __T3D_GPU_PROGRAM_H__


#include "T3DResource.h"


namespace Tiny3D
{
    /**
     * @brief 着色器程序参数
     */
    class T3D_ENGINE_API ShaderParam : public Object
    {
    public:
        static ShaderParamPtr create();

        virtual ~ShaderParam();

    protected:
        ShaderParam();
    };

    /**
     * @brief 着色器程序
     */
    class T3D_ENGINE_API Shader : public Resource
    {
    public:
        /**
         * @brief 着色器类型
         */
        enum ShaderType
        {
            E_ST_VERTEX_SHADER = 0, /**< 顶点着色器 */
            E_ST_PIXEL_SHADER,      /**< 像素着色器 */
        };

        /**
         * @brief 获取资源类型，重写基类 Resource::getType() 接口
         */
        virtual Type getType() const override;

        /**
         * @brief 获取着色器类型
         */
        virtual ShaderType getShaderType() const = 0;

        /**
         * @brief 编译着色器程序
         */
        virtual TResult compile() = 0;

        /**
         * @brief 是否已经编译过
         */
        virtual bool hasCompiled() const = 0;

    protected:
        /**
         * @brief 构造函数
         */
        Shader(const String &name);
    };

    /**
     * @brief GPU程序
     */
    class T3D_ENGINE_API GPUProgram : public Resource
    {
    public:
        typedef TMap<Shader::ShaderType, ShaderPtr> Shaders;
        typedef Shaders::iterator                   ShadersItr;
        typedef Shaders::const_iterator             ShadersConstItr;
        typedef Shaders::value_type                 ShadersValue;

        /**
         * @brief 獲取資源類型，重寫基類 Resource::getType() 接口
         */
        virtual Type getType() const override;

        /**
         * @brief 把所有著色器程序鏈接成一個 GPU 程序
         * @param [in] force : 是否強制重新鏈接
         * @return 調用成功返回 true
         */
        virtual TResult link(bool force = false) = 0;

        /**
         * @brief 是否鏈接成GPU程序
         */
        virtual bool hasLinked() const = 0;

        /**
         * @brief 新增 shader
         * @param [in] name : Shader 名稱
         * @param [in] type : Shader 類型
         * @param [in][out] shader : 返回新增的 Shader 對象
         * @return 調用成功返回 T3D_OK
         */
        TResult addShader(
            const String &name, Shader::ShaderType type, ShaderPtr &shader);

        /**
         * @brief 根據名稱移除 shader
         */
        TResult removeShader(const String &name);

        /**
         * @brief 根據類型移除 shader
         */
        TResult removeShader(Shader::ShaderType type);

        /**
         * @brief 获取頂點著色器對象
         */
        ShaderPtr getVertexShader() const;

        /**
         * @brief 获取片段著色器對象
         */
        ShaderPtr getPixelShader() const;

    protected:
        /**
         * @brief 構造函數
         */
        GPUProgram(const String &name);

        /**
         * @brief 克隆属性
         */
        virtual TResult cloneProperties(GPUProgramPtr newObj) const;

    protected:
        Shaders mShaders;   /**< 所有的著色器程序 */
    };
}


#endif  /*__T3D_GPU_PROGRAM_H__*/
