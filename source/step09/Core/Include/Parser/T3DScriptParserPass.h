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


#ifndef __T3D_SCRIPT_PARSER_PASS_H__
#define __T3D_SCRIPT_PARSER_PASS_H__


#include "Parser/T3DScriptParserBase.h"


namespace Tiny3D
{
    /**
     * @class   ScriptParserPass
     * @brief   A script parser pass.
     */
    class ScriptParserPass : public ScriptParserBase
    {
        T3D_CLASS(ScriptParserPass, ScriptParserBase);

    public:
        /**
         * @fn  static ScriptParserPassPtr ScriptParserPass::create();
         * @brief   创建对象
         * @return  A ScriptParserPassPtr.
         */
        static ScriptParserPassPtr create();

        /**
         * @fn  ScriptParserPass::~ScriptParserPass();
         * @brief   析构函数
         */
        ~ScriptParserPass();

        /**
         * @fn  virtual TResult ScriptParserPass::parseObject(
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
         * @fn  ScriptParserPass::ScriptParserPass();
         * @brief   构造函数
         */
        ScriptParserPass();

        /**
         * @fn  TResult ScriptParserPass::parseProperties(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseProperties(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseAmbient(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 ambient 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseAmbient(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseDiffuse(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 diffuse 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseDiffuse(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseSpecular(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 specular 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseSpecular(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseEmissive(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 emissive 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseEmissive(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseSceneBlend(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 scene_blend 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseSceneBlend(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseSeparateSceneBlend(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 separate_scene_blend 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseSeparateSceneBlend(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseSceneBlendOp(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 scene_blend_op 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseSceneBlendOp(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseSeparateSceneBlendOp(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 separate_scene_blend_op 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseSeparateSceneBlendOp(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseDepthCheck(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 depth_check 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseDepthCheck(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseDepthWrite(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 depth_write 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseDepthWrite(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseDepthBias(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 depth_bias 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseDepthBias(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseDepthFunc(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 depth_func 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseDepthFunc(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseIterationDepthBias(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 iteration_depth_bias 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseIterationDepthBias(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseAlphaRejection(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 alpha_rejection 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseAlphaRejection(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseAlphaToCoverage(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 alpha_to_coverage 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseAlphaToCoverage(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseLightScissor(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 light_scissor 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseLightScissor(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseLightClipPlanes(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 light_clip_planes 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseLightClipPlanes(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseTransparentSorting(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 transparent_sorting 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseTransparentSorting(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseIlluminationStage(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 illumination_stage 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseIlluminationStage(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseCullHardware(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 cull_hardware 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseCullHardware(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseCullSoftware(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 cull_software 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseCullSoftware(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseNormalizeNormals(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 normalize_normals 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseNormalizeNormals(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseLighting(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 lighting 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseLighting(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseShading(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 shading 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseShading(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parsePolygonMode(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 polygon_mode 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parsePolygonMode(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parsePolygonModeOverrideable(
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 polygon_mode_overrideable 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parsePolygonModeOverrideable(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseFogOverride( 
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 fog_override 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseFogOverride(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseColorWrite( 
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 color_write 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseColorWrite(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseMaxLights( 
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 max_lights 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseMaxLights(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseStartLight( 
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 start_light 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseStartLight(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseLightMask( 
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 light_mask 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseLightMask(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parseIteration( 
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 iteration 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parseIteration(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parsePointSize( 
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 point_size 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parsePointSize(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parsePointSprites( 
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 point_sprites 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parsePointSprites(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parsePointSizeAttenuation( 
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 point_size_attenuation 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parsePointSizeAttenuation(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parsePointSizeMin( 
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 point_size_min 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parsePointSizeMin(
            DataStream &stream, Pass *pass, uint32_t version);

        /**
         * @fn  TResult ScriptParserPass::parsePointSizeMax( 
         *      DataStream &stream, Pass *pass, uint32_t version);
         * @brief   解析 Pass 的 point_size_max 属性
         * @param [in,out]  stream  The stream.
         * @param [in,out]  pass    If non-null, the pass.
         * @param           version The version.
         * @return  A TResult.
         */
        TResult parsePointSizeMax(
            DataStream &stream, Pass *pass, uint32_t version);
    };
}


#endif  /*__T3D_SCRIPT_PARSER_PASS_H__*/
