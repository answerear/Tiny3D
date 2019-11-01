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


#include "Kernel/T3DScriptParserMaterial.h"
#include "Kernel/T3DScriptParser.h"
#include "T3DErrorDef.h"
#include "Resource/T3DMaterial.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ScriptParserMaterialPtr ScriptParserMaterial::create()
    {
        ScriptParserMaterialPtr parser = new ScriptParserMaterial();
        parser->release();
        return parser;
    }

    //--------------------------------------------------------------------------

    ScriptParserMaterial::ScriptParserMaterial()
    {

    }

    //--------------------------------------------------------------------------

    ScriptParserMaterial::~ScriptParserMaterial()
    {

    }

    //--------------------------------------------------------------------------

    TResult ScriptParserMaterial::parseObject(
        DataStream &stream, 
        Object *object,
        uint32_t version)
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
                    ret = parseMaterialProperties(stream, material, version);
                }
                else if (type == E_NT_OBJECT)
                {
                    
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

    TResult ScriptParserMaterial::parseMaterialProperties(DataStream &stream,
        Material *material, uint32_t version)
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
}
