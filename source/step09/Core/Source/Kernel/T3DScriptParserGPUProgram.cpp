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


#include "Kernel/T3DScriptParserGPUProgram.h"
#include "Kernel/T3DScriptParser.h"
#include "Resource/T3DGPUProgram.h"
#include "T3DErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ScriptParserGPUProgramPtr ScriptParserGPUProgram::create()
    {
        ScriptParserGPUProgramPtr parser = new ScriptParserGPUProgram();
        parser->release();
        return parser;
    }

    //--------------------------------------------------------------------------

    ScriptParserGPUProgram::ScriptParserGPUProgram()
    {
    }

    //--------------------------------------------------------------------------

    ScriptParserGPUProgram::~ScriptParserGPUProgram()
    {
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserGPUProgram::parseObject(
        DataStream &stream, Object *object, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
            size_t bytesOfRead = 0;

            GPUProgram *program = static_cast<GPUProgram*>(object);

            // 属性数量
            uint16_t count = 0;
            bytesOfRead = stream.read(&count, sizeof(count));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(count),
                "Read the number of children of pass failed !");

            // 名称
            String name;
            ret = parseString(stream, name);

            uint16_t type = E_NT_UNKNOWN;
            uint16_t i = 0;

            for (i = 0; i < count; ++i)
            {
                // Type
                bytesOfRead = stream.read(&type, sizeof(type));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                    "Read the type of property of GPU program failed !");

                if (type == E_NT_PROPERTY)
                {
                    ret = parseProperties(stream, program, version);
                }
                else if (type == E_NT_OBJECT)
                {
                    ret = parseObjects(stream, program, version);
                }
                else
                {
                    ret = T3D_ERR_RES_INVALID_OBJECT;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid object type in GPU program script !");
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

    TResult ScriptParserGPUProgram::parseProperties(
        DataStream &stream, GPUProgram *program, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // ID
            uint16_t id;
            bytesOfRead = stream.read(&id, sizeof(id));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(id),
                "Read ID of property of GPU program failed !");

            String source, target, entry, stage;

            switch (id)
            {
            case E_OP_SOURCE:
                {
                    ret = parseString(stream, source);
                    if (ret != T3D_OK)
                    {
                        T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                            "Read source of GPU program failed !");
                    }
                }
                break;
            case E_OP_TARGET:
                {
                    ret = parseString(stream, target);
                    if (ret != T3D_OK)
                    {
                        T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                            "Read target of GPU program failed !");
                    }
                }
                break;
            case E_OP_ENTRY_POINT:
                {
                    ret = parseString(stream, entry);
                    if (ret != T3D_OK)
                    {
                        T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                            "Read entry_point of GPU program failed !");
                    }
                }
                break;
            case E_OP_STAGE:
                {
                    ret = parseString(stream, stage);
                    if (ret != T3D_OK)
                    {
                        T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                            "Read stage of GPU program failed !");
                    }
                }
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid property of GPU program !");
                }
                break;
            }

            if (ret != T3D_OK)
            {
                break;
            }

            // TODO
        } while (0);

        return ret;
    }


    //--------------------------------------------------------------------------

    ScriptParserGPUProgramRefPtr ScriptParserGPUProgramRef::create()
    {
        ScriptParserGPUProgramRefPtr parser = new ScriptParserGPUProgramRef();
        parser->release();
        return parser;
    }

    //--------------------------------------------------------------------------

    ScriptParserGPUProgramRef::ScriptParserGPUProgramRef()
    {

    }

    //--------------------------------------------------------------------------

    ScriptParserGPUProgramRef::~ScriptParserGPUProgramRef()
    {

    }

    //--------------------------------------------------------------------------

    TResult ScriptParserGPUProgramRef::parseObject(
        DataStream &stream, Object *object, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            ShaderParam *param = static_cast<ShaderParam*>(object);

            // 属性数量
            uint16_t count = 0;
            bytesOfRead = stream.read(&count, sizeof(count));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(count),
                "Read the number of children of GPU program ref failed !");

            // 名称
            String name;
            ret = parseString(stream, name);

            uint16_t type = E_NT_UNKNOWN;
            uint16_t i = 0;

            for (i = 0; i < count; ++i)
            {
                // Type
                bytesOfRead = stream.read(&type, sizeof(type));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                    "Read the type of property of GPU program ref failed !");

                if (type == E_NT_PROPERTY)
                {
                    ret = parseProperties(stream, param, version);
                }
                else if (type == E_NT_OBJECT)
                {
                    ret = parseObjects(stream, param, version);
                }
                else
                {
                    ret = T3D_ERR_RES_INVALID_OBJECT;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid object type in GPU program ref script !");
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

    TResult ScriptParserGPUProgramRef::parseProperties(
        DataStream &stream, ShaderParam *param, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // ID
            uint16_t id;
            bytesOfRead = stream.read(&id, sizeof(id));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(id),
                "Read ID of property of GPU program failed !");

            switch (id)
            {
            case E_OP_SHARED_PARAMS_REF:
                ret = parseSharedParamsRef(stream, param, version);
                break;
            case E_OP_PARAM_INDEXED:
                ret = parseParamIndexed(stream, param, version);
                break;
            case E_OP_PARAM_NAMED:
                ret = parseParamNamed(stream, param, version);
                break;
            case E_OP_PARAM_INDEXED_AUTO:
                ret = parseParamIndexedAuto(stream, param, version);
                break;
            case E_OP_PARAM_NAMED_AUTO:
                ret = parseParamNamedAuto(stream, param, version);
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid property of GPU program ref !");
                }
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserGPUProgramRef::parseSharedParamsRef(
        DataStream &stream, ShaderParam *param, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            uint16_t type = 0;
            bytesOfRead = stream.read(&type, sizeof(type));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                "Read shared_params_ref failed !");

            // TODO
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserGPUProgramRef::parseParamIndexed(
        DataStream &stream, ShaderParam *param, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // index


            // type

            // values
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserGPUProgramRef::parseParamNamed(
        DataStream &stream, ShaderParam *param, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {

        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserGPUProgramRef::parseParamIndexedAuto(
        DataStream &stream, ShaderParam *param, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserGPUProgramRef::parseParamNamedAuto(
        DataStream &stream, ShaderParam *param, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
        } while (0);

        return ret;
    }
}
