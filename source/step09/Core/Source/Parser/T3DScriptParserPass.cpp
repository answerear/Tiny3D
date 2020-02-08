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


#include "Parser/T3DScriptParserPass.h"
#include "Parser/T3DScriptParser.h"
#include "Kernel/T3DPass.h"
#include "Kernel/T3DTechnique.h"
#include "T3DErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    T3D_IMPLEMENT_CLASS_1(ScriptParserPass, ScriptParserBase);

    //--------------------------------------------------------------------------

    ScriptParserPassPtr ScriptParserPass::create()
    {
        ScriptParserPassPtr parser = new ScriptParserPass();
        parser->release();
        return parser;
    }

    //--------------------------------------------------------------------------

    ScriptParserPass::ScriptParserPass()
    {

    }

    //--------------------------------------------------------------------------

    ScriptParserPass::~ScriptParserPass()
    {

    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseObject(
        DataStream &stream, Object *object, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
            size_t bytesOfRead = 0;

            Technique *tech = static_cast<Technique*>(object);

            // 属性数量
            uint16_t count = 0;
            bytesOfRead = stream.read(&count, sizeof(count));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(count),
                "Read the number of children of pass failed !");

            // 名称
            String name;
            ret = parseString(stream, name);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read the name of pass failed !");
                break;
            }

            if (name.empty())
            {
                name = generateName("Pass");
            }

            // MD5
            uint8_t hash[16];
            bytesOfRead = stream.read(hash, sizeof(hash));
            T3D_CHECK_READ_CONTENT(bytesOfRead, 16,
                "Read hash of pass failed !");

            PassPtr pass;
            ret = tech->addPass(name, pass);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Add pass [%s] to technique failed !", name.c_str());
                break;
            }

            uint16_t type = UNKNOWN;
            uint16_t i = 0;

            for (i = 0; i < count; ++i)
            {
                // Type
                bytesOfRead = stream.read(&type, sizeof(type));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                    "Read the type of property of pass failed !");

                if (type == PROPERTY)
                {
                    ret = parseProperties(stream, pass, version);
                }
                else if (type == OBJECT)
                {
                    ret = parseObjects(stream, pass, version);
                }
                else
                {
                    ret = T3D_ERR_RES_INVALID_OBJECT;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid object type in pass script !");
                    break;
                }

                if (ret != T3D_OK)
                {
                    break;
                }
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseProperties(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // ID
            uint16_t id;
            bytesOfRead = stream.read(&id, sizeof(id));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(id),
                "Read ID of property of pass failed !");

            switch (id)
            {
            case E_OP_AMBIENT:
                ret = parseAmbient(stream, pass, version);
                break;
            case E_OP_DIFFUSE:
                ret = parseDiffuse(stream, pass, version);
                break;
            case E_OP_SPECULAR:
                ret = parseSpecular(stream, pass, version);
                break;
            case E_OP_EMISSIVE:
                ret = parseEmissive(stream, pass, version);
                break;
            case E_OP_SCENE_BLEND:
                ret = parseSceneBlend(stream, pass, version);
                break;
            case E_OP_SEPARATE_SCENE_BLEND:
                ret = parseSeparateSceneBlend(stream, pass, version);
                break;
            case E_OP_SCENE_BLEND_OP:
                ret = parseSceneBlendOp(stream, pass, version);
                break;
            case E_OP_SEPARATE_SCENE_BLEND_OP:
                ret = parseSeparateSceneBlendOp(stream, pass, version);
                break;
            case E_OP_DEPTH_CHECK:
                ret = parseDepthCheck(stream, pass, version);
                break;
            case E_OP_DEPTH_WRITE:
                ret = parseDepthWrite(stream, pass, version);
                break;
            case E_OP_DEPTH_BIAS:
                ret = parseDepthBias(stream, pass, version);
                break;
            case E_OP_DEPTH_FUNC:
                ret = parseDepthFunc(stream, pass, version);
                break;
            case E_OP_ITERATION_DEPTH_BIAS:
                ret = parseIterationDepthBias(stream, pass, version);
                break;
            case E_OP_ALPHA_REJECTION:
                ret = parseAlphaRejection(stream, pass, version);
                break;
            case E_OP_ALPHA_TO_COVERAGE:
                ret = parseAlphaToCoverage(stream, pass, version);
                break;
            case E_OP_LIGHT_SCISSOR:
                ret = parseLightScissor(stream, pass, version);
                break;
            case E_OP_LIGHT_CLIP_PLANES:
                ret = parseLightClipPlanes(stream, pass, version);
                break;
            case E_OP_TRANSPARENT_SORTING:
                ret = parseTransparentSorting(stream, pass, version);
                break;
            case E_OP_ILLUMINATION_STAGE:
                ret = parseIlluminationStage(stream, pass, version);
                break;
            case E_OP_CULL_HARDWARE:
                ret = parseCullHardware(stream, pass, version);
                break;
            case E_OP_CULL_SOFTWARE:
                ret = parseCullSoftware(stream, pass, version);
                break;
            case E_OP_NORMALISE_NORMALS:
                ret = parseNormalizeNormals(stream, pass, version);
                break;
            case E_OP_LIGHTING:
                ret = parseLighting(stream, pass, version);
                break;
            case E_OP_SHADING:
                ret = parseShading(stream, pass, version);
                break;
            case E_OP_POLYGON_MODE:
                ret = parsePolygonMode(stream, pass, version);
                break;
            case E_OP_POLYGON_MODE_OVERRIDEABLE:
                ret = parsePolygonModeOverrideable(stream, pass, version);
                break;
            case E_OP_FOG_OVERRIDE:
                ret = parseFogOverride(stream, pass, version);
                break;
            case E_OP_COLOUR_WRITE:
                ret = parseColorWrite(stream, pass, version);
                break;
            case E_OP_MAX_LIGHTS:
                ret = parseMaxLights(stream, pass, version);
                break;
            case E_OP_START_LIGHT:
                ret = parseStartLight(stream, pass, version);
                break;
            case E_OP_LIGHT_MASK:
                ret = parseLightMask(stream, pass, version);
                break;
            case E_OP_ITERATION:
                ret = parseIteration(stream, pass, version);
                break;
            case E_OP_POINT_SIZE:
                ret = parsePointSize(stream, pass, version);
                break;
            case E_OP_POINT_SPRITES:
                ret = parsePointSprites(stream, pass, version);
                break;
            case E_OP_POINT_SIZE_ATTENUATION:
                ret = parsePointSizeAttenuation(stream, pass, version);
                break;
            case E_OP_POINT_SIZE_MIN:
                ret = parsePointSizeMin(stream, pass, version);
                break;
            case E_OP_POINT_SIZE_MAX:
                ret = parsePointSizeMax(stream, pass, version);
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid property of pass !");
                }
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseAmbient(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // 先读ID，用于判断是 vertex color 还是非 vertex color
            uint16_t id = 0;
            bytesOfRead = stream.read(&id, sizeof(id));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(id), 
                "Read ID for ambient failed !");

            if (id == E_OP_VERTEXCOLOUR)
            {
                // 顶点颜色
                TrackVertexColorType tracking = pass->getVertexColorTracking();
                tracking |= TrackVertexColor::AMBIENT;
                pass->setVertexColorTracking(tracking);
            }
            else if (id == 0)
            {
                // 非顶点颜色，ambient
                ColorRGBA color;
                ret = parseColor(stream, color);
                if (ret != T3D_OK)
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE, 
                        "Read ambient color failed !");
                    break;
                }

                pass->setAmbient(color);
            }
            else
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid value of ambient !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseDiffuse(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // 先读ID，用于判断是 vertex color 还是非 vertex color
            uint16_t id = 0;
            bytesOfRead = stream.read(&id, sizeof(id));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(id),
                "Read ID for ambient failed !");

            if (id == E_OP_VERTEXCOLOUR)
            {
                // 顶点颜色
                TrackVertexColorType tracking = pass->getVertexColorTracking();
                tracking |= TrackVertexColor::DIFFUSE;
                pass->setVertexColorTracking(tracking);
            }
            else if (id == 0)
            {
                // 非顶点颜色，diffuse
                ColorRGBA color;
                ret = parseColor(stream, color);
                if (ret != T3D_OK)
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Read diffuse color failed !");
                    break;
                }

                pass->setDiffuse(color);
            }
            else
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid value of diffuse !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseSpecular(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // 先读ID，用于判断是 vertex color 还是非 vertex color
            uint16_t id = 0;
            bytesOfRead = stream.read(&id, sizeof(id));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(id),
                "Read ID for ambient failed !");

            if (id == E_OP_VERTEXCOLOUR)
            {
                // 顶点颜色

                // shininess
                float32_t shininess;
                bytesOfRead = stream.read(&shininess, sizeof(shininess));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(shininess),
                    "Read shininess of specular in pass failed !");

                TrackVertexColorType tracking = pass->getVertexColorTracking();
                tracking |= TrackVertexColor::SPECULAR;
                pass->setVertexColorTracking(tracking);

                pass->setShininess(shininess);
            }
            else if (id == 0)
            {
                // 非顶点颜色，specular
                ColorRGBA color;
                ret = parseColor(stream, color);
                if (ret != T3D_OK)
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Read diffuse color failed !");
                    break;
                }

                // shininess
                float32_t shininess;
                bytesOfRead = stream.read(&shininess, sizeof(shininess));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(shininess),
                    "Read shininess of specular in pass failed !");

                pass->setSpecular(color);
                pass->setShininess(shininess);
            }
            else
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid value of specular !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseEmissive(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // 先读ID，用于判断是 vertex color 还是非 vertex color
            uint16_t id = 0;
            bytesOfRead = stream.read(&id, sizeof(id));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(id),
                "Read ID for ambient failed !");

            if (id == E_OP_VERTEXCOLOUR)
            {
                // 顶点颜色
                TrackVertexColorType tracking = pass->getVertexColorTracking();
                tracking |= TrackVertexColor::EMISSIVE;
                pass->setVertexColorTracking(tracking);
            }
            else if (id == 0)
            {
                // 非顶点颜色，emissive
                ColorRGBA color;
                ret = parseColor(stream, color);
                if (ret != T3D_OK)
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Read diffuse color failed !");
                    break;
                }

                pass->setEmissive(color);
            }
            else
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid value of emissive !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseSceneBlend(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // operating #1
            uint16_t op0 = 0;
            bytesOfRead = stream.read(&op0, sizeof(op0));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(op0),
                "Read scene_blend first argument failed !");

            if (op0 >= E_OP_ONE && op0 <= E_OP_ONE_MINUS_SRC_ALPHA)
            {
                // operating #2
                uint16_t op1 = 0;
                bytesOfRead = stream.read(&op1, sizeof(op1));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(op1),
                    "Read scene_blend second argument failed !");

                BlendFactor src = toBlendFactor(op0);
                BlendFactor dst = toBlendFactor(op1);
                pass->setSceneBlending(src, dst);
            }
            else if (op0 == E_OP_ADD || op0 == E_OP_MODULATE
                || op0 == E_OP_COLOUR_BLEND || op0 == E_OP_ALPHA_BLEND
                || op0 == E_OP_REPLACE)
            {
                BlendType bt = toBlendType(op0);
                pass->setSceneBlending(bt);
            }
            else
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid operating code for scene_blend !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseSeparateSceneBlend(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // operating #1
            uint16_t op0 = 0;
            bytesOfRead = stream.read(&op0, sizeof(op0));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(op0),
                "Read separate_scene_blend first argument failed !");

            if (op0 >= E_OP_ONE && op0 <= E_OP_ONE_MINUS_SRC_ALPHA)
            {
                // operating #2
                uint16_t op1 = 0;
                bytesOfRead = stream.read(&op1, sizeof(op1));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(op1),
                    "Read separate_scene_blend second argument failed !");

                // operating #3
                uint16_t op2 = 0;
                bytesOfRead = stream.read(&op2, sizeof(op2));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(op2),
                    "Read separate_scene_blend third argument failed !");

                // operating #4
                uint16_t op3 = 0;
                bytesOfRead = stream.read(&op3, sizeof(op3));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(op3),
                    "Read separate_scene_blend fourth argument failed !");

                BlendFactor bf0 = toBlendFactor(op0);
                BlendFactor bf1 = toBlendFactor(op1);
                BlendFactor bf2 = toBlendFactor(op2);
                BlendFactor bf3 = toBlendFactor(op3);
                pass->setSeparateSceneBlending(bf0, bf1, bf2, bf3);
            }
            else if (op0 == E_OP_ADD || op0 == E_OP_MODULATE
                || op0 == E_OP_COLOUR_BLEND || op0 == E_OP_ALPHA_BLEND)
            {
                // operating #2
                uint16_t op1 = 0;
                bytesOfRead = stream.read(&op1, sizeof(op1));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(op1),
                    "Read separate_scene_blend second argument failed !");

                BlendType bt0 = toBlendType(op0);
                BlendType bt1 = toBlendType(op1);
                pass->setSeparateSceneBlending(bt0, bt1);
            }
            else
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid operating code for separate_scene_blend !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseSceneBlendOp(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // operating
            uint16_t op = 0;
            bytesOfRead = stream.read(&op, sizeof(op));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(op),
                "Read scene_blend_op failed !");

            if (op == E_OP_ADD || op == E_OP_SUBTRACT
                || op == E_OP_REVERSE_SUBTRACT
                || op == E_OP_MIN || op == E_OP_MAX)
            {
                BlendOperation bo = toBlendOperation(op);
                pass->setSceneBlendingOperation(bo);
            }
            else
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid value of scene_blend_op !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseSeparateSceneBlendOp(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // operating #1
            uint16_t op0 = 0;
            bytesOfRead = stream.read(&op0, sizeof(op0));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(op0),
                "Read separate_scene_blend_op first argument failed !");

            // operating #2
            uint16_t op1 = 0;
            bytesOfRead = stream.read(&op1, sizeof(op1));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(op1),
                "Read separate_scene_blend_op second argument failed !");

            if ((op0 == E_OP_ADD || op0 == E_OP_SUBTRACT
                || op0 == E_OP_REVERSE_SUBTRACT
                || op0 == E_OP_MIN || op0 == E_OP_MAX)
                && (op1 == E_OP_ADD || op1 == E_OP_SUBTRACT
                || op1 == E_OP_REVERSE_SUBTRACT
                || op1 == E_OP_MIN || op1 == E_OP_MAX))
            {
                BlendOperation bo0 = toBlendOperation(op0);
                BlendOperation bo1 = toBlendOperation(op1);
                pass->setSeparateSceneBlendingOperation(bo0, bo1);
            }
            else
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid value of separate_scene_blend_op !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseDepthCheck(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // enabled flag
            bool enabled = false;
            bytesOfRead = stream.read(&enabled, sizeof(enabled));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(enabled),
                "Read depth_check failed !");

            pass->setDepthCheckEnabled(enabled);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseDepthWrite(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // enabled flag
            bool enabled = false;
            bytesOfRead = stream.read(&enabled, sizeof(enabled));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(enabled),
                "Read depth_write failed !");

            pass->setDepthWriteEnabled(enabled);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseDepthBias(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // number of values
            uint16_t argc = 0;
            bytesOfRead = stream.read(&argc, sizeof(argc));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(argc),
                "Read the number of argument of depth_bias failed !");

            if (argc == 1)
            {
                // values
                float32_t val;
                bytesOfRead = stream.read(&val, sizeof(val));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val),
                    "Read the values of depth_bias failed !");

                Real slopeScaleBias = 0.0f;
                pass->setDepthBias(val, slopeScaleBias);
            }
            else if (argc == 2)
            {
                // values
                float32_t val0, val1;
                bytesOfRead = stream.read(&val0, sizeof(val0));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val0),
                    "Read the values of depth_bias failed !");

                bytesOfRead = stream.read(&val1, sizeof(val1));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val1),
                    "Read the values of depth_bias failed !");

                pass->setDepthBias(val0, val1);
            }
            else
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid number of arguments for depth_bias !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseDepthFunc(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // depth compare function
            uint16_t func = 0;
            bytesOfRead = stream.read(&func, sizeof(func));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(func),
                "Read depth_func failed !");

            switch (func)
            {
            case E_OP_ALWAYS_FAIL:
            case E_OP_ALWAYS_PASS:
            case E_OP_LESS:
            case E_OP_LESS_EQUAL:
            case E_OP_EQUAL:
            case E_OP_NOT_EQUAL:
            case E_OP_GREATER_EQUAL:
            case E_OP_GREATER:
                {
                    CompareFunction f = toCompareFunction(func);
                    pass->setDepthFunction(f);
                }
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid value of depth_func !");
                }
                break;
            }

        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseIterationDepthBias(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // value
            float32_t val = 0.0f;
            bytesOfRead = stream.read(&val, sizeof(val));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val), 
                "Read iteration_depth_bias failed !");

            pass->setDepthBiasPerIteration(val);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseAlphaRejection(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // number of arguments
            uint16_t argc = 0;
            bytesOfRead = stream.read(&argc, sizeof(argc));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(argc),
                "Read the number of values of alpha_rejection failed !");

            if (argc == 1 || argc == 2)
            {
                // compare function
                uint16_t func;
                bytesOfRead = stream.read(&func, sizeof(func));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(func),
                    "Read method of alpha_rejection failed !");

                switch (func)
                {
                    case E_OP_ALWAYS_FAIL:
                    case E_OP_ALWAYS_PASS:
                    case E_OP_LESS:
                    case E_OP_LESS_EQUAL:
                    case E_OP_EQUAL:
                    case E_OP_NOT_EQUAL:
                    case E_OP_GREATER_EQUAL:
                    case E_OP_GREATER:
                        {
                            CompareFunction f = toCompareFunction(func);
                            pass->setAlphaRejectFunction(f);
                        }
                        break;
                    default:
                        {
                            ret = T3D_ERR_RES_INVALID_PROPERTY;
                            T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                                "Invalid function of alpha_rejection !");
                        }
                        break;
                }

                if (argc == 2)
                {
                    uint32_t val = 0;
                    bytesOfRead = stream.read(&val, sizeof(val));
                    T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val),
                        "Read value of alpha_rejection failed !");

                    CompareFunction f = toCompareFunction(func);
                    pass->setAlphaRejectSettings(f, (uint8_t)val);
                }
            }
            else
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid number of values for alpha_rejection !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseAlphaToCoverage(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // enabled flag
            bool enabled = false;
            bytesOfRead = stream.read(&enabled, sizeof(enabled));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(enabled),
                "Read alpha_to_coverage failed !");

            pass->setAlphaToCoverageEnabled(enabled);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseLightScissor(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // enabled flag
            bool enabled = false;
            bytesOfRead = stream.read(&enabled, sizeof(enabled));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(enabled),
                "Read light_scissor failed !");

            pass->setLightScissoringEnabled(enabled);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseLightClipPlanes(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // enabled flag
            bool enabled = false;
            bytesOfRead = stream.read(&enabled, sizeof(enabled));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(enabled),
                "Read light_clip_planes failed !");

            pass->setLightClipPlanesEnabled(enabled);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseTransparentSorting(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // transparent sorting, 1(true), 0(false) or 2(force)
            uint8_t val;
            bytesOfRead = stream.read(&val, sizeof(val));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val),
                "Read transparent_sorting failed !");

            if (val == 0 || val == 1)
            {
                bool enabled = (val == 1);
                pass->setTransparentSortingEnabled(enabled);
                pass->setTransparentSortingForced(false);
            }
            else if (val == 2)
            {
                pass->setTransparentSortingEnabled(true);
                pass->setTransparentSortingForced(true);
            }
            else
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid value of transparent_sorting !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseIlluminationStage(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // illumination stage
            uint16_t stage = 0;
            bytesOfRead = stream.read(&stage, sizeof(stage));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(stage),
                "Read illumination_stage failed !");

            switch (stage)
            {
            case E_OP_AMBIENT:
            case E_OP_PER_LIGHT:
            case E_OP_DECAL:
                {
                    IlluminationStage is = toIlluminationStage(stage);
                    pass->setIlluminationStage(is);
                }
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid value of illumination_stage!");
                }
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseCullHardware(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // culling mode
            uint16_t mode = 0;
            bytesOfRead = stream.read(&mode, sizeof(mode));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(mode),
                "Read cull_hardware failed !");

            switch (mode)
            {
            case E_OP_CLOCKWISE:
            case E_OP_ANTICLOCKWISE:
            case E_OP_NONE:
                {
                    CullingMode m = toCullingMode(mode);
                    pass->setCullingMode(m);
                }
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid value of cull_hardware !");
                }
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseCullSoftware(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // culling mode
            uint16_t culling = 0;
            bytesOfRead = stream.read(&culling, sizeof(culling));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(culling),
                "Read cull_software failed !");

            switch (culling)
            {
            case E_OP_FRONT:
            case E_OP_BACK:
            case E_OP_NONE:
                {
                    ManualCullingMode mode = toManualCullingMode(culling);
                    pass->setManualCullingMode(mode);
                }
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid value of cull_software !");
                }
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseNormalizeNormals(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // enabled flag
            bool enabled = false;
            bytesOfRead = stream.read(&enabled, sizeof(enabled));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(enabled),
                "Read normalize_normals failed !");

            pass->setNormalizeNormalsEnabled(enabled);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseLighting(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // enabled flag
            bool enabled = false;
            bytesOfRead = stream.read(&enabled, sizeof(enabled));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(enabled),
                "Read lighting failed !");

            // TODO
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseShading(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // shading
            uint16_t shading = 0;
            bytesOfRead = stream.read(&shading, sizeof(shading));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(shading),
                "Read shading failed !");

            switch (shading)
            {
            case E_OP_FLAT:
            case E_OP_GOURAUD:
            case E_OP_PHONG:
                {
                    ShadingMode mode = toShadingMode(shading);
                    pass->setShadingMode(mode);
                }
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE, 
                        "Invalid value of shading !");
                }
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parsePolygonMode(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // polygon mode
            uint16_t mode;
            bytesOfRead = stream.read(&mode, sizeof(mode));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(mode),
                "Read polygon_mode failed !");

            switch (mode)
            {
            case E_OP_SOLID:
            case E_OP_POINTS:
            case E_OP_WIREFRAME:
                {
                    PolygonMode m = toPolygonMode(mode);
                    pass->setPolygonMode(m);
                }
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid value of polygon_mode !");
                }
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parsePolygonModeOverrideable(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // enabled flags
            bool enabled = false;
            bytesOfRead = stream.read(&enabled, sizeof(enabled));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(enabled),
                "Read polygon_mode_overrideable failed !");

            pass->setPolygonModeOverrideable(enabled);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseFogOverride(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // number of arguments
            uint16_t argc = 0;
            bytesOfRead = stream.read(&argc, sizeof(argc));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(argc),
                "Read the number of arguments of fog_override failed !");

            if (argc == 1)
            {
                // enabled flag
                bool overrideable = false;
                bytesOfRead = stream.read(&overrideable, sizeof(overrideable));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(overrideable),
                    "Read override value of fog_override failed !");

                if (overrideable)
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "When the first argument is true in fog_override, \
                        the number of arguments must be six !");
                    break;
                }

                pass->setFog(false);
            }
            else if (argc == 8)
            {
                // enabled flag
                bool overrideable = false;
                bytesOfRead = stream.read(&overrideable, sizeof(overrideable));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(overrideable),
                    "Read fog_override flag failed !");

                if (!overrideable)
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "When the first argument is false in fog_override, \
                        the number of arguments must be only one !");
                    break;
                }

                // type
                uint16_t type;
                bytesOfRead = stream.read(&type, sizeof(type));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                    "Read type value of fog_override failed !");

                // color
                ColorRGBA color;
                ret = parseColor(stream, color);
                if (ret != T3D_OK)
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Read color value of fog_override failed !");
                    break;
                }

                // density
                float32_t density = 0.0f;
                bytesOfRead = stream.read(&density, sizeof(density));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(density),
                    "Read density of fog_override failed !");

                // start
                float32_t start = 0.0f;
                bytesOfRead = stream.read(&start, sizeof(start));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(start),
                    "Read start of fog_override failed !");

                // end
                float32_t end = 0.0f;
                bytesOfRead = stream.read(&end, sizeof(end));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(end),
                    "Read end of fog_override failed !");

                switch (type)
                {
                case E_OP_NONE:
                case E_OP_LINEAR:
                case E_OP_EXP:
                case E_OP_EXP2:
                    {
                        FogMode mode = toFogMode(type);
                        pass->setFog(true, mode, color, density, start, end);
                    }
                    break;
                default:
                    {
                        ret = T3D_ERR_RES_INVALID_PROPERTY;
                        T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                            "Invalid type of fog_override !");
                    }
                    break;
                }
            }
            else
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid value of fog_override !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseColorWrite(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // enabled flag
            bool enabled = false;
            bytesOfRead = stream.read(&enabled, sizeof(enabled));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(enabled),
                "Read color_write failed !");

            pass->setColorWriteEnabled(true);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseMaxLights(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // max lights
            uint32_t val;
            bytesOfRead = stream.read(&val, sizeof(val));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val),
                "Read max_lights failed !");

            pass->setMaxLights((uint16_t)val);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseStartLight(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // start light
            uint32_t val;
            bytesOfRead = stream.read(&val, sizeof(val));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val),
                "Read start_light failed !");

            pass->setStartLight((uint16_t)val);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseLightMask(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // light mask
            uint32_t val;
            bytesOfRead = stream.read(&val, sizeof(val));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val),
                "Read light_mask failed !");

            // TODO
            // Deprecated
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseIteration(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // number of arguments
            uint16_t argc = 0;
            bytesOfRead = stream.read(&argc, sizeof(argc));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(argc),
                "Read the number of arguments of iteration failed !");

            if (argc >= 1 && argc <= 4)
            {
                // ID
                uint16_t flag = 0;
                bytesOfRead = stream.read(&flag, sizeof(flag));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(flag),
                    "Read ID of iteration failed !");

                if (flag == E_OP_ONCE)
                {
                    // once
                    pass->setIteratePerLight(false);
                }
                else if (flag == E_OP_ONCE_PER_LIGHT)
                {
                    // once_per_light

                    // type
                    uint16_t type = 0;
                    bytesOfRead = stream.read(&type, sizeof(type));
                    T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                        "Read type of iteration failed as once_per_light !");

                    switch (type)
                    {
                    case 0:
                        {
                            pass->setIteratePerLight(true, false);
                        }
                        break;
                    case E_OP_POINT:
                    case E_OP_DIRECTIONAL:
                    case E_OP_SPOT:
                        {
                            LightType lt = toLightType(type);
                            pass->setIteratePerLight(true, true, lt);
                        }
                        break;
                    default:
                        {
                            ret = T3D_ERR_RES_INVALID_PROPERTY;
                            T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                                "Invalid value of light type as once_per_light !");
                        }
                        break;
                    }
                }
                else if (flag == 0)
                {
                    // number

                    // value of number
                    int32_t val = 0;
                    bytesOfRead = stream.read(&val, sizeof(val));
                    T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val),
                        "Read number of iteration failed !");

                    if (argc == 3)
                    {
                        // per_light
                        uint16_t method = 0;
                        bytesOfRead = stream.read(&method, sizeof(method));
                        T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(method),
                            "Read method of iteration failed as number !");

                        // light type
                        uint16_t type = 0;
                        bytesOfRead = stream.read(&type, sizeof(type));
                        T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                            "Read light type of iteration failed as number !");

                        if (method != E_OP_PER_LIGHT)
                        {
                            ret = T3D_ERR_RES_INVALID_PROPERTY;
                            T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                                "Invalid value of iteration as number !");
                            break;
                        }

                        switch (type)
                        {
                        case 0:
                            {
                                pass->setIteratePerLight(true, false);
                            }
                            break;
                        case E_OP_POINT:
                        case E_OP_DIRECTIONAL:
                        case E_OP_SPOT:
                            {
                                LightType lt = toLightType(type);
                                pass->setIteratePerLight(true, true, lt);
                            }
                            break;
                        default:
                            {
                                ret = T3D_ERR_RES_INVALID_PROPERTY;
                                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                                    "Invalid value of light type !");
                            }
                            break;
                        }
                    }
                    else if (argc == 4)
                    {
                        // per_n_lights
                        uint16_t method = 0;
                        bytesOfRead = stream.read(&method, sizeof(method));
                        T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(method),
                            "Read method of iteration failed as number !");

                        // nubmer of lights
                        int32_t numOfLights = 0;
                        bytesOfRead = stream.read(&numOfLights, sizeof(numOfLights));
                        T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(numOfLights),
                            "Read number of lights of iteration failed as nubmer !");

                        // light type
                        uint16_t type = 0;
                        bytesOfRead = stream.read(&type, sizeof(type));
                        T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                            "Read light type of iteration failed as number !");

                        if (method != E_OP_PER_N_LIGHTS)
                        {
                            ret = T3D_ERR_RES_INVALID_PROPERTY;
                            T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                                "Invalid value of iteration as number !");
                            break;
                        }

                        pass->setLightCountPerIteration((uint16_t)numOfLights);

                        switch (type)
                        {
                        case 0:
                            {
                                pass->setIteratePerLight(true, false);
                            }
                            break;
                        case E_OP_POINT:
                        case E_OP_DIRECTIONAL:
                        case E_OP_SPOT:
                            {
                                LightType lt = toLightType(type);
                                pass->setIteratePerLight(true, true, lt);
                            }
                            break;
                        default:
                            {
                                ret = T3D_ERR_RES_INVALID_PROPERTY;
                                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                                    "Invalid value of light type !");
                            }
                            break;
                        }
                    }
                }
                else
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid flag of iteration !");
                    break;
                }
            }
            else
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid number of arguments of iteration !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parsePointSize(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // point size
            float32_t val = 0.0f;
            bytesOfRead = stream.read(&val, sizeof(val));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val),
                "Read point_size failed !");

            pass->setPointSize(val);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parsePointSprites(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // enabled flag
            bool enabled = false;
            bytesOfRead = stream.read(&enabled, sizeof(enabled));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(enabled),
                "Read point_sprites failed !");

            pass->setPointSpritesEnabled(enabled);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parsePointSizeAttenuation(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // number of arguments
            uint16_t argc = 0;
            bytesOfRead = stream.read(&argc, sizeof(argc));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(argc),
                "Read number of arguments of point_size_attenuation failed !");

            if (argc == 1)
            {
                // enabled flag
                bool enabled = false;
                bytesOfRead = stream.read(&enabled, sizeof(enabled));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(enabled),
                    "Read flag of point_size_attenuation failed !");

                if (enabled)
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid value of point_size_attenuation !");
                    break;
                }

                pass->setPointAttenuation(false);
            }
            else if (argc == 4)
            {
                // enabled flag
                bool enabled = false;
                bytesOfRead = stream.read(&enabled, sizeof(enabled));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(enabled),
                    "Read flag of point_size_attenuation failed !");

                // constant
                float32_t constant = 0.0f;
                bytesOfRead = stream.read(&constant, sizeof(constant));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(constant),
                    "Read constant of point_size_attenuation failed !");

                // linear
                float32_t linear = 0.0f;
                bytesOfRead = stream.read(&linear, sizeof(linear));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(linear),
                    "Read linear of point_size_attenuation failed !");

                // quadratic
                float32_t quadratic = 0.0f;
                bytesOfRead = stream.read(&quadratic, sizeof(quadratic));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(quadratic),
                    "Read quadratic of point_size_attenuation failed !");

                if (!enabled)
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid value of point_size_attenuation !");
                    break;
                }

                pass->setPointAttenuation(enabled, constant, linear, quadratic);
            }
            else
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid number of arguments of point_size_attenuation !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parsePointSizeMin(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // point size min
            float32_t val = 0.0f;
            bytesOfRead = stream.read(&val, sizeof(val));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val),
                "Read point_size_min failed !");

            pass->setPointMinSize(val);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parsePointSizeMax(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // point size max
            float32_t val = 0.0f;
            bytesOfRead = stream.read(&val, sizeof(val));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val),
                "Read point_size_max failed !");

            pass->setPointMaxSize(val);
        } while (0);

        return ret;
    }

}
