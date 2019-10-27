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
        : ScriptParserBase()
    {
    }

    //--------------------------------------------------------------------------

    ScriptParser::~ScriptParser()
    {
    }

    //--------------------------------------------------------------------------

    TResult ScriptParser::parse(DataStream &stream, Object *object)
    {
        TResult ret = T3D_OK;

        do 
        {
            Material *material = static_cast<Material*>(object);

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

    TResult ScriptParser::parse_Ver00000100(DataStream &stream, Material *material)
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

            ret = parseObject(stream, material);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParser::parseObject(DataStream &stream, Object *object)
    {
        TResult ret = T3D_OK;

        do 
        {
            size_t bytesOfRead = 0;

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

            switch (opcode)
            {
            case E_OP_MATERIAL:
                ret = parseMaterial(stream, object);
                break;
            case E_OP_TECHNIQUE:
                ret = parseTechnique(stream, object);
                break;
            case E_OP_PASS:
                ret = parsePass(stream, object);
            default:
                break;
            }

        } while (0);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParser::parseMaterial(DataStream &stream, Object *object)
    {
        TResult ret = T3D_OK;

        do 
        {
            size_t bytesOfRead = 0;

            Material *material = static_cast<Material*>(object);

            // 属性数量
            uint16_t count = 0;
            bytesOfRead = stream.read(&count, sizeof(count));
            if (bytesOfRead != sizeof(count))
            {
                ret = T3D_ERR_RES_INVALID_CONTENT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, 
                    "Read the number of children of material failed !");
                break;
            }

            // 名称
            String name;
            ret = parseString(stream, name);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, 
                    "Read the name of material failed !");
                break;
            }

            material->setMaterialName(name);

            uint16_t type = E_NT_UNKNOWN;
            uint16_t i = 0;

            for (i = 0; i < count; ++i)
            {
                // Type
                bytesOfRead = stream.read(&type, sizeof(type));
                if (bytesOfRead != sizeof(type))
                {
                    ret = T3D_ERR_RES_INVALID_CONTENT;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE, 
                        "Read the type of property of material failed !");
                    break;
                }

                if (type == E_NT_PROPERTY)
                {
                    ret = parseMaterialProperties(stream, material);
                }
                else if (type == E_NT_OBJECT)
                {
                    ret = parseObject(stream, material);
                }
                else
                {
                    ret = T3D_ERR_RES_INVALID_OBJECT;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE, 
                        "Invalid object type in material script !");
                    break;
                }
            }

            if (ret != T3D_OK)
            {
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParser::parseMaterialProperties(DataStream &stream, 
        Material *material)
    {
        TResult ret = T3D_OK;

        do 
        {
            size_t bytesOfRead = 0;

            // ID
            uint16_t id;
            bytesOfRead = stream.read(&id, sizeof(id));
            if (bytesOfRead != sizeof(id))
            {
                ret = T3D_ERR_RES_INVALID_CONTENT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, 
                    "Read ID of property of material failed !");
                break;
            }

            switch (id)
            {
            case E_OP_LOD_VALUES:
                {
                    // The number of LOD
                    uint16_t count = 0;
                    bytesOfRead = stream.read(&count, sizeof(count));
                    if (bytesOfRead != sizeof(count))
                    {
                        ret = T3D_ERR_RES_INVALID_CONTENT;
                        T3D_LOG_ERROR(LOG_TAG_RESOURCE, 
                            "Read the number of LOD failed !");
                        break;
                    }

                    // LOD value
                    uint16_t i = 0;
                    for (i = 0; i < count; ++i)
                    {
                        float32_t value;
                        bytesOfRead = stream.read(&value, sizeof(value));
                        if (bytesOfRead != sizeof(value))
                        {
                            ret = T3D_ERR_RES_INVALID_CONTENT;
                            T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                                "Read the value of LOD failed !");
                            break;
                        }
                    }
                }
                break;
            case E_OP_RECEIVE_SHADOWS:
                {
                    // receiving shadows
                    bool enable = false;
                    bytesOfRead = stream.read(&enable, sizeof(enable));
                    if (bytesOfRead != sizeof(enable))
                    {
                        ret = T3D_ERR_RES_INVALID_CONTENT;
                        T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                            "Read the receive shadows failed !");
                        break;
                    }
                }
                break;
            case E_OP_TRANSPARENCY_CASTS_SHADOWS:
                {
                    // transparency cast shadows
                    bool enable = false;
                    bytesOfRead = stream.read(&enable, sizeof(enable));
                    if (bytesOfRead != sizeof(enable))
                    {
                        ret = T3D_ERR_RES_INVALID_CONTENT;
                        T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                            "Read the transparency cast shadows failed !");
                        break;
                    }
                }
                break;
            case E_OP_SET_TEXTURE_ALIAS:
                {
                    String aliasName;
                    ret = parseString(stream, aliasName);

                    if (ret != T3D_OK)
                    {
                        ret = T3D_ERR_RES_INVALID_CONTENT;
                        T3D_LOG_ERROR(LOG_TAG_RESOURCE, 
                            "Read alias name failed !");
                        break;
                    }

                    String textureName;
                    ret = parseString(stream, textureName);

                    if (ret != T3D_OK)
                    {
                        ret = T3D_ERR_RES_INVALID_CONTENT;
                        T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                            "Read texture name failed !");
                        break;
                    }
                }
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE, 
                        "Invalid property of material !");
                }
                break;
            }

            if (ret != T3D_OK)
            {
                break;
            }
        } while (0);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParser::parseTechnique(DataStream &stream, Object *object)
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParser::parsePass(DataStream &stream, Object *object)
    {
        TResult ret = T3D_OK;

        return ret;
    }
}
