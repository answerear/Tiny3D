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


#ifndef __T3D_SCRIPT_PARSER_H__
#define __T3D_SCRIPT_PARSER_H__


#include "Parser/T3DScriptParserBase.h"


namespace Tiny3D
{
    /**
     * @class   ScriptParser
     * @brief   A 3D engine api.
     */
    class T3D_ENGINE_API ScriptParser 
        : public Singleton<ScriptParser>
        , public ScriptParserBase
    {
    public:
        /**
         * @fn  static ScriptParserPtr create();
         * @brief   创建对象
         * @return  A ScriptParserPtr.
         */
        static ScriptParserPtr create();

        /**
         * @fn  virtual ~ScriptParser();
         * @brief   析构函数
         */
        virtual ~ScriptParser();

        /**
         * @fn  TResult parse(DataStream &stream, Object *material);
         * @brief   解析数据流到material对象中
         * @param [in,out]  stream      The stream.
         * @param [in,out]  material    If non-null, the material.
         * @return  A TResult.
         */
        TResult parse(DataStream &stream, Object *material);

        /**
         * @fn  TResult parse(DataStream &stream);
         * @brief   Parses the given stream
         * @param [in,out]  stream  The stream.
         * @return  A TResult.
         */
        TResult parse(DataStream &stream);

        /**
         * @fn  ScriptParserBase *getObjectParser(uint16_t opcode);
         * @brief   获取对应操作码的对象脚本解析器
         * @param   opcode  The opcode.
         * @return  Null if it fails, else the object parser.
         */
        ScriptParserBase *getObjectParser(uint16_t opcode);

    protected:
        /**
         * @fn  ScriptParser();
         * @brief   构造函数
         */
        ScriptParser();

        /**
         * @fn  virtual TResult parseObject( DataStream &stream, 
         *      Object *object, uint32_t version) override;
         * @brief   重写基类 ScriptParserBase::parseObject 接口
         * @param [in,out]  stream  The stream.
         * @param [in,out]  object  If non-null, the object.
         * @param           version The version.
         * @return  A TResult.
         */
        virtual TResult parseObject(
            DataStream &stream, Object *object, uint32_t version) override;

    protected:
        ScriptParserMaterialPtr         mMaterialParser;    /**< The material parser */
        ScriptParserTechniquePtr        mTechParser;        /**< The technology parser */
        ScriptParserPassPtr             mPassParser;        /**< The pass parser */
        ScriptParserTextureUnitPtr      mTexUnitParser;     /**< The tex unit parser */
        ScriptParserSamplerPtr          mSamplerParser;     /**< The sampler parser */
        ScriptParserShaderPtr           mShaderParser;      /**< The shader parser */
        ScriptParserGPUProgramPtr       mGPUParser;         /**< The GPU parser */
        ScriptParserGPUProgramRefPtr    mGPURefParser;      /**< The GPU reference parser */
        ScriptParserGPUContBufferPtr    mCBufferParser;     /**< The c buffer parser */
        ScriptParserGPUContBufferRefPtr mCBufferRefParser;  /**< The c buffer reference parser */
    };

    #define T3D_SCRIPT_PARSER   (ScriptParser::getInstance())
}


#endif  /*__T3D_SCRIPT_PARSER_H__*/
