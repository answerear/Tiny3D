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


#include "Parser/T3DScriptParserBase.h"


namespace Tiny3D
{
    /**
     * @class   ScriptParserShader
     * @brief   vertex_program、fragment_program等腳本對象解析器
     */
    class ScriptParserShader : public ScriptParserBase
    {
    public:
        /**
         * @fn  static ScriptParserShaderPtr ScriptParserShader::create();
         * @brief   創建解析器對象
         * @returns 調用成功返回新建對象.
         */
        static ScriptParserShaderPtr create();

        /**
         * @fn  virtual ScriptParserShader::~ScriptParserShader();
         * @brief   析構函數
         */
        virtual ~ScriptParserShader();
        
        /**
         * @fn  virtual TResult ScriptParserShader::parseObject(
         *      DataStream &stream, Object *object, uint32_t version) override;
         * @brief   重寫基類 ScriptParserBase::parseObject 接口
         * @param [in,out]  stream  需要解析的輸入數據流對象.
         * @param [in,out]  object  父對象.
         * @param           version 輸入數據格式版本號.
         * @returns 調用成功返回 T3D_OK.
         */
        virtual TResult parseObject(
            DataStream &stream, Object *object, uint32_t version) override;

    protected:
        /**
         * @fn  ScriptParserShader::ScriptParserShader();
         * @brief   默認構造函數
         */
        ScriptParserShader();

        /**
         * @fn  TResult ScriptParserShader::parseProperties(
         *      DataStream &stream, GPUProgram *program, uint32_t version);
         * @brief   解析 Shader 對象屬性
         * @param [in,out]  stream  需要解析的輸入數據流對象.
         * @param [in,out]  program 父對象.
         * @param           version 輸入數據格式版本號.
         * @returns 調用成功返回 T3D_OK.
         */
        TResult parseProperties(
            DataStream &stream, GPUProgram *program, uint32_t version);

    protected:
        String  mSource;
        String  mTarget;
        String  mEntry;
        String  mStage;
    };

    /**
     * @class   ScriptParserGPUProgram
     * @brief   gpu_program 腳本對象解析器
     */
    class ScriptParserGPUProgram : public ScriptParserBase
    {
    public:
        /**
         * @fn  static ScriptParserGPUProgramPtr 
         *      ScriptParserGPUProgram::create();
         * @brief   創建 ScriptParserGPUProgram 對象
         * @returns 調用成功返回新建對象.
         */
        static ScriptParserGPUProgramPtr create();

        /**
         * @fn  virtual ScriptParserGPUProgram::~ScriptParserGPUProgram();
         * @brief   析構函數
         */
        virtual ~ScriptParserGPUProgram();

        /**
         * @fn  virtual TResult ScriptParserGPUProgram::parseObject(
         *      DataStream& stream, Object* object, uint32_t version) override;
         * @brief   重寫基類 ScriptParserBase::parseObject 接口
         * @param [in,out]  stream  需要解析的輸入數據流對象.
         * @param [in,out]  object  父對象.
         * @param           version 輸入數據格式版本號.
         * @returns A TResult.
         */
        virtual TResult parseObject(
            DataStream& stream, Object* object, uint32_t version) override;

    protected:
        /**
         * @fn  ScriptParserGPUProgram::ScriptParserGPUProgram();
         * @brief   默認構造函數
         */
        ScriptParserGPUProgram();

        /**
         * @fn  TResult ScriptParserGPUProgram::parseProperties(
         *      DataStream& stream, GPUProgram* program, uint32_t version);
         * @brief   解析 GPU Program 對象屬性
         * @param [in,out]  stream  需要解析的輸入數據流對象.
         * @param [in,out]  program 父對象.
         * @param           version 輸入數據格式版本號.
         * @returns 調用成功返回 T3D_OK.
         */
        TResult parseProperties(
            DataStream& stream, GPUProgram* program, uint32_t version);
    };

    /**
     * @class   ScriptParserGPUProgramRef
     * @brief   gpu_program_ref 腳本對象解析器
     */
    class ScriptParserGPUProgramRef : public ScriptParserBase
    {
    public:
        /**
         * @fn  static ScriptParserGPUProgramRefPtr 
         *      ScriptParserGPUProgramRef::create();
         * @brief   創建對象
         * @returns 調用成功返回新建對象.
         */
        static ScriptParserGPUProgramRefPtr create();

        /**
         * @fn  virtual ScriptParserGPUProgramRef::~ScriptParserGPUProgramRef();
         * @brief   析構函數
         */
        virtual ~ScriptParserGPUProgramRef();

        /**
         * @fn  virtual TResult ScriptParserGPUProgramRef::parseObject(
         *      DataStream &stream, Object *object, uint32_t version) override;
         * @brief   重寫基類 ScriptParserBase::parseObject 接口
         * @param [in,out]  stream  需要解析的輸入數據流對象.
         * @param [in,out]  program 父對象.
         * @param           version 輸入數據格式版本號.
         * @returns 調用成功返回 T3D_OK.
         */
        virtual TResult parseObject(
            DataStream &stream, Object *object, uint32_t version) override;

    protected:
        /**
         * @fn  ScriptParserGPUProgramRef::ScriptParserGPUProgramRef();
         * @brief   構造函數
         */
        ScriptParserGPUProgramRef();

        /**
         * @fn  TResult ScriptParserGPUProgramRef::parseProperties(
         *      DataStream& stream, ShaderParam* param, uint32_t version);
         * @brief   解析 GPU Program 引用對象屬性
         * @param [in,out]  stream  需要解析的輸入數據流對象.
         * @param [in,out]  program 父對象.
         * @param           version 輸入數據格式版本號.
         * @returns 調用成功返回 T3D_OK.
         */
        TResult parseProperties(
            DataStream& stream, ShaderParam* param, uint32_t version);
    };

    /**
     * @class   ScriptParserGPUCBuffer
     * @brief   gpu_cbuffer 腳本對象解析器.
     */
    class ScriptParserGPUCBuffer : public ScriptParserBase
    {
    public:
        /**
         * @fn  static ScriptParserGPUCBufferPtr ScriptParserGPUCBuffer::create();
         * @brief   Creates a new ScriptParserGPUCBufferPtr
         * @returns A ScriptParserGPUCBufferPtr.
         */
        static ScriptParserGPUCBufferPtr create();

        /**
         * @fn  virtual ScriptParserGPUCBuffer::~ScriptParserGPUCBuffer();
         * @brief   Destructor
         */
        virtual ~ScriptParserGPUCBuffer();

        /**
         * @fn  virtual TResult ScriptParserGPUCBuffer::parseObject(
         *      DataStream& stream, Object* object, uint32_t version) override;
         * @brief   Parse object
         * @param [in,out]  stream  The stream.
         * @param [in,out]  object  If non-null, the object.
         * @param           version The version.
         * @returns A TResult.
         */
        virtual TResult parseObject(
            DataStream& stream, Object* object, uint32_t version) override;

    protected:
        /**
         * @fn  ScriptParserGPUCBuffer::ScriptParserGPUCBuffer();
         * @brief   Default constructor
         */
        ScriptParserGPUCBuffer();

        /**
         * @fn  TResult ScriptParserGPUCBuffer::parseProperties(
         *      DataStream& stream, ShaderParam* param, uint32_t version);
         * @brief   解析 GPU Program 引用對象屬性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  param   If non-null, the parameter.
         * @param           version The version.
         * @returns A TResult.
         */
        TResult parseProperties(
            DataStream& stream, ShaderParam *param, uint32_t version);

        /**
         * @fn  TResult ScriptParserGPUCBuffer::parseSharedParamsRef(
         *      DataStream& stream, ShaderParam* param, uint32_t version);
         * @brief   解析 GPU Program 引用的 shared_params_ref 屬性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  param   If non-null, the parameter.
         * @param           version The version.
         * @returns A TResult.
         */
        TResult parseSharedParamsRef(
            DataStream& stream, ShaderParam* param, uint32_t version);

        /**
         * @fn  TResult ScriptParserGPUCBuffer::parseParamIndexed(
         *      DataStream& stream, ShaderParam* param, uint32_t version);
         * @brief   解析 GPU Program 引用的 param_indexed 屬性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  param   If non-null, the parameter.
         * @param           version The version.
         * @returns A TResult.
         */
        TResult parseParamIndexed(
            DataStream& stream, ShaderParam *param, uint32_t version);

        /**
         * @fn  TResult ScriptParserGPUCBuffer::parseParamNamed(
         *      DataStream& stream, ShaderParam* param, uint32_t version);
         * @brief   解析 GPU Program 引用的 param_named 屬性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  param   If non-null, the parameter.
         * @param           version The version.
         * @returns A TResult.
         */
        TResult parseParamNamed(
            DataStream& stream, ShaderParam *param, uint32_t version);

        /**
         * @fn  TResult ScriptParserGPUCBuffer::parseParamIndexedAuto(
         *      DataStream& stream, ShaderParam* param, uint32_t version);
         * @brief   解析 GPU Program 引用的 param_indexed_auto 屬性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  param   If non-null, the parameter.
         * @param           version The version.
         * @returns A TResult.
         */
        TResult parseParamIndexedAuto(
            DataStream& stream, ShaderParam *param, uint32_t version);

        /**
         * @fn  TResult ScriptParserGPUCBuffer::parseParamNamedAuto(
         *      DataStream& stream, ShaderParam* param, uint32_t version);
         * @brief   解析 GPU Program 引用的 param_named_auto 屬性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  param   If non-null, the parameter.
         * @param           version The version.
         * @returns A TResult.
         */
        TResult parseParamNamedAuto(
            DataStream& stream, ShaderParam *param, uint32_t version);
    };

    /**
     * @class   ScriptParserGPUProgramEx
     * @brief   gpu_cbuffer_ref 腳本對象解析器
     */
    class ScriptParserGPUCBufferRef : public ScriptParserBase
    {
    public:
        /**
         * @fn  static ScriptParserGPUCBufferRefPtr 
         *      ScriptParserGPUCBufferRef::create();
         * @brief   創建對象
         * @returns A ScriptParserGPUCBufferRefPtr.
         */
        static ScriptParserGPUCBufferRefPtr create();

        /**
         * @fn  virtual ScriptParserGPUCBufferRef::~ScriptParserGPUCBufferRef();
         * @brief   析構函數
         */
        virtual ~ScriptParserGPUCBufferRef();

        /**
         * @fn  virtual TResult ScriptParserGPUCBufferRef::parseObject(
         *      DataStream &stream, Object *object, uint32_t version);
         * @brief   重寫基類 ScriptParserBase::parseObject 接口
         * @param [in,out]  stream  The stream.
         * @param [in,out]  object  If non-null, the object.
         * @param           version The version.
         * @returns A TResult.
         */
        virtual TResult parseObject(
            DataStream &stream, Object *object, uint32_t version);

    protected:
        /**
         * @fn  ScriptParserGPUCBufferRef::ScriptParserGPUCBufferRef();
         * @brief   構造函數
         */
        ScriptParserGPUCBufferRef();

        /**
         * @fn  TResult ScriptParserGPUCBufferRef::parseProperties(
         *      DataStream &stream, GPUProgram *program, uint32_t version);
         * @brief   解析 gpu_program 對象屬性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  program If non-null, the program.
         * @param           version The version.
         * @returns A TResult.
         */
        TResult parseProperties(
            DataStream &stream, GPUProgram *program, uint32_t version);
    };
}


#endif  /*__T3D_SCRIPT_PARSER_GPU_PROGRAM_H__*/
