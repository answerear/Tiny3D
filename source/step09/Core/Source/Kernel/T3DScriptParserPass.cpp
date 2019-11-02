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


#include "Kernel/T3DScriptParserPass.h"
#include "Kernel/T3DScriptParser.h"
#include "Kernel/T3DPass.h"
#include "Kernel/T3DTechnique.h"
#include "T3DErrorDef.h"


namespace Tiny3D
{
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

            PassPtr pass = Pass::create();

            uint16_t type = E_NT_UNKNOWN;
            uint16_t i = 0;

            for (i = 0; i < count; ++i)
            {
                // Type
                bytesOfRead = stream.read(&type, sizeof(type));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                    "Read the type of property of pass failed !");

                if (type == E_NT_PROPERTY)
                {
                    ret = parseProperties(stream, pass, version);
                }
                else if (type == E_NT_OBJECT)
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

    TResult ScriptParserPass::parseObjects(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
            size_t bytesOfRead = 0;

            // Op code
            uint16_t opcode = 0;
            bytesOfRead = stream.read(&opcode, sizeof(opcode));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(opcode),
                "Read the opcode of property of pass failed !");

            ScriptParserBase *parser
                = ScriptParser::getInstance().getObjectParser(opcode);
            if (parser == nullptr)
            {
                ret = T3D_ERR_RES_INVALID_OBJECT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid object in pass !");
                break;
            }

            ret = parser->parseObject(stream, pass, version);
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
                ret = parseInterationDepthBias(stream, pass, version);
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

            }
            else
            {
                // 非顶点颜色，ambient
                ColorARGB color;
                ret = parseColor(stream, color);
                if (ret != T3D_OK)
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE, 
                        "Read ambient color failed !");
                    break;
                }
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

            }
            else
            {
                // 非顶点颜色，diffuse
                ColorARGB color;
                ret = parseColor(stream, color);
                if (ret != T3D_OK)
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Read diffuse color failed !");
                    break;
                }
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
            }
            else
            {
                // 非顶点颜色，specular
                ColorARGB color;
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
            }
            else
            {
                // 非顶点颜色，emissive
                ColorARGB color;
                ret = parseColor(stream, color);
                if (ret != T3D_OK)
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Read diffuse color failed !");
                    break;
                }
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
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserPass::parseInterationDepthBias(
        DataStream &stream, Pass *pass, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
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
        } while (0);

        return ret;
    }

}
