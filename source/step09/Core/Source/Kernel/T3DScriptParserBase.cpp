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
#include "T3DErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ScriptParserBase::ScriptParserBase()
        : mText(nullptr)
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

    TResult ScriptParserBase::parseString(DataStream &stream, String &str)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // 读取字符串长度
            uint16_t len = 0;
            bytesOfRead = stream.read(&len, sizeof(len));
            if (bytesOfRead != sizeof(len))
            {
                ret = T3D_ERR_RES_INVALID_CONTENT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read the length of string failed !");
                break;
            }

            // 读取字符串内容
            if (len > mTextLen)
            {
                T3D_SAFE_DELETE_ARRAY(mText);
                mText = new char[len];
                mTextLen = len;
            }

            bytesOfRead = stream.read(mText, len);
            if (bytesOfRead != len)
            {
                ret = T3D_ERR_RES_INVALID_CONTENT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read the string content failed !");
                break;
            }

            str.assign(mText, len);
        } while (0);

        return ret;
    }
}
