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


#include "Kernel/T3DScriptParserBase.h"
#include "Kernel/T3DScriptParser.h"
#include "T3DErrorDef.h"
#include <sstream>


namespace Tiny3D
{
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

            ScriptParserBase *parser
                = ScriptParser::getInstance().getObjectParser(opcode);
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

    TResult ScriptParserBase::parseColor(DataStream &stream, ColorARGB &color)
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
}
