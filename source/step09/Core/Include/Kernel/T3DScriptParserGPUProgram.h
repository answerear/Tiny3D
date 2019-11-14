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
     * @brief GPU 程序對象腳本解析器
     */
    class ScriptParserGPUProgram : public ScriptParserBase
    {
    public:
        /** 創建對象 */
        static ScriptParserGPUProgramPtr create();

        /** 析構函數 */
        virtual ~ScriptParserGPUProgram();
        
        /** 重寫基類 ScriptParserBase::parseObject 接口 */
        virtual TResult parseObject(
            DataStream &stream, Object *object, uint32_t version) override;

    protected:
        /** 構造函數 */
        ScriptParserGPUProgram();

        /** 解析 GPU Program 對象屬性  */
        TResult parseProperties(
            DataStream &stream, GPUProgram *program, uint32_t version);

    protected:
        String  mSource;
        String  mTarget;
        String  mEntry;
        String  mStage;
    };

    /**
     * @brief GPU 程序引用對象腳本解析器
     */
    class ScriptParserGPUProgramRef : public ScriptParserBase
    {
    public:
        /** 創建對象 */
        static ScriptParserGPUProgramRefPtr create();

        /** 析構函數 */
        virtual ~ScriptParserGPUProgramRef();

        /** 重寫基類 ScriptParserBase::parseObject 接口 */
        virtual TResult parseObject(
            DataStream &stream, Object *object, uint32_t version) override;

    protected:
        /** 構造函數 */
        ScriptParserGPUProgramRef();

        /** 解析 GPU Program 引用對象屬性 */
        TResult parseProperties(
            DataStream &stream, ShaderParam *param, uint32_t version);

        /** 解析 GPU Program 引用的 shared_params_ref 屬性 */
        TResult parseSharedParamsRef(
            DataStream &stream, ShaderParam *param, uint32_t version);

        /** 解析 GPU Program 引用的 param_indexed 屬性 */
        TResult parseParamIndexed(
            DataStream &stream, ShaderParam *param, uint32_t version);

        /** 解析 GPU Program 引用的 param_named 屬性 */
        TResult parseParamNamed(
            DataStream &stream, ShaderParam *param, uint32_t version);

        /** 解析 GPU Program 引用的 param_indexed_auto 屬性 */
        TResult parseParamIndexedAuto(
            DataStream &stream, ShaderParam *param, uint32_t version);

        /** 解析 GPU Program 引用的 param_named_auto 屬性 */
        TResult parseParamNamedAuto(
            DataStream &stream, ShaderParam *param, uint32_t version);
    };

    /**
     * @brief gpu_program 對象腳本解析器
     */
    class ScriptParserGPUProgramEx : public ScriptParserBase
    {
    public:
        /** 創建對象 */
        static ScriptParserGPUProgramExPtr create();

        /** 析構函數 */
        virtual ~ScriptParserGPUProgramEx();

        /** 重寫基類 ScriptParserBase::parseObject 接口 */
        virtual TResult parseObject(
            DataStream &stream, Object *object, uint32_t version);

    protected:
        /** 構造函數 */
        ScriptParserGPUProgramEx();

        /** 解析 gpu_program 對象屬性 */
        TResult parseProperties(
            DataStream &stream, GPUProgram *program, uint32_t version);
    };

    /**
     * @brief gpu_program_ref 對象腳本解析器
     */
    class ScriptParserGPUProgramRefEx : public ScriptParserBase
    {
    public:
        /** 創建對象 */
        static ScriptParserGPUProgramRefExPtr create();

        /** 析構函數 */
        virtual ~ScriptParserGPUProgramRefEx();

        /** 重寫基類 ScriptParserBase::parseObject 接口 */
        virtual TResult parseObject(
            DataStream &stream, Object *object, uint32_t version);

    protected:
        /** 構造函數 */
        ScriptParserGPUProgramRefEx();

        /** 解析 gpu_program_ref 對象屬性 */
        TResult parseProperties(
            DataStream &stream, ShaderParam *param, uint32_t version);
    };
}


#endif  /*__T3D_SCRIPT_PARSER_GPU_PROGRAM_H__*/
