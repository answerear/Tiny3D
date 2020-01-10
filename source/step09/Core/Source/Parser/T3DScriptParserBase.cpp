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


#include "Parser/T3DScriptParserBase.h"
#include "Parser/T3DScriptParser.h"
#include "T3DErrorDef.h"
#include <sstream>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    T3D_IMPLEMENT_CLASS_BASECLASS_1(ScriptParserBase, Object);

    //--------------------------------------------------------------------------

    ScriptParserBase::ScriptParserBase()
        : mText(nullptr)
        , mTextLen(0)
        , mNameSuffix(0)
    {
        const size_t MAX_STR_SIZE = 1024;
        mText = new char[MAX_STR_SIZE];
        mTextLen = MAX_STR_SIZE;
    }

    //--------------------------------------------------------------------------

    ScriptParserBase::~ScriptParserBase()
    {
        T3D_SAFE_DELETE_ARRAY(mText);
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserBase::parseObjects(
        DataStream &stream, Object *object, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // Op code
            uint16_t opcode = 0;
            bytesOfRead = stream.read(&opcode, sizeof(opcode));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(opcode),
                "Read the opcode of property of object failed !");

            ScriptParserBase *parser = T3D_SCRIPT_PARSER.getObjectParser(opcode);
            if (parser == nullptr)
            {
                ret = T3D_ERR_RES_INVALID_OBJECT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid object in script !");
                break;
            }

            ret = parser->parseObject(stream, object, version);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserBase::parseString(DataStream &stream, String &str)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // 读取字符串长度
            uint16_t len = 0;
            bytesOfRead = stream.read(&len, sizeof(len));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(len), 
                "Read the length of string failed !");

            // 读取字符串内容
            if (len > mTextLen)
            {
                T3D_SAFE_DELETE_ARRAY(mText);
                mText = new char[len];
                mTextLen = len;
            }

            if (len > 0)
            {
                bytesOfRead = stream.read(mText, len);
                T3D_CHECK_READ_CONTENT(bytesOfRead, len,
                    "Read the string content failed !");

                str.assign(mText, len);
            }
            else
            {
                str.clear();
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserBase::parseColor(DataStream &stream, ColorRGBA &color)
    {
        TResult ret = T3D_OK;

        do 
        {
            size_t bytesOfRead = 0;

            float32_t val;
            bytesOfRead = stream.read(&val, sizeof(val));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val),
                "Read the blue component in the color failed !");
            color.blue() = val;

            bytesOfRead = stream.read(&val, sizeof(val));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val),
                "Read the green component in the color failed !");
            color.green() = val;

            bytesOfRead = stream.read(&val, sizeof(val));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val),
                "Read the red component in the color failed !");
            color.red() = val;

            bytesOfRead = stream.read(&val, sizeof(val));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val),
                "Read the alpha component in the color failed !");
            color.alpha() = val;
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserBase::parseMatrix(DataStream &stream, Matrix4 &m)
    {
        TResult ret = T3D_OK;

        do 
        {
            size_t bytesOfRead = 0;

            for (size_t y = 0; y < 4; ++y)
            {
                for (size_t x = 0; x < 4; ++x)
                {
                    float32_t val;
                    bytesOfRead = stream.read(&val, sizeof(val));
                    T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(val),
                        "Read tuple of matrix failed !");

                    m[y][x] = val;
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

    String ScriptParserBase::generateName(const String &prefix)
    {
        std::stringstream ss;
        ss.width(3);
        ss.fill('0');
        ss << prefix << "_Noname#" << mNameSuffix;
        mNameSuffix++;
        return ss.str();
    }

    //--------------------------------------------------------------------------

    BlendType ScriptParserBase::toBlendType(uint16_t type) const
    {
        BlendType bt;

        switch (type)
        {
        case E_OP_ADD:
            bt = BlendType::ADD;
            break;
        case E_OP_MODULATE:
            bt = BlendType::MODULATE;
            break;
        case E_OP_COLOUR_BLEND:
            bt = BlendType::TRANSPARENT_COLOR;
            break;
        case E_OP_ALPHA_BLEND:
            bt = BlendType::TRANSPARENT_ALPHA;
            break;
        case E_OP_REPLACE:
            bt = BlendType::REPLACE;
            break;
        }

        return bt;
    }

    //--------------------------------------------------------------------------

    BlendFactor ScriptParserBase::toBlendFactor(uint16_t factor) const
    {
        BlendFactor bf;

        switch (factor)
        {
        case E_OP_ONE:
            bf = BlendFactor::ONE;
            break;
        case E_OP_ZERO:
            bf = BlendFactor::ZERO;
            break;
        case E_OP_DEST_COLOUR:
            bf = BlendFactor::DEST_COLOR;
            break;
        case E_OP_SRC_COLOUR:
            bf = BlendFactor::SOURCE_COLOR;
            break;
        case E_OP_ONE_MINUS_DEST_COLOUR:
            bf = BlendFactor::DEST_COLOR;
            break;
        case E_OP_ONE_MINUS_SRC_COLOUR:
            bf = BlendFactor::ONE_MINUS_SOURCE_COLOR;
            break;
        case E_OP_DEST_ALPHA:
            bf = BlendFactor::DEST_ALPHA;
            break;
        case E_OP_SRC_ALPHA:
            bf = BlendFactor::SOURCE_ALPHA;
            break;
        case E_OP_ONE_MINUS_DEST_ALPHA:
            bf = BlendFactor::ONE_MINUS_DEST_ALPHA;
            break;
        case E_OP_ONE_MINUS_SRC_ALPHA:
            bf = BlendFactor::ONE_MINUS_SOURCE_ALPHA;
            break;
        }

        return bf;
    }

    //--------------------------------------------------------------------------

    BlendOperation ScriptParserBase::toBlendOperation(uint16_t op) const
    {
        BlendOperation bo;

        switch (op)
        {
        case E_OP_ADD:
            bo = BlendOperation::ADD;
            break;
        case E_OP_SUBTRACT:
            bo = BlendOperation::SUBTRACT;
            break;
        case E_OP_REVERSE_SUBTRACT:
            bo = BlendOperation::REVERSE_SUBTRACT;
            break;
        case E_OP_MIN:
            bo = BlendOperation::MIN;
            break;
        case E_OP_MAX:
            bo = BlendOperation::MAX;
            break;
        }

        return bo;
    }

    //--------------------------------------------------------------------------

    CompareFunction ScriptParserBase::toCompareFunction(uint16_t func) const
    {
        CompareFunction cf;

        switch (func)
        {
        case E_OP_ALWAYS_FAIL:
            cf = CompareFunction::ALWAYS_FAIL;
            break;
        case E_OP_ALWAYS_PASS:
            cf = CompareFunction::ALWAYS_PASS;
            break;
        case E_OP_LESS:
            cf = CompareFunction::LESS;
            break;
        case E_OP_LESS_EQUAL:
            cf = CompareFunction::LESS_EQUAL;
            break;
        case E_OP_EQUAL:
            cf = CompareFunction::EQUAL;
            break;
        case E_OP_NOT_EQUAL:
            cf = CompareFunction::NOT_EQUAL;
            break;
        case E_OP_GREATER_EQUAL:
            cf = CompareFunction::GREATER_EQUAL;
            break;
        case E_OP_GREATER:
            cf = CompareFunction::GREATER;
            break;
        }

        return cf;
    }

    //--------------------------------------------------------------------------

    IlluminationStage ScriptParserBase::toIlluminationStage(uint16_t stage) const
    {
        IlluminationStage is;

        switch (stage)
        {
        case E_OP_AMBIENT:
            is = IlluminationStage::AMBIENT;
            break;
        case E_OP_PER_LIGHT:
            is = IlluminationStage::PER_LIGHT;
            break;
        case E_OP_DECAL:
            is = IlluminationStage::DECAL;
            break;
        default:
            is = IlluminationStage::UNKNOWN;
        }

        return is;
    }

    //--------------------------------------------------------------------------

    CullingMode ScriptParserBase::toCullingMode(uint16_t mode) const
    {
        CullingMode cm;

        switch (mode)
        {
        case E_OP_CLOCKWISE:
            cm = CullingMode::CLOCKWISE;
            break;
        case E_OP_ANTICLOCKWISE:
            cm = CullingMode::ANTICLOCKWISE;
            break;
        case E_OP_NONE:
            cm = CullingMode::NONE;
            break;
        }

        return cm;
    }

    //--------------------------------------------------------------------------

    ManualCullingMode ScriptParserBase::toManualCullingMode(uint16_t mode) const
    {
        ManualCullingMode cm;

        switch (mode)
        {
        case E_OP_FRONT:
            cm = ManualCullingMode::FRONT;
            break;
        case E_OP_BACK:
            cm = ManualCullingMode::BACK;
            break;
        case E_OP_NONE:
            cm = ManualCullingMode::NONE;
            break;
        }

        return cm;
    }

    //--------------------------------------------------------------------------

    ShadingMode ScriptParserBase::toShadingMode(uint16_t mode) const
    {
        ShadingMode sm;

        switch (mode)
        {
        case E_OP_FLAT:
            sm = ShadingMode::FLAT;
            break;
        case E_OP_GOURAUD:
            sm = ShadingMode::GOURAUD;
            break;
        case E_OP_PHONG:
            sm = ShadingMode::PHONG;
        }

        return sm;
    }

    //--------------------------------------------------------------------------

    PolygonMode ScriptParserBase::toPolygonMode(uint16_t mode) const
    {
        PolygonMode pm;

        switch (mode)
        {
        case E_OP_SOLID:
            pm = PolygonMode::SOLID;
            break;
        case E_OP_POINTS:
            pm = PolygonMode::POINT;
            break;
        case E_OP_WIREFRAME:
            pm = PolygonMode::WIREFRAME;
            break;
        }

        return pm;
    }

    //--------------------------------------------------------------------------

    FogMode ScriptParserBase::toFogMode(uint16_t mode) const
    {
        FogMode fm;

        switch (mode)
        {
        case E_OP_NONE:
            fm = FogMode::NONE;
            break;
        case E_OP_LINEAR:
            fm = FogMode::LINEAR;
            break;
        case E_OP_EXP:
            fm = FogMode::EXP;
            break;
        case E_OP_EXP2:
            fm = FogMode::EXP2;
            break;
        }

        return fm;
    }

    //--------------------------------------------------------------------------

    LightType ScriptParserBase::toLightType(uint16_t type) const
    {
        LightType lt;

        switch (type)
        {
        case E_OP_POINT:
            lt = LightType::POINT;
            break;
        case E_OP_DIRECTIONAL:
            lt = LightType::DIRECTIONAL;
            break;
        case E_OP_SPOT:
            lt = LightType::SPOTLIGHT;
            break;
        }

        return lt;
    }

    //--------------------------------------------------------------------------

    TextureAddressMode ScriptParserBase::toTexAddressMode(uint16_t tam) const
    {
        TextureAddressMode mode;

        switch (tam)
        {
        case E_OP_WRAP:
            mode = TextureAddressMode::WRAP;
            break;
        case E_OP_CLAMP:
            mode = TextureAddressMode::CLAMP;
            break;
        case E_OP_BORDER:
            mode = TextureAddressMode::BORDER;
            break;
        case E_OP_MIRROR:
            mode = TextureAddressMode::MIRROR;
            break;
        default:
            mode = TextureAddressMode::UNKNOWN;
            break;
        }

        return mode;
    }

    //--------------------------------------------------------------------------

    FilterOptions ScriptParserBase::toFilterOptions(uint16_t filter) const
    {
        FilterOptions opt;

        switch (filter)
        {
        case E_OP_POINT:
            opt = FilterOptions::POINT;
            break;
        case E_OP_LINEAR:
            opt = FilterOptions::LINEAR;
            break;
        case E_OP_ANISOTROPIC:
            opt = FilterOptions::ANISOTROPIC;
            break;
        default:
            break;
        }

        return opt;
    }
}
