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


#include "Kernel/T3DScriptParserTechnique.h"
#include "Kernel/T3DScriptParser.h"
#include "Resource/T3DMaterial.h"
#include "Kernel/T3DTechnique.h"
#include "T3DErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ScriptParserTechniquePtr ScriptParserTechnique::create()
    {
        ScriptParserTechniquePtr parser = new ScriptParserTechnique();
        parser->release();
        return parser;
    }

    //--------------------------------------------------------------------------

    ScriptParserTechnique::ScriptParserTechnique()
    {

    }

    //--------------------------------------------------------------------------

    ScriptParserTechnique::~ScriptParserTechnique()
    {

    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTechnique::parseObject(
        DataStream &stream, Object *object, uint32_t version)
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

            TechniquePtr tech = Technique::create();

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
                        "Read the type of property of technique failed !");
                    break;
                }

                if (type == E_NT_PROPERTY)
                {
                    ret = parseProperties(stream, tech, version);
                }
                else if (type == E_NT_OBJECT)
                {
                    ret = parseObjects(stream, tech, version);
                }
                else
                {
                    ret = T3D_ERR_RES_INVALID_OBJECT;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid object type in technique script !");
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

    TResult ScriptParserTechnique::parseObjects(
        DataStream &stream, Technique *tech, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
            size_t bytesOfRead = 0;

            // Op code
            uint16_t opcode = 0;
            bytesOfRead = stream.read(&opcode, sizeof(opcode));
            if (bytesOfRead != sizeof(opcode))
            {
                ret = T3D_ERR_RES_INVALID_CONTENT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read the opcode of property of technique failed !");
                break;
            }

            ScriptParserBase *parser
                = ScriptParser::getInstance().getObjectParser(opcode);
            if (parser == nullptr)
            {
                ret = T3D_ERR_RES_INVALID_OBJECT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid object in technique !");
                break;
            }

            ret = parser->parseObject(stream, tech, version);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTechnique::parseProperties(
        DataStream &stream, Technique *tech, uint32_t version)
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
                    "Read ID of property of technique failed !");
                break;
            }

            switch (id)
            {
            case E_OP_SCHEME:
                ret = parseScheme(stream, tech, version);
                break;
            case E_OP_LOD_INDEX:
                ret = parseLODIndex(stream, tech, version);
                break;
            case E_OP_SHADOW_CASTER_MATERIAL:
                ret = parseShadowCasterMaterial(stream, tech, version);
                break;
            case E_OP_SHADOW_RECEIVER_MATERIAL:
                ret = parseShadowReceiverMaterial(stream, tech, version);
                break;
            case E_OP_GPU_VENDOR_RULE:
                ret = parseGPUVendorRule(stream, tech, version);
                break;
            case E_OP_GPU_DEVICE_RULE:
                ret = parseGPUDeviceRule(stream, tech, version);
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid property of technique !");
                }
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTechnique::parseScheme(
        DataStream &stream, Technique *tech, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
            String scheme;
            ret = parseString(stream, scheme);

            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, 
                    "Invalid scheme in technique !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTechnique::parseLODIndex(
        DataStream &stream, Technique *tech, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
            size_t bytesOfRead = 0;
            uint32_t index = 0;
            bytesOfRead = stream.read(&index, sizeof(index));
            if (bytesOfRead != sizeof(index))
            {
                ret = T3D_ERR_RES_INVALID_CONTENT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid LOD index of property of technique !");
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTechnique::parseShadowCasterMaterial(
        DataStream &stream, Technique *tech, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
            String matName;
            ret = parseString(stream, matName);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid ")
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTechnique::parseShadowReceiverMaterial(
        DataStream &stream, Technique *tech, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTechnique::parseGPUVendorRule(
        DataStream &stream, Technique *tech, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserTechnique::parseGPUDeviceRule(
        DataStream &stream, Technique *tech, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
        } while (0);

        return ret;
    }
}
