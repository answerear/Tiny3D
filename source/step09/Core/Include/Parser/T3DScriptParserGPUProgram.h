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


#ifndef __T3D_SCRIPT_PARSER_GPU_PROGRAM_H__
#define __T3D_SCRIPT_PARSER_GPU_PROGRAM_H__


#include "Parser/T3DScriptParserBase.h"
#include "Resource/T3DGPUConstBuffer.h"


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
         *      DataStream &stream, GPUProgram *program, uint32_t version);
         * @brief   解析 GPU Program 對象屬性
         * @param [in,out]  stream  需要解析的輸入數據流對象.
         * @param [in,out]  program 父對象.
         * @param           version 輸入數據格式版本號.
         * @returns 調用成功返回 T3D_OK.
         */
        TResult parseProperties(
            DataStream &stream, GPUProgram *program, uint32_t version);
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
         *      DataStream &stream, GPUProgramRef *program, uint32_t version);
         * @brief   解析 GPU Program 引用對象屬性
         * @param [in,out]  stream  需要解析的輸入數據流對象.
         * @param [in,out]  program 父對象.
         * @param           version 輸入數據格式版本號.
         * @returns 調用成功返回 T3D_OK.
         */
        TResult parseProperties(
            DataStream &stream, GPUProgramRef *program, uint32_t version);
    };

    /**
     * @class   ScriptParserGPUContBuffer
     * @brief   gpu_cbuffer 腳本對象解析器.
     */
    class ScriptParserGPUContBuffer : public ScriptParserBase
    {
    public:
        /**
     * @enum    BuiltinConstant
     * @brief   Values that represent built-in constants
     */
        enum class BuiltinConstant : uint16_t
        {
            WORLD_MATRIX,   /**< 模型的世界變換矩陣 */
            INVERSE_WORLD_MATRIX,   /**< 世界變換矩陣逆矩陣 */
            TRANSPOSE_WORLD_MATRIX, /**< 世界變換矩陣的轉置矩陣 */
            INVERSE_TRANSPOSE_WORLD_MATRIX, /**<  */

            WORLD_MATRIX_ARRAY_3x4,
            WORLD_MATRIX_ARRAY,
            WORLD_DUALQUATERNION_ARRAY_2x4,
            WORLD_SCALE_SHEAR_MATRIX_ARRAY_3x4,

            VIEW_MATRIX,
            INVERSE_VIEW_MATRIX,
            TRANSPOSE_VIEW_MATRIX,
            INVERSE_TRANSPOSE_VIEW_MATRIX,

            PROJECTION_MATRIX,
            INVERSE_PROJECTION_MATRIX,
            TRANSPOSE_PROJECTION_MATRIX,
            INVERSE_TRANSPOSE_PROJECTION_MATRIX,

            VIEWPROJ_MATRIX,
            INVERSE_VIEWPROJ_MATRIX,
            TRANSPOSE_VIEWPROJ_MATRIX,
            INVERSE_TRANSPOSE_VIEWPROJ_MATRIX,

            WORLDVIEW_MATRIX,
            INVERSE_WORLDVIEW_MATRIX,
            TRANSPOSE_WORLDVIEW_MATRIX,
            INVERSE_TRANSPOSE_WORLDVIEW_MATRIX,

            WORLDVIEWPROJ_MATRIX,
            INVERSE_WORLDVIEWPROJ_MATRIX,
            TRANSPOSE_WORLDVIEWPROJ_MATRIX,
            INVERSE_TRANSPOSE_WORLDVIEWPROJ_MATRIX,

            RENDER_TARGET_FLIPPING,

            VERTEX_WINDING,

            FOG_COLOUR,
            FOG_PARAMS,
            SURFACE_AMBIENT_COLOUR,
            SURFACE_DIFFUSE_COLOUR,
            SURFACE_SPECULAR_COLOUR,
            SURFACE_EMISSIVE_COLOUR,
            SURFACE_SHININESS,
            SURFACE_ALPHA_REJECTION_VALUE,

            LIGHT_COUNT,

            AMBIENT_LIGHT_COLOUR,
            LIGHT_DIFFUSE_COLOUR,
            LIGHT_SPECULAR_COLOUR,
            LIGHT_ATTENUATION,
            SPOTLIGHT_PARAMS,
            LIGHT_POSITION,
            LIGHT_POSITION_OBJECT_SPACE,
            LIGHT_POSITION_VIEW_SPACE,
            LIGHT_DIRECTION,
            LIGHT_DIRECTION_OBJECT_SPACE,
            LIGHT_DIRECTION_VIEW_SPACE,
            LIGHT_DISTANCE_OBJECT_SPACE,
            LIGHT_POWER_SCALE,
            LIGHT_DIFFUSE_COLOUR_POWER_SCALED,
            LIGHT_SPECULAR_COLOUR_POWER_SCALED,
            LIGHT_DIFFUSE_COLOUR_ARRAY,
            LIGHT_SPECULAR_COLOUR_ARRAY,
            LIGHT_DIFFUSE_COLOUR_POWER_SCALED_ARRAY,
            LIGHT_SPECULAR_COLOUR_POWER_SCALED_ARRAY,
            LIGHT_ATTENUATION_ARRAY,
            LIGHT_POSITION_ARRAY,
            LIGHT_POSITION_OBJECT_SPACE_ARRAY,
            LIGHT_POSITION_VIEW_SPACE_ARRAY,
            LIGHT_DIRECTION_ARRAY,
            LIGHT_DIRECTION_OBJECT_SPACE_ARRAY,
            LIGHT_DIRECTION_VIEW_SPACE_ARRAY,
            LIGHT_DISTANCE_OBJECT_SPACE_ARRAY,
            LIGHT_POWER_SCALE_ARRAY,
            SPOTLIGHT_PARAMS_ARRAY,

            DERIVED_AMBIENT_LIGHT_COLOUR,
            DERIVED_SCENE_COLOUR,
            DERIVED_LIGHT_DIFFUSE_COLOUR,
            DERIVED_LIGHT_SPECULAR_COLOUR,

            DERIVED_LIGHT_DIFFUSE_COLOUR_ARRAY,
            DERIVED_LIGHT_SPECULAR_COLOUR_ARRAY,
            LIGHT_NUMBER,
            LIGHT_CASTS_SHADOWS,
            LIGHT_CASTS_SHADOWS_ARRAY,


            SHADOW_EXTRUSION_DISTANCE,
            CAMERA_POSITION,
            CAMERA_POSITION_OBJECT_SPACE,
            TEXTURE_VIEWPROJ_MATRIX,
            TEXTURE_VIEWPROJ_MATRIX_ARRAY,
            TEXTURE_WORLDVIEWPROJ_MATRIX,
            TEXTURE_WORLDVIEWPROJ_MATRIX_ARRAY,
            SPOTLIGHT_VIEWPROJ_MATRIX,
            SPOTLIGHT_VIEWPROJ_MATRIX_ARRAY,
            SPOTLIGHT_WORLDVIEWPROJ_MATRIX,
            SPOTLIGHT_WORLDVIEWPROJ_MATRIX_ARRAY,
            CUSTOM,
            TIME,
            TIME_0_X,
            COSTIME_0_X,
            SINTIME_0_X,
            TANTIME_0_X,
            TIME_0_X_PACKED,
            TIME_0_1,
            COSTIME_0_1,
            SINTIME_0_1,
            TANTIME_0_1,
            TIME_0_1_PACKED,
            TIME_0_2PI,
            COSTIME_0_2PI,
            SINTIME_0_2PI,
            TANTIME_0_2PI,
            TIME_0_2PI_PACKED,
            FRAME_TIME,
            FPS,
            VIEWPORT_WIDTH,
            VIEWPORT_HEIGHT,
            INVERSE_VIEWPORT_WIDTH,
            INVERSE_VIEWPORT_HEIGHT,
            VIEWPORT_SIZE,

            VIEW_DIRECTION,
            VIEW_SIDE_VECTOR,
            VIEW_UP_VECTOR,
            FOV,
            NEAR_CLIP_DISTANCE,
            FAR_CLIP_DISTANCE,

            PASS_NUMBER,
            PASS_ITERATION_NUMBER,

            ANIMATION_PARAMETRIC,

            TEXEL_OFFSETS,

            SCENE_DEPTH_RANGE,

            SHADOW_SCENE_DEPTH_RANGE,

            SHADOW_SCENE_DEPTH_RANGE_ARRAY,

            SHADOW_COLOUR,
            TEXTURE_SIZE,
            INVERSE_TEXTURE_SIZE,
            PACKED_TEXTURE_SIZE,

            TEXTURE_MATRIX,

            LOD_CAMERA_POSITION,
            LOD_CAMERA_POSITION_OBJECT_SPACE,
            LIGHT_CUSTOM,

            UNKNOWN = 999
        };

        /**
         * @fn  static ScriptParserGPUContBufferPtr 
         *      ScriptParserGPUContBuffer::create();
         * @brief   Creates a new ScriptParserGPUContBufferPtr
         * @returns A ScriptParserGPUContBufferPtr.
         */
        static ScriptParserGPUContBufferPtr create();

        /**
         * @fn  virtual ScriptParserGPUContBuffer::~ScriptParserGPUContBuffer();
         * @brief   Destructor
         */
        virtual ~ScriptParserGPUContBuffer();

        /**
         * @fn  virtual TResult ScriptParserGPUContBuffer::parseObject(
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
         * @fn  ScriptParserGPUContBuffer::ScriptParserGPUContBuffer();
         * @brief   Default constructor
         */
        ScriptParserGPUContBuffer();

        /**
         * @fn  TResult ScriptParserGPUContBuffer::parseProperties(
         *      DataStream& stream, GPUConstBuffer *buffer, uint32_t version);
         * @brief   解析 GPU Program 引用對象屬性
         * @param [in,out]  stream  The stream.
         * @param           buffer  If non-null, the parameter.
         * @param           version The version.
         * @returns A TResult.
         */
        TResult parseProperties(
            DataStream& stream, GPUConstBuffer *buffer, uint32_t version);

        /**
         * @fn  TResult ScriptParserGPUContBuffer::parseSharedParamsRef(
         *      DataStream& stream, GPUConstBuffer *buffer, uint32_t version);
         * @brief   解析 GPU Program 引用的 shared_params_ref 屬性
         * @param [in,out]  stream  The stream.
         * @param           buffer  If non-null, the parameter.
         * @param           version The version.
         * @returns A TResult.
         */
        TResult parseSharedParamsRef(
            DataStream& stream, GPUConstBuffer *buffer, uint32_t version);

        /**
         * @fn  TResult ScriptParserGPUContBuffer::parseParamIndexed(
         *      DataStream& stream, ShaderParam* param, uint32_t version);
         * @brief   解析 GPU Program 引用的 param_indexed 屬性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  param   If non-null, the parameter.
         * @param           version The version.
         * @returns A TResult.
         */
        TResult parseParamIndexed(
            DataStream& stream, GPUConstBuffer *buffer, uint32_t version);

        /**
         * @fn  TResult ScriptParserGPUContBuffer::parseParamNamed(
         *      DataStream& stream, GPUConstBuffer *buffer, uint32_t version);
         * @brief   解析 GPU Program 引用的 param_named 屬性
         * @param [in,out]  stream  The stream.
         * @param           buffer  If non-null, the parameter.
         * @param           version The version.
         * @returns A TResult.
         */
        TResult parseParamNamed(
            DataStream& stream, GPUConstBuffer *buffer, uint32_t version);

        /**
         * @fn  TResult ScriptParserGPUContBuffer::parseParamIndexedAuto(
         *      DataStream& stream, GPUConstBuffer *buffer, uint32_t version);
         * @brief   解析 GPU Program 引用的 param_indexed_auto 屬性
         * @param [in,out]  stream  The stream.
         * @param           buffer  If non-null, the parameter.
         * @param           version The version.
         * @returns A TResult.
         */
        TResult parseParamIndexedAuto(
            DataStream& stream, GPUConstBuffer *buffer, uint32_t version);

        /**
         * @fn  TResult ScriptParserGPUContBuffer::parseParamNamedAuto(
         *      DataStream& stream, GPUConstBuffer *buffer, uint32_t version);
         * @brief   解析 GPU Program 引用的 param_named_auto 屬性
         * @param [in,out]  stream  The stream.
         * @param           buffer  If non-null, the parameter.
         * @param           version The version.
         * @returns A TResult.
         */
        TResult parseParamNamedAuto(
            DataStream& stream, GPUConstBuffer *buffer, uint32_t version);
    };

    /**
     * @class   ScriptParserGPUProgramEx
     * @brief   gpu_cbuffer_ref 腳本對象解析器
     */
    class ScriptParserGPUContBufferRef : public ScriptParserBase
    {
    public:
        /**
         * @fn  static ScriptParserGPUContBufferRefPtr 
         *      ScriptParserGPUContBufferRef::create();
         * @brief   創建對象
         * @returns A ScriptParserGPUContBufferRefPtr.
         */
        static ScriptParserGPUContBufferRefPtr create();

        /**
         * @fn  virtual 
         *      ScriptParserGPUContBufferRef::~ScriptParserGPUContBufferRef();
         * @brief   析構函數
         */
        virtual ~ScriptParserGPUContBufferRef();

        /**
         * @fn  virtual TResult ScriptParserGPUContBufferRef::parseObject(
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
         * @fn  ScriptParserGPUContBufferRef::ScriptParserGPUContBufferRef();
         * @brief   構造函數
         */
        ScriptParserGPUContBufferRef();

        /**
         * @fn  TResult ScriptParserGPUContBufferRef::parseProperties(
         *      DataStream &stream, GPUProgramRef *program, uint32_t version);
         * @brief   解析 gpu_program 對象屬性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  program If non-null, the program.
         * @param           version The version.
         * @returns A TResult.
         */
        TResult parseProperties(
            DataStream &stream, GPUProgramRef *program, uint32_t version);

    protected:
        uint32_t    mSlot;  /**< The slot */
    };
}


#endif  /*__T3D_SCRIPT_PARSER_GPU_PROGRAM_H__*/
