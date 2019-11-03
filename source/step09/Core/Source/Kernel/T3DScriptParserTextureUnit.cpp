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


#include "Kernel/T3DScriptParserTextureUnit.h"
#include "Kernel/T3DScriptParser.h"
#include "Kernel/T3DPass.h"
#include "Kernel/T3DTextureUnit.h"
#include "T3DErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ScriptParserTextureUnitPtr ScriptParserTextureUnit::create()
    {
        ScriptParserTextureUnitPtr parser = new ScriptParserTextureUnit();
        parser->release();
        return parser;
    }

    //--------------------------------------------------------------------------

    ScriptParserTextureUnit::ScriptParserTextureUnit()
    {

    }

    //--------------------------------------------------------------------------

    ScriptParserTextureUnit::~ScriptParserTextureUnit()
    {

    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseObject(
        DataStream &stream, Object *object, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
            size_t bytesOfRead = 0;

            Pass *pass = static_cast<Pass*>(object);

            // 属性数量
            uint16_t count = 0;
            bytesOfRead = stream.read(&count, sizeof(count));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(count),
                "Read the number of children of texture unit failed !");

            // 名称
            String name;
            ret = parseString(stream, name);

            TextureUnitPtr unit = TextureUnit::create();

            uint16_t type = E_NT_UNKNOWN;
            uint16_t i = 0;

            for (i = 0; i < count; ++i)
            {
                // Type
                bytesOfRead = stream.read(&type, sizeof(type));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                    "Read the type of property of texture unit failed !");

                if (type == E_NT_PROPERTY)
                {
                    ret = parseProperties(stream, unit, version);
                }
                else if (type == E_NT_OBJECT)
                {
                    ret = parseObjects(stream, unit, version);
                }
                else
                {
                    ret = T3D_ERR_RES_INVALID_OBJECT;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid object type in texture unit script !");
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

    TResult ScriptParserTextureUnit::parseObjects(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
            size_t bytesOfRead = 0;

            // Op code
            uint16_t opcode = 0;
            bytesOfRead = stream.read(&opcode, sizeof(opcode));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(opcode),
                "Read the opcode of property of texture unit failed !");

            ScriptParserBase *parser
                = ScriptParser::getInstance().getObjectParser(opcode);
            if (parser == nullptr)
            {
                ret = T3D_ERR_RES_INVALID_OBJECT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid object in texture unit !");
                break;
            }

            ret = parser->parseObject(stream, unit, version);

        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseProperties(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
            size_t bytesOfRead = 0;

            // ID
            uint16_t id;
            bytesOfRead = stream.read(&id, sizeof(id));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(id),
                "Read ID of property of texture unit failed !");

            switch (id)
            {
            case E_OP_TEX_ADDRESS_MODE:
            case E_OP_TEX_BORDER_COLOUR:
            case E_OP_FILTERING:
            case E_OP_CMPTEST:
            case E_OP_CMPFUNC:
            case E_OP_COMP_FUNC:
            case E_OP_MAX_ANISOTROPY:
            case E_OP_MIPMAP_BIAS:
                {
                    ScriptParserBase *parser
                        = ScriptParser::getInstance().getObjectParser(id);
                    if (parser == nullptr)
                    {
                        ret = T3D_ERR_RES_INVALID_OBJECT;
                        T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                            "Invalid object in texture unit !");
                        break;
                    }

                    ret = parser->parseObject(stream, unit, version);
                }
                break;
            case E_OP_SAMPLER_REF:
                ret = parseSamplerRef(stream, unit, version);
                break;
            case E_OP_TEXTURE_ALIAS:
                ret = parseTextureAlias(stream, unit, version);
                break;
            case E_OP_TEXTURE:
                ret = parseTexture(stream, unit, version);
                break;
            case E_OP_ANIM_TEXTURE:
                ret = parseAnimTexture(stream, unit, version);
                break;
            case E_OP_CUBIC_TEXTURE:
                ret = parseCubicTexture(stream, unit, version);
                break;
            case E_OP_TEX_COORD_SET:
                ret = parseTexCoordSet(stream, unit, version);
                break;
            case E_OP_COLOUR_OP:
                ret = parseColorOp(stream, unit, version);
                break;
            case E_OP_COLOUR_OP_EX:
                ret = parseColorOpEx(stream, unit, version);
                break;
            case E_OP_COLOUR_OP_MULTIPASS_FALLBACK:
                ret = parseColorOpMultipassFallback(stream, unit, version);
                break;
            case E_OP_ALPHA_OP_EX:
                ret = parseColorOpEx(stream, unit, version);
                break;
            case E_OP_ENV_MAP:
                ret = parseEnvMap(stream, unit, version);
                break;
            case E_OP_SCROLL:
                ret = parseScroll(stream, unit, version);
                break;
            case E_OP_SCROLL_ANIM:
                ret = parseScrollAnim(stream, unit, version);
                break;
            case E_OP_ROTATE:
                ret = parseRotate(stream, unit, version);
                break;
            case E_OP_ROTATE_ANIM:
                ret = parseRotateAnim(stream, unit, version);
                break;
            case E_OP_SCALE:
                ret = parseScale(stream, unit, version);
                break;
            case E_OP_WAVE_XFORM:
                ret = parseWaveXform(stream, unit, version);
                break;
            case E_OP_TRANSFORM:
                ret = parseTransform(stream, unit, version);
                break;
            case E_OP_BINDING_TYPE:
                ret = parseBindingType(stream, unit, version);
                break;
            case E_OP_CONTENT_TYPE:
                ret = parseContentType(stream, unit, version);
                break;
            default:
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid property of texture unit !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseSamplerRef(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseTextureAlias(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseTexture(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseAnimTexture(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseCubicTexture(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseTexCoordSet(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseColorOp(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseColorOpEx(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseColorOpMultipassFallback(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseAlphaOpEx(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseEnvMap(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseScroll(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseScrollAnim(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseRotate(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseRotateAnim(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseScale(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseWaveXform(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseTransform(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseBindingType(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTextureUnit::parseContentType(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
        } while (0);

        return ret;
    }
}
