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


#include "Parser/T3DScriptParserMaterial.h"
#include "Parser/T3DScriptParser.h"
#include "T3DErrorDef.h"
#include "Resource/T3DMaterial.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    T3D_IMPLEMENT_CLASS_1(ScriptParserMaterial, ScriptParserBase);

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
        DataStream &stream, Object *object, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            Material *material = static_cast<Material*>(object);

            // 属性和子对象数量
            uint16_t count = 0;
            bytesOfRead = stream.read(&count, sizeof(count));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(count),
                "Read the number of children of material failed !");

            // 名称
            String name;
            ret = parseString(stream, name);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read the name of material failed !");
                break;
            }

            // MD5
            uint8_t hash[16];
            bytesOfRead = stream.read(hash, sizeof(hash));
            T3D_CHECK_READ_CONTENT(bytesOfRead, 16,
                "Read hash of material failed !");

            material->setMaterialName(name);

            uint16_t type = UNKNOWN;
            uint16_t i = 0;

            for (i = 0; i < count; ++i)
            {
                // Type
                bytesOfRead = stream.read(&type, sizeof(type));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                    "Read the type of property of material failed !");

                if (type == PROPERTY)
                {
                    ret = parseProperties(stream, material, version);
                }
                else if (type == OBJECT)
                {
                    ret = parseObjects(stream, material, version);
                }
                else
                {
                    ret = T3D_ERR_RES_INVALID_OBJECT;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid object type in material script !");
                    break;
                }

                if (T3D_FAILED(ret))
                {
                    break;
                }
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserMaterial::parseProperties(DataStream &stream,
        Material *material, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // ID
            uint16_t id;
            bytesOfRead = stream.read(&id, sizeof(id));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(id),
                "Read ID of property of material failed !");

            switch (id)
            {
            case E_OP_LOD_VALUES:
                // lod_values
                ret = parseLODValus(stream, material, version);
                break;
            case E_OP_LOD_STRATEGY:
                // lod_strategy
                ret = parseLODStrategy(stream, material, version);
                break;
            case E_OP_RECEIVE_SHADOWS:
                // receive_shadows
                ret = parseReceiveShadows(stream, material, version);
                break;
            case E_OP_TRANSPARENCY_CASTS_SHADOWS:
                // transparency_casts_shadows
                ret = parseTransparencyCastsShadows(stream, material, version);
                break;
            case E_OP_SET_TEXTURE_ALIAS:
                // set_texture_alias
                ret = parseSetTextureAlias(stream, material, version);
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid property of material !");
                }
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserMaterial::parseLODValus(
        DataStream &stream, Material *material, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
            size_t bytesOfRead = 0;

            // The number of LOD
            uint16_t count = 0;
            bytesOfRead = stream.read(&count, sizeof(count));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(count), 
                "Read the number of LOD failed !");

            // LOD values
            uint16_t i = 0;
            for (i = 0; i < count; ++i)
            {
                float32_t value;
                bytesOfRead = stream.read(&value, sizeof(value));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(value), 
                    "Read the value of LOD failed !");
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserMaterial::parseLODStrategy(
        DataStream &stream, Material *material, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
            // LOD strategy
            String strategy;
            ret = parseString(stream, strategy);
            if (T3D_FAILED(ret))
            {
                break;
            }

        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserMaterial::parseReceiveShadows(
        DataStream &stream, Material *material, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
            size_t bytesOfRead = 0;

            // receive_shadows
            bool enable = false;
            bytesOfRead = stream.read(&enable, sizeof(enable));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(enable),
                "Read the receive shadows failed !");
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserMaterial::parseTransparencyCastsShadows(
        DataStream &stream, Material *material, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
            size_t bytesOfRead = 0;

            bool enable = false;
            bytesOfRead = stream.read(&enable, sizeof(enable));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(enable),
                "Read the transparency cast shadows failed !");
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserMaterial::parseSetTextureAlias(
        DataStream &stream, Material *material, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
            String aliasName;
            ret = parseString(stream, aliasName);

            if (T3D_FAILED(ret))
            {
                ret = T3D_ERR_RES_INVALID_CONTENT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read alias name failed !");
                break;
            }

            String textureName;
            ret = parseString(stream, textureName);

            if (T3D_FAILED(ret))
            {
                ret = T3D_ERR_RES_INVALID_CONTENT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read texture name failed !");
                break;
            }
        } while (0);

        return ret;
    }
}
