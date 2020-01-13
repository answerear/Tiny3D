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


#ifndef __T3D_SCRIPT_PARSER_TECHNIQUE_H__
#define __T3D_SCRIPT_PARSER_TECHNIQUE_H__


#include "Parser/T3DScriptParserBase.h"


namespace Tiny3D
{
    /**
     * @class   ScriptParserTechnique
     * @brief   A script parser technique.
     */
    class ScriptParserTechnique : public ScriptParserBase
    {
        T3D_CLASS(ScriptParserTechnique, ScriptParserBase);

    public:
        /**
         * @fn  static ScriptParserTechniquePtr ScriptParserTechnique::create();
         * @brief   创建对象
         * @return  A ScriptParserTechniquePtr.
         */
        static ScriptParserTechniquePtr create();

        /**
         * @fn  virtual ScriptParserTechnique::~ScriptParserTechnique();
         * @brief   析构函数
         */
        virtual ~ScriptParserTechnique();

        /** 重写基类 ScriptParserBase::parseObject 接口 */
        virtual TResult parseObject(
            DataStream &stream, Object *object, uint32_t version) override;

    protected:
        /**
         * @fn  ScriptParserTechnique::ScriptParserTechnique();
         * @brief   构造函数
         */
        ScriptParserTechnique();

        /**
         * @fn  TResult ScriptParserTechnique::parseProperties( 
         *      DataStream &stream, Technique *tech, uint32_t version);
         * @brief   解析 Technique 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  tech    If non-null, the technology.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseProperties(
            DataStream &stream, Technique *tech, uint32_t version);

        /**
         * @fn  TResult ScriptParserTechnique::parseScheme( 
         *      DataStream &stream, Technique *tech, uint32_t version);
         * @brief   解析 Technique 的 scheme 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  tech    If non-null, the technology.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseScheme(
            DataStream &stream, Technique *tech, uint32_t version);

        /**
         * @fn  TResult ScriptParserTechnique::parseLODIndex( 
         *      DataStream &stream, Technique *tech, uint32_t version);
         * @brief   解析 Technique 的 lod_index 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  tech    If non-null, the technology.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseLODIndex(
            DataStream &stream, Technique *tech, uint32_t version);

        /**
         * @fn  TResult ScriptParserTechnique::parseShadowCasterMaterial( 
         *      DataStream &stream, Technique *tech, uint32_t version);
         * @brief   解析 Technique 的 shadow_caster_material 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  tech    If non-null, the technology.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseShadowCasterMaterial(
            DataStream &stream, Technique *tech, uint32_t version);

        /**
         * @fn  TResult ScriptParserTechnique::parseShadowReceiverMaterial( 
         *      DataStream &stream, Technique *tech, uint32_t version);
         * @brief   解析 Technique 的 shadow_receiver_material 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  tech    If non-null, the technology.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseShadowReceiverMaterial(
            DataStream &stream, Technique *tech, uint32_t version);

        /**
         * @fn  TResult ScriptParserTechnique::parseGPUVendorRule( 
         *      DataStream &stream, Technique *tech, uint32_t version);
         * @brief   解析 Technique 的 gpu_vendor_rule 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  tech    If non-null, the technology.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseGPUVendorRule(
            DataStream &stream, Technique *tech, uint32_t version);

        /**
         * @fn  TResult ScriptParserTechnique::parseGPUDeviceRule( 
         *      DataStream &stream, Technique *tech, uint32_t version);
         * @brief   解析 Technique 的 gpu_device_rule 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  tech    If non-null, the technology.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseGPUDeviceRule(
            DataStream &stream, Technique *tech, uint32_t version);

    protected:
        
    };
}


#endif  /*__T3D_SCRIPT_PARSER_TECHNIQUE_H__*/
