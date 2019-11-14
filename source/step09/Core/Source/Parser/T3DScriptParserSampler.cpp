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


#include "Parser/T3DScriptParserSampler.h"
#include "Parser/T3DScriptParser.h"
#include "Kernel/T3DTextureUnit.h"
#include "T3DErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ScriptParserSamplerPtr ScriptParserSampler::create()
    {
        ScriptParserSamplerPtr parser = new ScriptParserSampler();
        parser->release();
        return parser;
    }

    //--------------------------------------------------------------------------

    ScriptParserSampler::ScriptParserSampler()
    {

    }

    //--------------------------------------------------------------------------

    ScriptParserSampler::~ScriptParserSampler()
    {

    }

    //--------------------------------------------------------------------------

    TResult ScriptParserSampler::parseObject(
        DataStream &stream, Object *object, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
            size_t bytesOfRead = 0;

            TextureUnit *unit = static_cast<TextureUnit*>(object);

            // 属性数量
            uint16_t count = 0;
            bytesOfRead = stream.read(&count, sizeof(count));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(count),
                "Read the number of children of sampler failed !");

            // 名称
            String name;
            ret = parseString(stream, name);

            uint16_t type = E_NT_UNKNOWN;
            uint16_t i = 0;

            for (i = 0; i < count; ++i)
            {
                // Type
                bytesOfRead = stream.read(&type, sizeof(type));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                    "Read the type of property of sampler failed !");

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
                        "Invalid object type in sampler script !");
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

    TResult ScriptParserSampler::parseProperties(
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
                ret = parseTexAddressMode(stream, unit, version);
                break;
            case E_OP_TEX_BORDER_COLOUR:
                ret = parseTexBorderColor(stream, unit, version);
                break;
            case E_OP_FILTERING:
                ret = parseFiltering(stream, unit, version);
                break;
            case E_OP_CMPTEST:
                ret = parseCompareTest(stream, unit, version);
                break;
            case E_OP_CMPFUNC:
                ret = parseCompareFunc(stream, unit, version);
                break;
            case E_OP_MAX_ANISOTROPY:
                ret = parseMaxAnisotropy(stream, unit, version);
                break;
            case E_OP_MIPMAP_BIAS:
                ret = parseMipmapBias(stream, unit, version);
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

    TResult ScriptParserSampler::parseSamplerParams(
        DataStream &stream, Object *object, uint32_t version)
    {
        TextureUnit *unit = static_cast<TextureUnit*>(object);

        return parseProperties(stream, unit, version);
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserSampler::parseTexAddressMode(DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
            size_t bytesOfRead = 0;

            // number of arguments
            uint16_t argc = 0;
            bytesOfRead = stream.read(&argc, sizeof(argc));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(argc),
                "Read the number of arguments of tex_address_mode failed !");

            // texture address mode
            uint16_t type[3] = { 0 };
            bytesOfRead = stream.read(type, sizeof(uint16_t) * argc);

            for (uint16_t i = 0; i < argc; ++i)
            {
                if (type[i] != E_OP_WRAP && type[i] != E_OP_MIRROR
                    && type[i] != E_OP_CLAMP && type[i] != E_OP_BORDER)
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    break;
                }
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

    TResult ScriptParserSampler::parseTexBorderColor(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            ColorARGB color;
            ret = parseColor(stream, color);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read value of tex_border_color failed !");
                break;
            }

            // TODO
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserSampler::parseFiltering(
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
                "Read the number of argument of filtering failed !");

            if (argc == 1)
            {
                // filtering mode
                uint16_t mode = 0;
                bytesOfRead = stream.read(&mode, sizeof(mode));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(mode),
                    "Read mode of filtering failed !");

                // TODO
            }
            else if (argc == 3)
            {
                // minification
                uint16_t minification = 0;
                bytesOfRead = stream.read(&minification, sizeof(minification));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(minification),
                    "Read minification of filtering failed !");

                // magnification
                uint16_t magnification = 0;
                bytesOfRead = stream.read(&magnification, sizeof(magnification));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(magnification),
                    "Read magnification of filtering failed !");

                // mip
                uint16_t mip = 0;
                bytesOfRead = stream.read(&mip, sizeof(mip));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(mip),
                    "Read mip of filtering failed !");

                if (minification == E_OP_NONE || minification == E_OP_LINEAR
                    || minification == E_OP_BILINEAR
                    || minification == E_OP_TRILINEAR
                    || minification == E_OP_ANISOTROPIC)
                {
                    // TODO
                }
                else
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid minification of filtering !");
                    break;
                }

                if (magnification == E_OP_NONE || magnification == E_OP_LINEAR
                    || magnification == E_OP_BILINEAR
                    || magnification == E_OP_TRILINEAR
                    || magnification == E_OP_ANISOTROPIC)
                {
                    // TODO
                }
                else
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid magnification of filtering !");
                    break;
                }

                if (mip == E_OP_NONE || mip == E_OP_LINEAR
                    || mip == E_OP_BILINEAR || mip == E_OP_TRILINEAR
                    || mip == E_OP_ANISOTROPIC)
                {
                    // TODO
                }
                else
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid mip of filtering !");
                    break;
                }
            }
            else
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid number of arguments of filtering !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserSampler::parseCompareTest(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // enabled flag
            bool val = false;
            bytesOfRead = stream.read(&val, sizeof(val));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val),
                "Read value of cmp_test failed !");

            // TODO
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserSampler::parseCompareFunc(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // value
            uint16_t val = 0;
            bytesOfRead = stream.read(&val, sizeof(val));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val),
                "Read value of cmp_func failed !");

            switch (val)
            {
            case E_OP_ALWAYS_FAIL:
            case E_OP_ALWAYS_PASS:
            case E_OP_LESS_EQUAL:
            case E_OP_LESS:
            case E_OP_EQUAL:
            case E_OP_NOT_EQUAL:
            case E_OP_GREATER_EQUAL:
            case E_OP_GREATER:
                {
                    // TODO
                }
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid value of cmp_func !");
                }
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserSampler::parseMaxAnisotropy(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // value
            float32_t val;
            bytesOfRead = stream.read(&val, sizeof(val));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val),
                "Read value of max_anisotropy failed !");

            // TODO
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserSampler::parseMipmapBias(
        DataStream &stream, TextureUnit *unit, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // value
            float32_t val = 0;
            bytesOfRead = stream.read(&val, sizeof(val));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val),
                "Read value of mipmap_bias failed !");

            // TODO
        } while (0);

        return ret;
    }
}
