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

#include "Kernel/T3DScriptParser.h"
#include "Resource/T3DMaterial.h"
#include "T3DErrorDef.h"


namespace Tiny3D
{
    #define T3D_TSC_VERSION_00000100            0x00000100
    #define T3D_TSC_CURRENT_VERSION             T3D_TSC_VERSION_00000100
    #define T3D_TSC_MAGIC                       "TSC"

    //--------------------------------------------------------------------------

    T3D_INIT_SINGLETON(ScriptParser);

    //--------------------------------------------------------------------------

    ScriptParserPtr ScriptParser::create()
    {
        ScriptParserPtr parser = new ScriptParser();
        parser->release();
        return parser;
    }

    //--------------------------------------------------------------------------

    ScriptParser::ScriptParser()
        : mText(nullptr)
    {
        const size_t MAX_STR_SIZE = 1024;
        mText = new char[MAX_STR_SIZE];
        mTextLen = MAX_STR_SIZE;
    }

    //--------------------------------------------------------------------------

    ScriptParser::~ScriptParser()
    {
        T3D_SAFE_DELETE_ARRAY(mText);
    }

    //--------------------------------------------------------------------------

    TResult ScriptParser::parse(DataStream &stream, Material &material)
    {
        TResult ret = T3D_OK;

        do 
        {
            // 读取文件头
            TSCFileHeader header;
            stream.read(&header, sizeof(header));

            if (stricmp(header.magic, T3D_TSC_MAGIC) != 0)
            {
                // 非法的文件类型
                ret = T3D_ERR_RES_INVALID_FILETYPE;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid file type !");
                break;
            }

            // 根据版本解析
            switch (header.version)
            {
            case T3D_TSC_VERSION_00000100:
                ret = parse_Ver00000100(stream, material);
                break;
            default:
                ret = T3D_ERR_RES_INVALID_VERSION;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid file version !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParser::parse_Ver00000100(DataStream &stream, Material &material)
    {
        TResult ret = T3D_OK;

        do 
        {
            size_t bytesOfRead = 0;

            // 类型
            uint16_t type = 0;
            bytesOfRead = stream.read(&type, sizeof(type));
            if (bytesOfRead != sizeof(type))
            {
                // 文件出错了
                ret = T3D_ERR_RES_INVALID_CONTENT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Read object type failed !");
                break;
            }

            if (type != E_NT_OBJECT)
            {
                // 错误类型，这里需要对象
                ret = T3D_ERR_RES_INVALID_OBJECT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, 
                    "Invalid object ! Here need material object !");
                break;
            }

            // Op Code
            uint16_t opcode = 0;
            bytesOfRead = stream.read(&opcode, sizeof(opcode));
            if (bytesOfRead != sizeof(opcode))
            {
                // 文件出错了
                ret = T3D_ERR_RES_INVALID_CONTENT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Read op code failed !");
                break;
            }

            if (opcode != E_OP_MATERIAL)
            {
                ret = T3D_ERR_RES_INVALID_OBJECT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, 
                    "Invalid op code ! Herer need Material op code !");
                break;
            }

            // 名称
            String name;
            ret = readString(stream, name);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Read material name failed !");
                break;
            }

            material.setMaterialName(name);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParser::readString(DataStream &stream, String &str)
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
