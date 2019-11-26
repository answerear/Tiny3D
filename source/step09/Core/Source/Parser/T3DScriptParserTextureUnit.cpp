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


#include "Parser/T3DScriptParserTextureUnit.h"
#include "Parser/T3DScriptParser.h"
#include "Parser/T3DScriptParserSampler.h"
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

            uint16_t type = UNKNOWN;
            uint16_t i = 0;

            for (i = 0; i < count; ++i)
            {
                // Type
                bytesOfRead = stream.read(&type, sizeof(type));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                    "Read the type of property of texture unit failed !");

                if (type == PROPERTY)
                {
                    ret = parseProperties(stream, unit, version);
                }
                else if (type == OBJECT)
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

                    ScriptParserSampler *sampler 
                        = static_cast<ScriptParserSampler*>(parser);
                    ret = sampler->parseSamplerParams(stream, unit, version);
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
            // the name of sampler
            String name;
            ret = parseString(stream, name);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read the name of sampler_ref failed !");
                break;
            }

            // TODO
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
            // the name of texture alias
            String name;
            ret = parseString(stream, name);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read the name of texture_alias failed !");
                break;
            }

            // TODO
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
            size_t bytesOfRead = 0;

            // number of arguments
            uint16_t argc = 0;
            bytesOfRead = stream.read(&argc, sizeof(argc));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(argc),
                "Read the number of arguments of texture failed !");

            if (argc >= 1 && argc <= 6)
            {
                String name;
                ret = parseString(stream, name);
                if (ret != T3D_OK)
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Read the number of texture failed !");
                    break;
                }

                uint16_t i = 0;
                for (i = 1; i < argc; ++i)
                {
                    uint16_t id;
                    bytesOfRead = stream.read(&id, sizeof(id));
                    T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(id),
                        "Read value of texture failed !");

                    switch (id)
                    {
                    case E_OP_1D:
                    case E_OP_2D:
                    case E_OP_3D:
                    case E_OP_CUBIC:
                    case E_OP_2DARRAY:
                    case E_OP_UNLIMITED:
                    case E_OP_ALPHA:
                    case E_OP_GAMMA:
                        {
                            // TODO
                        }
                        break;
                    case E_OP_PIXELFORMAT:
                        {
                            // value of pixel format
                            uint32_t format = 0;
                            bytesOfRead = stream.read(&format, sizeof(format));
                            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(format),
                                "Read value of pixel_format failed !");

                            // TODO
                        }
                        break;
                    default:
                        {
                            // number of mipmaps

                            // TODO
                        }
                        break;
                    }
                }

                if (ret != T3D_OK)
                {
                    break;
                }
            }
            else
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, 
                    "Invalid number of arguments of texture !");
                break;
            }
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
            size_t bytesOfRead = 0;

            // number of arguments
            uint16_t argc = 0;
            bytesOfRead = stream.read(&argc, sizeof(argc));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(argc),
                "Read the number of arguments of anim_texture failed !");

            if (argc >= 3)
            {
                // type, short or long form
                uint16_t type = 0;
                bytesOfRead = stream.read(&type, sizeof(type));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                    "Read type of anim_texture failed !");

                if (type == 0)
                {
                    // short form

                    // name
                    String name;
                    ret = parseString(stream, name);
                    if (ret != T3D_OK)
                    {
                        T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                            "Read name of anim_texture failed !");
                        break;
                    }

                    // number of frames
                    uint16_t frames = 0;
                    bytesOfRead = stream.read(&frames, sizeof(frames));
                    T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(frames),
                        "Read number of frames of anim_texture failed !");

                    // duration
                    float32_t duration = 0.0f;
                    bytesOfRead = stream.read(&duration, sizeof(duration));
                    T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(duration),
                        "Read duration of anim_texture failed !");

                    // TODO
                }
                else if (type == 1)
                {
                    // long form

                    // number of frames
                    uint16_t frames = 0;
                    bytesOfRead = stream.read(&frames, sizeof(frames));
                    T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(frames),
                        "Read number of frames of anim_texture failed !");

                    // duration
                    float32_t duration = 0.0f;
                    bytesOfRead = stream.read(&duration, sizeof(duration));
                    T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(duration),
                        "Read duration of anim_texture failed !");

                    // names
                    uint16_t i = 3;
                    for (i = 0; i < argc; ++i)
                    {
                        String name;
                        ret = parseString(stream, name);
                        if (ret != T3D_OK)
                        {
                            T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                                "Read name of anim_texture failed !");
                            break;
                        }

                        // TODO
                    }

                    if (ret != T3D_OK)
                    {
                        break;
                    }

                    // TODO
                }
                else
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid type of anim_texture !");
                    break;
                }
            }
            else
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid number of arguments of anim_texture !");
                break;
            }
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
            size_t bytesOfRead = 0;

            // type
            uint16_t type = 0;
            bytesOfRead = stream.read(&type, sizeof(type));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                "Read type of cubic_textuer failed !");

            if (type == 2)
            {
                // base_name
                String name;
                ret = parseString(stream, name);
                if (ret != T3D_OK)
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Read name of cubic_texture failed !");
                    break;
                }

                // combinedUVW | separateUV
                uint16_t id = 0;
                bytesOfRead = stream.read(&id, sizeof(id));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(id),
                    "Read combinedUVW | separateUV of cubic_texture failed !");

                // TODO
            }
            else if (type == 7)
            {
                // front
                String front;
                ret = parseString(stream, front);
                if (ret != T3D_OK)
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Read front of cubic_texture failed !");
                    break;
                }

                // back
                String back;
                ret = parseString(stream, back);
                if (ret != T3D_OK)
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Read back of cubic_texture failed !");
                    break;
                }

                // left
                String left;
                ret = parseString(stream, left);
                if (ret != T3D_OK)
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Read left of cubic_texture failed !");
                    break;
                }

                // right
                String right;
                ret = parseString(stream, right);
                if (ret != T3D_OK)
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Read right of cubic_texture failed !");
                    break;
                }

                // up
                String up;
                ret = parseString(stream, up);
                if (ret != T3D_OK)
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Read up of cubic_texture failed !");
                    break;
                }

                // down
                String down;
                ret = parseString(stream, down);
                if (ret != T3D_OK)
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Read down of cubic_texture failed !");
                    break;
                }

                // separateUV
                uint16_t separateUV;
                bytesOfRead = stream.read(&separateUV, sizeof(separateUV));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(separateUV),
                    "Read separateUV of cubic_texture failed !");
            }
            else
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid type of anim_texture !");
                break;
            }
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
            size_t bytesOfRead = 0;

            // set_num
            uint32_t setNum = 0;
            bytesOfRead = stream.read(&setNum, sizeof(setNum));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(setNum),
                "read set_num of tex_coord_set failed !");

            // TODO
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
            size_t bytesOfRead = 0;

            // value of color_op
            uint16_t method = 0;
            bytesOfRead = stream.read(&method, sizeof(method));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(method),
                "Read value of color_op failed !");

            switch (method)
            {
            case E_OP_REPLACE:
            case E_OP_ADD:
            case E_OP_MODULATE:
            case E_OP_ALPHA_BLEND:
                {
                    // TODO
                }
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid value of color_op !");
                }
                break;
            }
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
            size_t bytesOfRead = 0;

            // operation
            uint16_t op = 0;
            bytesOfRead = stream.read(&op, sizeof(op));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(op),
                "Read opration of color_op_ex failed !");

            // source1
            uint16_t source1 = 0;
            bytesOfRead = stream.read(&source1, sizeof(source1));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(source1),
                "Read source1 of color_op_ex failed !");

            // source2
            uint16_t source2 = 0;
            bytesOfRead = stream.read(&source2, sizeof(source2));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(source2),
                "Read source2 of color_op_ex failed !");

            float32_t manualBlend = 0.0f;
            ColorARGB arg1 = ColorARGB::WHITE;
            ColorARGB arg2 = ColorARGB::WHITE;

            switch (op)
            {
            case E_OP_BLEND_MANUAL:
                {
                    // manual_factor
                    bytesOfRead = stream.read(&manualBlend, sizeof(manualBlend));
                    T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(manualBlend),
                        "Read manual_factor of color_op_ex failed !");
                }
            case E_OP_SOURCE1:
            case E_OP_SOURCE2:
            case E_OP_MODULATE:
            case E_OP_MODULATE_X2:
            case E_OP_MODULATE_X4:
            case E_OP_ADD:
            case E_OP_ADD_SIGNED:
            case E_OP_ADD_SMOOTH:
            case E_OP_SUBTRACT:
            case E_OP_BLEND_DIFFUSE_ALPHA:
            case E_OP_BLEND_TEXTURE_ALPHA:
            case E_OP_BLEND_CURRENT_ALPHA:
            case  E_OP_DOT_PRODUCT:
            case E_OP_BLEND_DIFFUSE_COLOUR:
                {
                    // TODO
                }
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid operation of color_op !");
                }
                break;
            }

            if (ret != T3D_OK)
            {
                break;
            }

            switch (source1)
            {
            case E_OP_SRC_MANUAL:
                {
                    // manual_color1
                    ret = parseColor(stream, arg1);
                    if (ret != T3D_OK)
                    {
                        T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                            "Read manual_color1 of color_op_ex failed !");
                        break;
                    }
                }
            case E_OP_SRC_CURRENT:
            case E_OP_SRC_TEXTURE:
            case E_OP_SRC_DIFFUSE:
            case E_OP_SRC_SPECULAR:
                {
                    // TODO
                }
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid source1 of color_op !");
                }
                break;
            }

            if (ret != T3D_OK)
            {
                break;
            }

            switch (source2)
            {
            case E_OP_SRC_MANUAL:
                {
                    // manual_color1
                    ret = parseColor(stream, arg2);
                    if (ret != T3D_OK)
                    {
                        T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                            "Read manual_color2 of color_op_ex failed !");
                        break;
                    }
                }
            case E_OP_SRC_CURRENT:
            case E_OP_SRC_TEXTURE:
            case E_OP_SRC_DIFFUSE:
            case E_OP_SRC_SPECULAR:
                {
                    // TODO
                }
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid source2 of color_op !");
                }
                break;
            }

            if (ret != T3D_OK)
            {
                break;
            }

            // TODO
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
            size_t bytesOfRead = 0;

            // number of arguments
            uint16_t argc = 0;
            bytesOfRead = stream.read(&argc, sizeof(argc));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(argc),
                "Read the number of arguments of color_op_multipass_fallback \
                failed !");

            if (argc == 1)
            {
                // factor
                uint16_t factor = 0;
                bytesOfRead = stream.read(&factor, sizeof(factor));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(factor),
                    "Read src_factor of color_op_multipass_fallback failed !");

                switch (factor)
                {
                case E_OP_ADD:
                case E_OP_MODULATE:
                case E_OP_COLOUR_BLEND:
                case E_OP_ALPHA_BLEND:
                case E_OP_REPLACE:
                    {
                        // TODO
                    }
                    break;
                default:
                    {
                        ret = T3D_ERR_RES_INVALID_PROPERTY;
                        T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                            "Invalid factor of color_op_multipass_fallback !");
                    }
                    break;
                }
            }
            else if (argc == 2)
            {
                // src_factor
                uint16_t src = 0;
                bytesOfRead = stream.read(&src, sizeof(src));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(src),
                    "Read src_factor of color_op_multipass_fallback failed !");

                // dst_factor
                uint16_t dst = 0;
                bytesOfRead = stream.read(&dst, sizeof(dst));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(dst),
                    "Read dst_factor of color_op_multipass_fallback failed !");

                // TODO
            }
            else
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid number of arguments of \
                    color_op_multipass_fallback!");
                break;
            }
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
            size_t bytesOfRead = 0;

            // operation
            uint16_t op = 0;
            bytesOfRead = stream.read(&op, sizeof(op));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(op),
                "Read opration of alpha_op_ex failed !");

            // source1
            uint16_t source1 = 0;
            bytesOfRead = stream.read(&source1, sizeof(source1));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(source1),
                "Read source1 of alpha_op_ex failed !");

            // source2
            uint16_t source2 = 0;
            bytesOfRead = stream.read(&source2, sizeof(source2));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(source2),
                "Read source2 of alpha_op_ex failed !");

            float32_t manualBlend = 0.0f;
            ColorARGB arg1 = ColorARGB::WHITE;
            ColorARGB arg2 = ColorARGB::WHITE;

            switch (op)
            {
            case E_OP_BLEND_MANUAL:
                {
                    // manual_factor
                    bytesOfRead = stream.read(&manualBlend, sizeof(manualBlend));
                    T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(manualBlend),
                        "Read manual_factor of alpha_op_ex failed !");
                }
            case E_OP_SOURCE1:
            case E_OP_SOURCE2:
            case E_OP_MODULATE:
            case E_OP_MODULATE_X2:
            case E_OP_MODULATE_X4:
            case E_OP_ADD:
            case E_OP_ADD_SIGNED:
            case E_OP_ADD_SMOOTH:
            case E_OP_SUBTRACT:
            case E_OP_BLEND_DIFFUSE_ALPHA:
            case E_OP_BLEND_TEXTURE_ALPHA:
            case E_OP_BLEND_CURRENT_ALPHA:
            case  E_OP_DOT_PRODUCT:
            case E_OP_BLEND_DIFFUSE_COLOUR:
                {
                    // TODO
                }
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid operation of alpha_op_ex !");
                }
                break;
            }

            if (ret != T3D_OK)
            {
                break;
            }

            switch (source1)
            {
            case E_OP_SRC_MANUAL:
                {
                    // manual_color1
                    ret = parseColor(stream, arg1);
                    if (ret != T3D_OK)
                    {
                        T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                            "Read manual_color1 of alpha_op_ex failed !");
                        break;
                    }
                }
            case E_OP_SRC_CURRENT:
            case E_OP_SRC_TEXTURE:
            case E_OP_SRC_DIFFUSE:
            case E_OP_SRC_SPECULAR:
                {
                    // TODO
                }
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid source1 of alpha_op_ex !");
                }
                break;
            }

            if (ret != T3D_OK)
            {
                break;
            }

            switch (source2)
            {
            case E_OP_SRC_MANUAL:
                {
                    // manual_color1
                    ret = parseColor(stream, arg2);
                    if (ret != T3D_OK)
                    {
                        T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                            "Read manual_color2 of alpha_op_ex failed !");
                        break;
                    }
                }
            case E_OP_SRC_CURRENT:
            case E_OP_SRC_TEXTURE:
            case E_OP_SRC_DIFFUSE:
            case E_OP_SRC_SPECULAR:
                {
                    // TODO
                }
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid source2 of alpha_op !");
                }
                break;
            }

            if (ret != T3D_OK)
            {
                break;
            }

            // TODO
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
            size_t bytesOfRead = 0;

            // type
            uint16_t type = 0;
            bytesOfRead = stream.read(&type, sizeof(type));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                "Read type of env_map failed !");

            switch (type)
            {
            case E_BV_OFF:
            case E_OP_SPHERICAL:
            case E_OP_PLANAR:
            case E_OP_CUBIC_REFLECTION:
            case E_OP_CUBIC_NORMAL:
                {
                    // TODO
                }
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid type of env_map !");
                }
                break;
            }
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
            size_t bytesOfRead = 0;

            // x
            float32_t x;
            bytesOfRead = stream.read(&x, sizeof(x));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(x),
                "Read x of scroll failed !");

            // y
            float32_t y;
            bytesOfRead = stream.read(&y, sizeof(y));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(y),
                "Read y of scroll failed !");

            // TODO
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
            size_t bytesOfRead = 0;

            // x
            float32_t x;
            bytesOfRead = stream.read(&x, sizeof(x));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(x),
                "Read x of scroll failed !");

            // y
            float32_t y;
            bytesOfRead = stream.read(&y, sizeof(y));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(y),
                "Read y of scroll_anim failed !");

            // TODO
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
            size_t bytesOfRead = 0;

            // angle
            float32_t angle = 0.0f;
            bytesOfRead = stream.read(&angle, sizeof(angle));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(angle),
                "Read angle of rotate failed !");

            // TODO
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
            size_t bytesOfRead = 0;

            // revolutions per second
            float32_t velocity = 0.0f;
            bytesOfRead = stream.read(&velocity, sizeof(velocity));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(velocity),
                "Read velocity of rotate_anim failed !");

            // TODO
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
            size_t bytesOfRead = 0;

            // x
            float32_t scaleX;
            bytesOfRead = stream.read(&scaleX, sizeof(scaleX));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(scaleX),
                "Read x of scale failed !");

            // y
            float32_t scaleY;
            bytesOfRead = stream.read(&scaleY, sizeof(scaleY));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(scaleY),
                "Read y of scale failed !");

            // TODO
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
            size_t bytesOfRead = 0;

            // xform_type
            uint16_t type1 = 0;
            bytesOfRead = stream.read(&type1, sizeof(type1));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type1),
                "Read xform_type of wave_xform failed !");

            // wave_type
            uint16_t type2 = 0;
            bytesOfRead = stream.read(&type2, sizeof(type2));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type2),
                "Read wave_type of wave_xform failed !");

            // base
            float32_t base = 0.0f;
            bytesOfRead = stream.read(&base, sizeof(base));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(base),
                "Read base of wave_xform failed !");

            // frequency
            float32_t freq = 0.0f;
            bytesOfRead = stream.read(&freq, sizeof(freq));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(freq),
                "Read frequncy of wave_xform failed !");

            // phase
            float32_t phase = 0.0f;
            bytesOfRead = stream.read(&phase, sizeof(phase));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(phase),
                "Read phase of wave_xform failed !");

            // amplitude
            float32_t amp = 0.0f;
            bytesOfRead = stream.read(&amp, sizeof(amp));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(amp),
                "Read amplitude of wave_xform failed !");

            switch (type1)
            {
            case E_OP_SCROLL_X:
            case E_OP_SCROLL_Y:
            case E_OP_SCALE_X:
            case E_OP_SCALE_Y:
            case E_OP_ROTATE:
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid xform_type of wave_xform !");
                }
                break;
            }

            if (ret != T3D_OK)
            {
                break;
            }

            switch (type2)
            {
            case E_OP_SINE:
            case E_OP_TRIANGLE:
            case E_OP_SQUARE:
            case E_OP_SAWTOOTH:
            case E_OP_INVERSE_SAWTOOTH:
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid wave_type of wave_xform !");
                }
                break;
            }

            if (ret != T3D_OK)
            {
                break;
            }

            // TODO
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
            // matrix
            Matrix4 m;
            ret = parseMatrix(stream, m);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read matrix of transform failed !");
                break;
            }

            // TODO
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
            size_t bytesOfRead = 0;

            // binding type
            uint16_t type = 0;
            bytesOfRead = stream.read(&type, sizeof(type));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                "Read value of binding_type failed !");

            if (type == E_OP_FRAGMENT || type == E_OP_VERTEX)
            {
                // TODO
            }
            else
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid property of binding_type !");
            }
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
            // 暂时不支持该指令
            T3D_ASSERT(0);
        } while (0);

        return ret;
    }
}
