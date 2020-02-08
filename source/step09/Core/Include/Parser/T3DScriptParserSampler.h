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


#ifndef __T3D_SCRIPT_PARSER_SAMPLER_H__
#define __T3D_SCRIPT_PARSER_SAMPLER_H__


#include "Parser/T3DScriptParserBase.h"


namespace Tiny3D
{
    /**
     * @class   ScriptParserSampler
     * @brief   A script parser sampler.
     */
    class ScriptParserSampler : public ScriptParserBase
    {
        T3D_DECLARE_CLASS();

    public:
        /**
         * @fn  static ScriptParserSamplerPtr ScriptParserSampler::create();
         * @brief   创建对象
         * @return  A ScriptParserSamplerPtr.
         */
        static ScriptParserSamplerPtr create();

        /**
         * @fn  virtual ScriptParserSampler::~ScriptParserSampler();
         * @brief   析构函数
         */
        virtual ~ScriptParserSampler();

        /**
         * @fn  virtual TResult ScriptParserSampler::parseObject(
         *      DataStream &stream, Object *object, uint32_t version) override;
         * @brief   重写基类 ScriptParserBase::parseObject 接口
         * @param [in,out]  stream  The stream.
         * @param [in,out]  object  If non-null, the object.
         * @param           version The version.
         * @return  A TResult.
         */
        virtual TResult parseObject(
            DataStream &stream, Object *object, uint32_t version) override;

    protected:
        /**
         * @fn  ScriptParserSampler::ScriptParserSampler();
         * @brief   构造函数
         */
        ScriptParserSampler();

        /**
         * @fn  TResult ScriptParserSampler::parseProperties( 
         *      DataStream &stream, Sampler *sample, uint32_t version);
         * @brief   解析 Sampler 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  sample  If non-null, the sample.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseProperties(
            DataStream &stream, Sampler *sampler, uint32_t version);

        /**
         * @fn  TResult ScriptParserSampler::parseTexAddressMode( 
         *      DataStream &stream, Sampler *sample, uint32_t version);
         * @brief   解析 tex_address_mode 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  sample  If non-null, the sample.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseTexAddressMode(
            DataStream &stream, Sampler *sampler, uint32_t version);

        /**
         * @fn  TResult ScriptParserSampler::parseTexBorderColor( 
         *      DataStream &stream, Sampler *sample, uint32_t version);
         * @brief   解析 tex_border_color 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  sample  If non-null, the sample.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseTexBorderColor(
            DataStream &stream, Sampler *sampler, uint32_t version);

        /**
         * @fn  TResult ScriptParserSampler::parseFiltering( 
         *      DataStream &stream, Sampler *sample, uint32_t version);
         * @brief   解析 filtering 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  sample  If non-null, the sample.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseFiltering(
            DataStream &stream, Sampler *sampler, uint32_t version);

        /**
         * @fn  TResult ScriptParserSampler::parseCompareTest( 
         *      DataStream &stream, Sampler *sample, uint32_t version);
         * @brief   解析 cmp_test 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  sample  If non-null, the sample.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseCompareTest(
            DataStream &stream, Sampler *sampler, uint32_t version);

        /**
         * @fn  TResult ScriptParserSampler::parseCompareFunc( 
         *      DataStream &stream, Sampler *sample, uint32_t version);
         * @brief   解析 cmp_func 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  sample  If non-null, the sample.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseCompareFunc(
            DataStream &stream, Sampler *sampler, uint32_t version);

        /**
         * @fn  TResult ScriptParserSampler::parseMaxAnisotropy( 
         *      DataStream &stream, Sampler *sample, uint32_t version);
         * @brief   解析 max_anisotropy 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  sample  If non-null, the sample.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseMaxAnisotropy(
            DataStream &stream, Sampler *sampler, uint32_t version);

        /**
         * @fn  TResult ScriptParserSampler::parseMipmapBias( 
         *      DataStream &stream, Sampler *sample, uint32_t version);
         * @brief   解析 mipmap_bias 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  sample  If non-null, the sample.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseMipmapBias(
            DataStream &stream, Sampler *sampler, uint32_t version);
    };
}


#endif  /*__T3D_SCRIPT_PARSER_SAMPLER_H__*/
