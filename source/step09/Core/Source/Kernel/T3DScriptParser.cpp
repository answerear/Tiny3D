﻿/*******************************************************************************
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
#include "T3DErrorDef.h"
#include "Kernel/T3DScriptParserMaterial.h"
#include "Kernel/T3DScriptParserTechnique.h"
#include "Kernel/T3DScriptParserPass.h"


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
        : ScriptParserBase()
    {
    }

    //--------------------------------------------------------------------------

    ScriptParser::~ScriptParser()
    {
    }

    //--------------------------------------------------------------------------

    ScriptParserBase *ScriptParser::getObjectParser(uint16_t opcode)
    {
        ScriptParserBase *parser = nullptr;

        switch (opcode)
        {
        case E_OP_MATERIAL:
            parser = mMaterialParser;
            break;
        case E_OP_TECHNIQUE:
            parser = mTechParser;
            break;
        case E_OP_PASS:
            parser = mPassParser;
            break;
        }

        return parser;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParser::parse(DataStream &stream, Object *object)
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

            ret = parseObject(stream, object, header.version);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParser::parseObject(DataStream &stream, Object *object, uint32_t version)
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

            ScriptParserBase *parser = getObjectParser(opcode);
            if (parser == nullptr)
            {
                ret = T3D_ERR_RES_INVALID_OBJECT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid op code ! Here need Material op code !");
                break;
            }

            ret = parser->parseObject(stream, object, version);
        } while (0);
        
        return ret;
    }
}