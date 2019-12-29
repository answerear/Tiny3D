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


#ifndef __T3D_SCRIPT_PARSER_TEXTURE_UNIT_H__
#define __T3D_SCRIPT_PARSER_TEXTURE_UNIT_H__


#include "Parser/T3DScriptParserBase.h"


namespace Tiny3D
{
    class ScriptParserTextureUnit : public ScriptParserBase
    {
    public:
        /** 创建对象 */
        static ScriptParserTextureUnitPtr create();

        /** 析构函数 */
        ~ScriptParserTextureUnit();

        /** 重写基类 ScriptParserBase::parseObject 接口 */
        virtual TResult parseObject(
            DataStream &stream, Object *object, uint32_t version) override;

    protected:
        /** 构造函数 */
        ScriptParserTextureUnit();

        /** 解析 TextureUnit 属性 */
        TResult parseProperties(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 TextureUnit 的 sampler_ref 属性 */
        TResult parseSamplerRef(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 TextureUnit 的 texture_alias 属性 */
        TResult parseTextureAlias(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 TextureUnit 的 texture 属性 */
        TResult parseTexture(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 TextureUnit 的 anim_texture 属性 */
        TResult parseAnimTexture(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 TextureUnit 的 cubic_texture 属性 */
        TResult parseCubicTexture(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 TextureUnit 的 tex_coord_set 属性 */
        TResult parseTexCoordSet(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 TextureUnit 的 color_op 属性 */
        TResult parseColorOp(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 TextureUnit 的 color_op_ex 属性 */
        TResult parseColorOpEx(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 TextureUnit 的 color_op_multipass_fallback 属性 */
        TResult parseColorOpMultipassFallback(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 TextureUnit 的 alpha_op_ex 属性 */
        TResult parseAlphaOpEx(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 TextureUnit 的 env_map 属性 */
        TResult parseEnvMap(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 TextureUnit 的 scroll 属性 */
        TResult parseScroll(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 TextureUnit 的 scroll_anim 属性 */
        TResult parseScrollAnim(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 TextureUnit 的 rotate 属性 */
        TResult parseRotate(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 TextureUnit 的 rotate_anim 属性 */
        TResult parseRotateAnim(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 TextureUnit 的 scale 属性 */
        TResult parseScale(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 TextureUnit 的 wave_xform 属性 */
        TResult parseWaveXform(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 TextureUnit 的 transform 属性 */
        TResult parseTransform(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 TextureUnit 的 binding_type 属性 */
        TResult parseBindingType(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /** 解析 TextureUnit 的 content_type 属性 */
        TResult parseContentType(
            DataStream &stream, TextureUnit *unit, uint32_t version);
    };
}


#endif  /*__T3D_SCRIPT_PARSER_TEXTURE_UNIT_H__*/
