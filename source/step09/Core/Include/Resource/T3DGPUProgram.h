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
     * @brief 着色器程序
     */
    class T3D_ENGINE_API Shader : public Resource
    {
    public:
        /**
         * @brief 获取资源类型，重写基类 Resource::getType() 接口
         */
        virtual Type getType() const override;

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
        /**
         * @brief 获取资源类型，重写基类 Resource::getType() 接口
         */
        virtual Type getType() const override;

        /**
         * @brief 把着色器链接成一个GPU程序
         */
        virtual TResult link(ShaderPtr vertexShader, ShaderPtr pixelShader) = 0;

        /**
         * @brief 是否链接成GPU程序
         */
        virtual bool hasLinked() const = 0;

    protected:
        /**
         * @brief 构造函数
         */
        GPUProgram(const String &name);
    };
}


#endif  /*__T3D_GPU_PROGRAM_H__*/
