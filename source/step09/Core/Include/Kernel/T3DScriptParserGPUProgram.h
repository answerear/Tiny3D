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


#ifndef __T3D_SCRIPT_PARSER_GPU_PROGRAM_H__
#define __T3D_SCRIPT_PARSER_GPU_PROGRAM_H__


#include "Kernel/T3DScriptParserBase.h"


namespace Tiny3D
{
    /**
     * @brief GPU 程序对象脚本解析器
     */
    class ScriptParserGPUProgram : public ScriptParserBase
    {
    public:
        /** 创建对象 */
        static ScriptParserGPUProgramPtr create();

        /** 析构函数 */
        virtual ~ScriptParserGPUProgram();
        
        /** 重写基类 ScriptParserBase::parseObject 接口 */
        virtual TResult parseObject(
            DataStream &stream, Object *object, uint32_t version) override;

    protected:
        /** 构造函数 */
        ScriptParserGPUProgram();

        /** 解析 GPU Program 对象的属性 */
        TResult parseProperties(
            DataStream &stream, GPUProgram *program, uint32_t version);
    };

    /**
     * @brief GPU 程序引用对象脚本解析器
     */
    class ScriptParserGPUProgramRef : public ScriptParserBase
    {
    public:
        /** 创建对象 */
        static ScriptParserGPUProgramRefPtr create();

        /** 析构函数 */
        virtual ~ScriptParserGPUProgramRef();

        /** 重写基类 ScriptParserBase::parseObject 接口 */
        virtual TResult parseObject(
            DataStream &stream, Object *object, uint32_t version) override;

    protected:
        /** 构造函数 */
        ScriptParserGPUProgramRef();

        /** 解析 GPU Program 引用子对象属性 */
        TResult parseProperties(
            DataStream &stream, ShaderParam *param, uint32_t version);

        /** 解析 GPU Program 引用的 shared_params_ref 属性 */
        TResult parseSharedParamsRef(
            DataStream &stream, ShaderParam *param, uint32_t version);

        /** 解析 GPU Program 引用的 param_indexed 属性 */
        TResult parseParamIndexed(
            DataStream &stream, ShaderParam *param, uint32_t version);

        /** 解析 GPU Program 引用的 param_named 屬性 */
        TResult parseParamNamed(
            DataStream &stream, ShaderParam *param, uint32_t version);

        /** 解析 GPU Program 引用的 param_indexed_auto 属性 */
        TResult parseParamIndexedAuto(
            DataStream &stream, ShaderParam *param, uint32_t version);

        /** 解析 GPU Program 引用的 param_named_auto 屬性 */
        TResult parseParamNamedAuto(
            DataStream &stream, ShaderParam *param, uint32_t version);
    };
}


#endif  /*__T3D_SCRIPT_PARSER_GPU_PROGRAM_H__*/
