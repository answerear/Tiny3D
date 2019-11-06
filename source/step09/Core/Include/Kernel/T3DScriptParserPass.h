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


#ifndef __T3D_SCRIPT_PARSER_PASS_H__
#define __T3D_SCRIPT_PARSER_PASS_H__


#include "Kernel/T3DScriptParserBase.h"


namespace Tiny3D
{
    class ScriptParserPass : public ScriptParserBase
    {
    public:
        /** 创建对象 */
        static ScriptParserPassPtr create();

        /** 析构函数 */
        ~ScriptParserPass();

        /** 重写基类 ScriptParserBase::parseObject 接口 */
        virtual TResult parseObject(
            DataStream &stream, Object *object, uint32_t version) override;

    protected:
        /** 构造函数 */
        ScriptParserPass();

        /** 解析 Pass 属性 */
        TResult parseProperties(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 ambient 属性 */
        TResult parseAmbient(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 diffuse 属性 */
        TResult parseDiffuse(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 specular 属性 */
        TResult parseSpecular(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 emissive 属性 */
        TResult parseEmissive(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 scene_blend 属性 */
        TResult parseSceneBlend(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 separate_scene_blend 属性 */
        TResult parseSeparateSceneBlend(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 scene_blend_op 属性 */
        TResult parseSceneBlendOp(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 separate_scene_blend_op 属性 */
        TResult parseSeparateSceneBlendOp(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 depth_check 属性 */
        TResult parseDepthCheck(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 depth_write 属性 */
        TResult parseDepthWrite(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 depth_bias 属性 */
        TResult parseDepthBias(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 depth_func 属性 */
        TResult parseDepthFunc(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 iteration_depth_bias 属性 */
        TResult parseInterationDepthBias(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 alpha_rejection 属性 */
        TResult parseAlphaRejection(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 alpha_to_coverage 属性 */
        TResult parseAlphaToCoverage(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 light_scissor 属性 */
        TResult parseLightScissor(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 light_clip_planes 属性 */
        TResult parseLightClipPlanes(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 transparent_sorting 属性 */
        TResult parseTransparentSorting(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 illumination_stage 属性 */
        TResult parseIlluminationStage(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 cull_hardware 属性 */
        TResult parseCullHardware(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 cull_software 属性 */
        TResult parseCullSoftware(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 normalize_normals 属性 */
        TResult parseNormalizeNormals(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 lighting 属性 */
        TResult parseLighting(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 shading 属性 */
        TResult parseShading(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 polygon_mode 属性 */
        TResult parsePolygonMode(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 polygon_mode_overrideable 属性 */
        TResult parsePolygonModeOverrideable(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 fog_override 属性 */
        TResult parseFogOverride(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 color_write 属性 */
        TResult parseColorWrite(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 max_lights 属性 */
        TResult parseMaxLights(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 start_light 属性 */
        TResult parseStartLight(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 light_mask 属性 */
        TResult parseLightMask(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 iteration 属性 */
        TResult parseIteration(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 point_size 属性 */
        TResult parsePointSize(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 point_sprites 属性 */
        TResult parsePointSprites(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 point_size_attenuation 属性 */
        TResult parsePointSizeAttenuation(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 point_size_min 属性 */
        TResult parsePointSizeMin(
            DataStream &stream, Pass *pass, uint32_t version);

        /** 解析 Pass 的 point_size_max 属性 */
        TResult parsePointSizeMax(
            DataStream &stream, Pass *pass, uint32_t version);
    };
}


#endif  /*__T3D_SCRIPT_PARSER_PASS_H__*/
