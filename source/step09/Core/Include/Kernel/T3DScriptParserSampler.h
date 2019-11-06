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


#ifndef __T3D_SCRIPT_PARSER_SAMPLER_H__
#define __T3D_SCRIPT_PARSER_SAMPLER_H__


#include "Kernel/T3DScriptParserBase.h"


namespace Tiny3D
{
    class ScriptParserSampler : public ScriptParserBase
    {
    public:
        /** 创建对象 */
        static ScriptParserSamplerPtr create();

        /** 析构函数 */
        virtual ~ScriptParserSampler();

        /** 重写基类 ScriptParserBase::parseObject 接口 */
        virtual TResult parseObject(
            DataStream &stream, Object *object, uint32_t version) override;

        /** 解析 sampler 属性 */
        TResult parseSamplerParams(
            DataStream &stream, Object *object, uint32_t version);

    protected:
        /** 构造函数 */
        ScriptParserSampler();

        /** 解析 Sampler 属性 */
        TResult parseProperties(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 tex_address_mode 属性 */
        TResult parseTexAddressMode(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 tex_border_color 属性 */
        TResult parseTexBorderColor(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 filtering 属性 */
        TResult parseFiltering(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 cmp_test 属性 */
        TResult parseCompareTest(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 cmp_func 属性 */
        TResult parseCompareFunc(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 max_anisotropy 属性 */
        TResult parseMaxAnisotropy(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 mipmap_bias 属性 */
        TResult parseMipmapBias(
            DataStream &stream, TextureUnit *unit, uint32_t version);
    };
}


#endif  /*__T3D_SCRIPT_PARSER_SAMPLER_H__*/
