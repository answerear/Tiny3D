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


#ifndef __T3D_D3D11_GPU_PROGRAM_CREATOR_H__
#define __T3D_D3D11_GPU_PROGRAM_CREATOR_H__


#include "T3DD3D11Prerequisites.h"


namespace Tiny3D
{
    /**
     * @brief DirectX 11 着色器生成器
     */
    class D3D11ShaderCreator : public ShaderCreator
    {
    public:
        static const char * const SHADER_TYPE;

        /**
         * @brief 重写ShaderCreator::getType() 接口
         */
        virtual String getType() const override;

        /**
         * @brief 重写ShaderCreator::createObject() 接口
         */
        virtual ShaderPtr createObject(int32_t argc, ...) const override;
    };

    /**
     * @brief DirectX 11 GPU程序生成器
     */
    class D3D11GPUProgramCreator : public GPUProgramCreator
    {
    public:
        static const char * const GPUPROGRAM_TYPE;

        /**
         * @brief 重写ShaderCreator::getType() 接口
         */
        virtual String getType() const override;

        /**
         * @brief 重写ShaderCreator::createObject() 接口
         */
        virtual GPUProgramPtr createObject(int32_t argc, ...) const override;
    };
}


#endif  /*__T3D_D3D11_GPU_PROGRAM_CREATOR_H__*/