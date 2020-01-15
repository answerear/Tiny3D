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
    /**
     * @class   ScriptParserTextureUnit
     * @brief   A script parser texture unit.
     */
    class ScriptParserTextureUnit : public ScriptParserBase
    {
        T3D_DECLARE_CLASS();

    public:
        /**
         * @fn  static ScriptParserTextureUnitPtr 
         *      ScriptParserTextureUnit::create();
         * @brief   创建对象
         * @return  A ScriptParserTextureUnitPtr.
         */
        static ScriptParserTextureUnitPtr create();

        /**
         * @fn  ScriptParserTextureUnit::~ScriptParserTextureUnit();
         * @brief   析构函数
         */
        ~ScriptParserTextureUnit();

        /**
         * @fn  virtual TResult ScriptParserTextureUnit::parseObject( 
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
         * @fn  ScriptParserTextureUnit::ScriptParserTextureUnit();
         * @brief   构造函数
         */
        ScriptParserTextureUnit();

        /**
         * @fn  TResult ScriptParserTextureUnit::parseProperties( 
         *      DataStream &stream, TextureUnit *unit, uint32_t version);
         * @brief   解析 TextureUnit 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  unit    If non-null, the unit.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseProperties(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /**
         * @fn  TResult ScriptParserTextureUnit::parseSamplerRef( 
         *      DataStream &stream, TextureUnit *unit, uint32_t version);
         * @brief   解析 TextureUnit 的 sampler_ref 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  unit    If non-null, the unit.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseSamplerRef(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /**
         * @fn  TResult ScriptParserTextureUnit::parseTextureAlias( 
         *      DataStream &stream, TextureUnit *unit, uint32_t version);
         * @brief   解析 TextureUnit 的 texture_alias 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  unit    If non-null, the unit.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseTextureAlias(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /**
         * @fn  TResult ScriptParserTextureUnit::parseTexture( 
         *      DataStream &stream, TextureUnit *unit, uint32_t version);
         * @brief   解析 TextureUnit 的 texture 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  unit    If non-null, the unit.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseTexture(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /**
         * @fn  TResult ScriptParserTextureUnit::parseAnimTexture( 
         *      DataStream &stream, TextureUnit *unit, uint32_t version);
         * @brief   解析 TextureUnit 的 anim_texture 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  unit    If non-null, the unit.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseAnimTexture(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /**
         * @fn  TResult ScriptParserTextureUnit::parseCubicTexture( 
         *      DataStream &stream, TextureUnit *unit, uint32_t version);
         * @brief   解析 TextureUnit 的 cubic_texture 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  unit    If non-null, the unit.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseCubicTexture(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /**
         * @fn  TResult ScriptParserTextureUnit::parseTexCoordSet( 
         *      DataStream &stream, TextureUnit *unit, uint32_t version);
         * @brief   解析 TextureUnit 的 tex_coord_set 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  unit    If non-null, the unit.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseTexCoordSet(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /**
         * @fn  TResult ScriptParserTextureUnit::parseColorOp( 
         *      DataStream &stream, TextureUnit *unit, uint32_t version);
         * @brief   解析 TextureUnit 的 color_op 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  unit    If non-null, the unit.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseColorOp(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /**
         * @fn  TResult ScriptParserTextureUnit::parseColorOpEx( 
         *      DataStream &stream, TextureUnit *unit, uint32_t version);
         * @brief   解析 TextureUnit 的 color_op_ex 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  unit    If non-null, the unit.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseColorOpEx(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /**
         * @fn  TResult ScriptParserTextureUnit::parseColorOpMultipassFallback( 
         *      DataStream &stream, TextureUnit *unit, uint32_t version);
         * @brief   解析 TextureUnit 的 color_op_multipass_fallback 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  unit    If non-null, the unit.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseColorOpMultipassFallback(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /**
         * @fn  TResult ScriptParserTextureUnit::parseAlphaOpEx( 
         *      DataStream &stream, TextureUnit *unit, uint32_t version);
         * @brief   解析 TextureUnit 的 alpha_op_ex 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  unit    If non-null, the unit.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseAlphaOpEx(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /**
         * @fn  TResult ScriptParserTextureUnit::parseEnvMap( 
         *      DataStream &stream, TextureUnit *unit, uint32_t version);
         * @brief   解析 TextureUnit 的 env_map 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  unit    If non-null, the unit.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseEnvMap(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /**
         * @fn  TResult ScriptParserTextureUnit::parseScroll( 
         *      DataStream &stream, TextureUnit *unit, uint32_t version);
         * @brief   解析 TextureUnit 的 scroll 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  unit    If non-null, the unit.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseScroll(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /**
         * @fn  TResult ScriptParserTextureUnit::parseScrollAnim( 
         *      DataStream &stream, TextureUnit *unit, uint32_t version);
         * @brief   解析 TextureUnit 的 scroll_anim 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  unit    If non-null, the unit.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseScrollAnim(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /**
         * @fn  TResult ScriptParserTextureUnit::parseRotate( 
         *      DataStream &stream, TextureUnit *unit, uint32_t version);
         * @brief   解析 TextureUnit 的 rotate 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  unit    If non-null, the unit.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseRotate(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /**
         * @fn  TResult ScriptParserTextureUnit::parseRotateAnim( 
         *      DataStream &stream, TextureUnit *unit, uint32_t version);
         * @brief   解析 TextureUnit 的 rotate_anim 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  unit    If non-null, the unit.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseRotateAnim(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /**
         * @fn  TResult ScriptParserTextureUnit::parseScale( 
         *      DataStream &stream, TextureUnit *unit, uint32_t version);
         * @brief   解析 TextureUnit 的 scale 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  unit    If non-null, the unit.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseScale(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /**
         * @fn  TResult ScriptParserTextureUnit::parseWaveXform( 
         *      DataStream &stream, TextureUnit *unit, uint32_t version);
         * @brief   解析 TextureUnit 的 wave_xform 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  unit    If non-null, the unit.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseWaveXform(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /**
         * @fn  TResult ScriptParserTextureUnit::parseTransform( 
         *      DataStream &stream, TextureUnit *unit, uint32_t version);
         * @brief   解析 TextureUnit 的 transform 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  unit    If non-null, the unit.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseTransform(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /**
         * @fn  TResult ScriptParserTextureUnit::parseBindingType( 
         *      DataStream &stream, TextureUnit *unit, uint32_t version);
         * @brief   解析 TextureUnit 的 binding_type 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  unit    If non-null, the unit.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseBindingType(
            DataStream &stream, TextureUnit *unit, uint32_t version);

        /**
         * @fn  TResult ScriptParserTextureUnit::parseContentType( 
         *      DataStream &stream, TextureUnit *unit, uint32_t version);
         * @brief   解析 TextureUnit 的 content_type 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  unit    If non-null, the unit.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseContentType(
            DataStream &stream, TextureUnit *unit, uint32_t version);
    };
}


#endif  /*__T3D_SCRIPT_PARSER_TEXTURE_UNIT_H__*/
