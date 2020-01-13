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


#include "Parser/T3DScriptParserSampler.h"
#include "Parser/T3DScriptParser.h"
#include "Kernel/T3DTextureUnit.h"
#include "T3DErrorDef.h"
#include "Resource/T3DSampler.h"
#include "Resource/T3DSamplerManager.h"
#include "Resource/T3DMaterial.h"


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

//             TextureUnit *unit = static_cast<TextureUnit*>(object);

            // 属性数量
            uint16_t count = 0;
            bytesOfRead = stream.read(&count, sizeof(count));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(count),
                "Read the number of children of sampler failed !");

            // 名称
            String name;
            ret = parseString(stream, name);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read the name of sampler failed !");
                break;
            }

            if (name.empty())
            {
                name = generateName("Sampler");
            }

            // MD5
            uint8_t hash[16];
            bytesOfRead = stream.read(hash, sizeof(hash));
            T3D_CHECK_READ_CONTENT(bytesOfRead, 16,
                "Read hash of sampler failed !");

            SamplerPtr sampler = T3D_SAMPLER_MGR.loadSampler(name);

            uint16_t type = UNKNOWN;
            uint16_t i = 0;

            for (i = 0; i < count; ++i)
            {
                // Type
                bytesOfRead = stream.read(&type, sizeof(type));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                    "Read the type of property of sampler failed !");

                if (type == PROPERTY)
                {
                    ret = parseProperties(stream, sampler, version);
                }
                else if (type == OBJECT)
                {
                    ret = parseObjects(stream, sampler, version);
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
        DataStream &stream, Sampler *sampler, uint32_t version)
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
                ret = parseTexAddressMode(stream, sampler, version);
                break;
            case E_OP_TEX_BORDER_COLOUR:
                ret = parseTexBorderColor(stream, sampler, version);
                break;
            case E_OP_FILTERING:
                ret = parseFiltering(stream, sampler, version);
                break;
            case E_OP_CMPTEST:
                ret = parseCompareTest(stream, sampler, version);
                break;
            case E_OP_CMPFUNC:
            case E_OP_COMP_FUNC:
                ret = parseCompareFunc(stream, sampler, version);
                break;
            case E_OP_MAX_ANISOTROPY:
                ret = parseMaxAnisotropy(stream, sampler, version);
                break;
            case E_OP_MIPMAP_BIAS:
                ret = parseMipmapBias(stream, sampler, version);
                break;
            default:
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid property of sampler !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

//     TResult ScriptParserSampler::parseSamplerParams(
//         DataStream &stream, Object *object, uint32_t version)
//     {
//         TextureUnit *unit = static_cast<TextureUnit*>(object);
// 
//         return parseProperties(stream, unit, version);
//     }

    //--------------------------------------------------------------------------

    TResult ScriptParserSampler::parseTexAddressMode(
        DataStream &stream, Sampler *sampler, uint32_t version)
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

            if (argc == 1)
            {
                // u = v = w
                TextureAddressMode mode = toTexAddressMode(type[0]);
                sampler->setAddressMode(mode);
            }
            else if (argc == 2)
            {
                // u & v
                TextureAddressMode u = toTexAddressMode(type[0]);
                TextureAddressMode v = toTexAddressMode(type[1]);
                sampler->setAddressMode(u, v, TextureAddressMode::WRAP);
            }
            else if (argc == 3)
            {
                // u, v, w
                TextureAddressMode u = toTexAddressMode(type[0]);
                TextureAddressMode v = toTexAddressMode(type[1]);
                TextureAddressMode w = toTexAddressMode(type[2]);
                sampler->setAddressMode(u, v, w);
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserSampler::parseTexBorderColor(
        DataStream &stream, Sampler *sampler, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            ColorRGBA color;
            ret = parseColor(stream, color);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read value of tex_border_color failed !");
                break;
            }

            sampler->setBorderColor(color);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserSampler::parseFiltering(
        DataStream &stream, Sampler *sampler, uint32_t version)
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

                FilterOptions min, mag, mip;

                switch (mode)
                {
                case E_OP_NONE:
                    min = FilterOptions::POINT;
                    mag = FilterOptions::POINT;
                    mip = FilterOptions::NONE;
                    break;
                case E_OP_BILINEAR:
                    min = FilterOptions::LINEAR;
                    mag = FilterOptions::LINEAR;
                    mip = FilterOptions::POINT;
                case E_OP_TRILINEAR:
                    min = FilterOptions::LINEAR;
                    mag = FilterOptions::LINEAR;
                    mip = FilterOptions::LINEAR;
                    break;
                case E_OP_ANISOTROPIC:
                    min = FilterOptions::ANISOTROPIC;
                    mag = FilterOptions::ANISOTROPIC;
                    mip = FilterOptions::LINEAR;
                    break;
                default:
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid sampler filtering value !");
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    break;
                }

                sampler->setFilter(min, mag, mip);
            }
            else if (argc == 3)
            {
                // minification
                uint16_t min = 0;
                bytesOfRead = stream.read(&min, sizeof(min));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(min),
                    "Read minification of filtering failed !");

                // magnification
                uint16_t mag = 0;
                bytesOfRead = stream.read(&mag, sizeof(mag));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(mag),
                    "Read magnification of filtering failed !");

                // mip
                uint16_t mip = 0;
                bytesOfRead = stream.read(&mip, sizeof(mip));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(mip),
                    "Read mip of filtering failed !");

                FilterOptions minOpt, magOpt, mipOpt;

                if (min == E_OP_POINT || min == E_OP_LINEAR
                    || min == E_OP_ANISOTROPIC)
                {
                    minOpt = toFilterOptions(min);
                }
                else
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid minification of filtering !");
                    break;
                }

                if (mag == E_OP_POINT || mag == E_OP_LINEAR
                    || mag == E_OP_ANISOTROPIC)
                {
                    magOpt = toFilterOptions(mag);
                }
                else
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid magnification of filtering !");
                    break;
                }

                if (mip == E_OP_POINT || mip == E_OP_LINEAR
                    || mip == E_OP_ANISOTROPIC)
                {
                    mipOpt = toFilterOptions(mip);
                }
                else
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid mip of filtering !");
                    break;
                }

                sampler->setFilter(minOpt, magOpt, mipOpt);
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
        DataStream &stream, Sampler *sampler, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // enabled flag
            bool enabled = false;
            bytesOfRead = stream.read(&enabled, sizeof(enabled));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(enabled),
                "Read value of cmp_test failed !");

            sampler->setCompareEnabled(enabled);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserSampler::parseCompareFunc(
        DataStream &stream, Sampler *sampler, uint32_t version)
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
                    CompareFunction func = toCompareFunction(val);
                    sampler->setCompareFunction(func);
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
        DataStream &stream, Sampler *sampler, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // value
            uint32_t val;
            bytesOfRead = stream.read(&val, sizeof(val));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val),
                "Read value of max_anisotropy failed !");

            sampler->setAnisotropy(val);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserSampler::parseMipmapBias(
        DataStream &stream, Sampler *sampler, uint32_t version)
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

            sampler->setMipmapBias(val);
        } while (0);

        return ret;
    }
}
