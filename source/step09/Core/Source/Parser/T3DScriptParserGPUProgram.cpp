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


#include "Parser/T3DScriptParserGPUProgram.h"
#include "Parser/T3DScriptParser.h"
#include "Resource/T3DGPUProgram.h"
#include "Resource/T3DGPUProgramManager.h"
#include "Resource/T3DGPUConstBuffer.h"
#include "Resource/T3DGPUConstBufferManager.h"
#include "Resource/T3DMaterial.h"
#include "Kernel/T3DPass.h"
#include "T3DErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    T3D_IMPLEMENT_CLASS_BASECLASS_1(ScriptParserShader, ScriptParserBase);

    //--------------------------------------------------------------------------

    ScriptParserShaderPtr ScriptParserShader::create()
    {
        ScriptParserShaderPtr parser = new ScriptParserShader();
        parser->release();
        return parser;
    }

    //--------------------------------------------------------------------------

    ScriptParserShader::ScriptParserShader()
    {
    }

    //--------------------------------------------------------------------------

    ScriptParserShader::~ScriptParserShader()
    {
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserShader::parseObject(
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
                "Read the number of children of shader object failed !");

            // 名称
            String name;
            ret = parseString(stream, name);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read the name of shader failed !");
                break;
            }

            if (name.empty())
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid name of shader ! It must not be empty !");
                break;
            }

            // MD5
            uint8_t hash[16];
            bytesOfRead = stream.read(hash, sizeof(hash));
            T3D_CHECK_READ_CONTENT(bytesOfRead, 16,
                "Read hash of shader failed !");

            uint16_t type = UNKNOWN;
            uint16_t i = 0;

            for (i = 0; i < count; ++i)
            {
                // Type
                bytesOfRead = stream.read(&type, sizeof(type));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                    "Read the type of property of shader failed !");

                if (type == PROPERTY)
                {
                    ret = parseProperties(stream, program, version);
                }
                else if (type == OBJECT)
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

            if (ret != T3D_OK)
            {
                break;
            }

            Shader::ShaderType shaderType = Shader::ShaderType::VERTEX_SHADER;
            size_t len = mSource.find_last_of('.');
            String title =
                (len != String::npos ? mSource.substr(0, len) : mSource);
            String source = title + "." + mStage;

            if (mStage == "vs")
            {
                // vertex shader
                shaderType = Shader::ShaderType::VERTEX_SHADER;
            }
            else if (mStage == "ps")
            {
                // fragment shader
                shaderType = Shader::ShaderType::PIXEL_SHADER;

            }
            else if (mStage == "gs")
            {
                // geometry shader
            }
            else if (mStage == "ds")
            {

            }
            else if (mStage == "cs")
            {

            }
            else
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid property of GPU program !");
                break;
            }

            ShaderPtr shader;
            ret = program->addShader(source, shaderType, shader);
            if (ret != T3D_OK)
            {
                break;
            }

        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserShader::parseProperties(
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
                            "Read source of shader failed !");
                    }
                    mSource = source;
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
                    mTarget = target;
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
                    mEntry = entry;
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
                    mStage = stage;
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
        } while (0);

        return ret;
    }

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
        DataStream& stream, Object* object, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            Material* material = static_cast<Material*>(object);

            // 属性数量
            uint16_t count = 0;
            bytesOfRead = stream.read(&count, sizeof(count));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(count),
                "Read the number of children of gpu_program failed !");

            // 名称
            String name;
            ret = parseString(stream, name);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read the name of gpu_program failed !");
                break;
            }

            if (name.empty())
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid name of gpu_program ! It must not be empty !");
                break;
            }

            // MD5
            uint8_t hash[16];
            bytesOfRead = stream.read(hash, sizeof(hash));
            T3D_CHECK_READ_CONTENT(bytesOfRead, 16,
                "Read hash of GPU program failed !");

            GPUProgramPtr program;
            if (material != nullptr)
            {
                // belongs to material
                ret = material->addGPUProgram(name, program);
                if (ret != T3D_OK)
                {
                    break;
                }
            }
            else
            {
                // global
                program = T3D_GPU_PROGRAM_MGR.loadGPUProgram(name);
                if (program == nullptr)
                {
                    ret = T3D_ERR_RES_CREATE_GPUPROGRAM;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Create GPU Program [%s] failed !", name.c_str());
                    break;
                }
            }

            uint16_t type = UNKNOWN;
            uint16_t i = 0;

            for (i = 0; i < count; ++i)
            {
                // Type
                bytesOfRead = stream.read(&type, sizeof(type));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                    "Read the type of property of gpu_program failed !");

                if (type == PROPERTY)
                {
                    ret = parseProperties(stream, program, version);
                }
                else if (type == OBJECT)
                {
                    ret = parseObjects(stream, program, version);
                }
                else
                {
                    ret = T3D_ERR_RES_INVALID_OBJECT;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid object type in gpu_program script !");
                    break;
                }

                if (ret != T3D_OK)
                {
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

    TResult ScriptParserGPUProgram::parseProperties(
        DataStream& stream, GPUProgram* program, uint32_t version)
    {
        TResult ret = T3D_OK;

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

            Pass *pass = static_cast<Pass*>(object);

            // 属性数量
            uint16_t count = 0;
            bytesOfRead = stream.read(&count, sizeof(count));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(count),
                "Read the number of children of gpu_program_ref failed !");

            // 名称
            String name;
            ret = parseString(stream, name);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read the name of gpu_program_ref failed !");
                break;
            }

            if (name.empty())
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid name of gpu_program_ref ! It must not be empty !");
                break;
            }

            GPUProgramRefPtr program = GPUProgramRef::create(name);

            // MD5
            uint8_t hash[16];
            bytesOfRead = stream.read(hash, sizeof(hash));
            T3D_CHECK_READ_CONTENT(bytesOfRead, 16,
                "Read hash of GPU program reference failed !");

            uint16_t type = UNKNOWN;
            uint16_t i = 0;

            for (i = 0; i < count; ++i)
            {
                // Type
                bytesOfRead = stream.read(&type, sizeof(type));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                    "Read the type of property of gpu_program_ref failed !");

                if (type == PROPERTY)
                {
                    ret = parseProperties(stream, program, version);
                }
                else if (type == OBJECT)
                {
                    ret = parseObjects(stream, program, version);
                }
                else
                {
                    ret = T3D_ERR_RES_INVALID_OBJECT;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid object type in gpu_program_ref script !");
                    break;
                }

                if (ret != T3D_OK)
                {
                    break;
                }
            }

            pass->setGPUProgram(program);
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserGPUProgramRef::parseProperties(
        DataStream &stream, GPUProgramRef *param, uint32_t version)
    {
        TResult ret = T3D_OK;
        return ret;
    }

    //--------------------------------------------------------------------------

    ScriptParserGPUContBufferPtr ScriptParserGPUContBuffer::create()
    {
        ScriptParserGPUContBufferPtr parser = new ScriptParserGPUContBuffer();
        parser->release();
        return parser;
    }

    //--------------------------------------------------------------------------

    ScriptParserGPUContBuffer::ScriptParserGPUContBuffer()
    {

    }

    //--------------------------------------------------------------------------

    ScriptParserGPUContBuffer::~ScriptParserGPUContBuffer()
    {

    }

    //--------------------------------------------------------------------------

    TResult ScriptParserGPUContBuffer::parseObject(
        DataStream& stream, Object* object, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            Material *material = static_cast<Material*>(object);

            // 属性数量
            uint16_t count = 0;
            bytesOfRead = stream.read(&count, sizeof(count));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(count),
                "Read the number of children of gpu_cbuffer failed !");

            // 名称
            String name;
            ret = parseString(stream, name);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read the name of gpu_cbuffer failed !");
                break;
            }

            if (name.empty())
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid name of gpu_cbuffer ! It must not be empty !");
                break;
            }

            // MD5
            uint8_t hash[16];
            bytesOfRead = stream.read(hash, sizeof(hash));
            T3D_CHECK_READ_CONTENT(bytesOfRead, 16,
                "Read hash of GPU constant buffer failed !");

            GPUConstBufferPtr buffer;

            if (material != nullptr)
            {
                // belongs to material
                ret = material->addGPUConstBuffer(name, buffer);
                if (ret != T3D_OK)
                {
                    break;
                }
            }
            else
            {
                // global
                buffer = T3D_GPU_CONST_BUFFER_MGR.loadBuffer(name);
                if (buffer == nullptr)
                {
                    ret = T3D_ERR_RES_CREATE_GPUCBUFFER;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Create GPU constant buffer [%s] failed !", 
                        name.c_str());
                    break;
                }
            }

            uint16_t type = UNKNOWN;
            uint16_t i = 0;

            for (i = 0; i < count; ++i)
            {
                // Type
                bytesOfRead = stream.read(&type, sizeof(type));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                    "Read the type of property of gpu_cbuffer failed !");

                if (type == PROPERTY)
                {
                    ret = parseProperties(stream, buffer, version);
                }
                else if (type == OBJECT)
                {
                    ret = parseObjects(stream, buffer, version);
                }
                else
                {
                    ret = T3D_ERR_RES_INVALID_OBJECT;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid object type in gpu_cbuffer script !");
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

    TResult ScriptParserGPUContBuffer::parseProperties(
        DataStream& stream, GPUConstBuffer *buffer, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // ID
            uint16_t id;
            bytesOfRead = stream.read(&id, sizeof(id));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(id),
                "Read ID of property of gpu_cbuffer failed !");

            switch (id)
            {
            case E_OP_SHARED_PARAMS_REF:
                ret = parseSharedParamsRef(stream, buffer, version);
                break;
            case E_OP_PARAM_INDEXED:
                ret = parseParamIndexed(stream, buffer, version);
                break;
            case E_OP_PARAM_NAMED:
                ret = parseParamNamed(stream, buffer, version);
                break;
            case E_OP_PARAM_INDEXED_AUTO:
                ret = parseParamIndexedAuto(stream, buffer, version);
                break;
            case E_OP_PARAM_NAMED_AUTO:
                ret = parseParamNamedAuto(stream, buffer, version);
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid property of gpu_cbuffer !");
                }
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserGPUContBuffer::parseSharedParamsRef(
        DataStream &stream, GPUConstBuffer *buffer, uint32_t version)
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

    TResult ScriptParserGPUContBuffer::parseParamIndexed(
        DataStream &stream, GPUConstBuffer *buffer, uint32_t version)
    {
        TResult ret = T3D_OK;

        void *values = nullptr;

        do
        {
            size_t bytesOfRead = 0;

            // index
            uint32_t index = 0;
            bytesOfRead = stream.read(&index, sizeof(index));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(index),
                "Read index of param_indexed failed !");

            // data type
            uint8_t type = 0;
            bytesOfRead = stream.read(&type, sizeof(type));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                "Read type of param_indexed failed !");

            // the number of values
            uint8_t count = 0;
            bytesOfRead = stream.read(&count, sizeof(count));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(count),
                "Read number of values of param_indexed failed !");

            BuiltinType bt = (BuiltinType)type;
            
            size_t typeSize = 0;

            switch (bt)
            {
            case BuiltinType::REAL:
                values = new float32_t[count];
                typeSize = sizeof(float32_t);
                break;
            case BuiltinType::INT:
                values = new int32_t[count];
                typeSize = sizeof(int32_t);
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid property of param_indexed !");
                }
                break;
            }

            if (ret != T3D_OK)
            {
                break;
            }

            // values
            bytesOfRead = stream.read(values, count * typeSize);
            T3D_CHECK_READ_CONTENT(bytesOfRead, count * typeSize,
                "Read values of param_indexed failed !");

            // TODO
        } while (0);

        T3D_SAFE_DELETE_ARRAY(values);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserGPUContBuffer::parseParamNamed(
        DataStream &stream, GPUConstBuffer *buffer, uint32_t version)
    {
        TResult ret = T3D_OK;

        void *values = nullptr;

        do
        {
            size_t bytesOfRead = 0;

            // name
            String name;
            ret = parseString(stream, name);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read name of param_named failed !");
                break;
            }

            // data type
            uint8_t type = 0;
            bytesOfRead = stream.read(&type, sizeof(type));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                "Read type of param_named failed !");

            // the number of values
            uint8_t count = 0;
            bytesOfRead = stream.read(&count, sizeof(count));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(count),
                "Read the number of values of param_named failed !");

            BuiltinType bt = (BuiltinType)type;

            size_t typeSize = 0;

            switch (bt)
            {
            case BuiltinType::REAL:
                values = new float32_t[count];
                typeSize = sizeof(float32_t);
                break;
            case BuiltinType::INT:
                values = new int32_t[count];
                typeSize = sizeof(int32_t);
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid property of param_indexed !");
                }
                break;
            }

            if (ret != T3D_OK)
            {
                break;
            }

            // values
            bytesOfRead = stream.read(values, count * typeSize);
            T3D_CHECK_READ_CONTENT(bytesOfRead, count * typeSize,
                "Read values of param_indexed failed !");

            // TODO
        } while (0);

        T3D_SAFE_DELETE_ARRAY(values);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserGPUContBuffer::parseParamIndexedAuto(
        DataStream &stream, GPUConstBuffer *buffer, uint32_t version)
    {
        TResult ret = T3D_OK;
        void *extra = nullptr;

        do
        {
            size_t bytesOfRead = 0;

            // index
            uint32_t index = 0;
            bytesOfRead = stream.read(&index, sizeof(index));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(index),
                "Read index of param_indexed_auto failed !");

            // constant
            uint16_t constant = 0;
            bytesOfRead = stream.read(&constant, sizeof(constant));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(constant),
                "Read value_code of param_indexed_auto failed !");

            // element type
            uint8_t type = 0;
            bytesOfRead = stream.read(&type, sizeof(type));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                "Read elemente type of param_indexed_auto failed !");

            if ((BuiltinType)type != BuiltinType::REAL 
                && (BuiltinType)type != BuiltinType::INT)
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid data type of param_indexed_auto !");
                break;
            }

            // the number of elements
            uint8_t count = 0;
            bytesOfRead = stream.read(&count, sizeof(count));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(count),
                "Read the number of constant values of \
                param_indexed_auto failed !");

            // extra data type
            uint8_t extraType = 0;
            bytesOfRead = stream.read(&extraType, sizeof(extraType));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(extraType),
                "Read extra data type of param_indexed_auto failed !");

            // the number of extra data
            uint8_t extraCount = 0;
            bytesOfRead = stream.read(&extraCount, sizeof(extraCount));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(extraCount),
                "Read the number of extra data of param_indexed_auto failed !");

            if (extraCount > 0)
            {
                size_t typeSize = 0;

                switch ((BuiltinType)extraType)
                {
                case BuiltinType::REAL:
                    extra = new float32_t[extraCount];
                    typeSize = sizeof(float32_t);
                    break;
                case BuiltinType::INT:
                    extra = new int32_t[extraCount];
                    typeSize = sizeof(int32_t);
                    break;
                default:
                    {
                        ret = T3D_ERR_RES_INVALID_PROPERTY;
                        T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                            "Invalid property of param_indexed !");
                    }
                    break;
                }

                if (ret != T3D_OK)
                {
                    break;
                }

                // extra data
                bytesOfRead = stream.read(extra, extraCount * typeSize);
                T3D_CHECK_READ_CONTENT(bytesOfRead, extraCount * typeSize,
                    "Read extra data of param_indexed failed !");
            }

            // TODO
            ret = buffer->addDataDeclaration(
                (GPUConstBuffer::BuiltinType)type, count);
        } while (0);

        T3D_SAFE_DELETE_ARRAY(extra);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserGPUContBuffer::parseParamNamedAuto(
        DataStream &stream, GPUConstBuffer *buffer, uint32_t version)
    {
        TResult ret = T3D_OK;

        void *extra = nullptr;

        do
        {
            size_t bytesOfRead = 0;

            // name
            String name;
            ret = parseString(stream, name);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read name of param_named_auto failed !");
                break;
            }

            // constant
            uint16_t constant = 0;
            bytesOfRead = stream.read(&constant, sizeof(constant));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(constant),
                "Read value_code of param_indexed_auto failed !");

            // element type
            uint8_t type = 0;
            bytesOfRead = stream.read(&type, sizeof(type));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                "Read elemente type of param_indexed_auto failed !");

            // the number of elements
            uint8_t count = 0;
            bytesOfRead = stream.read(&count, sizeof(count));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(count),
                "Read the number of constant values of \
                param_indexed_auto failed !");

            // extra data type
            uint8_t extraType = 0;
            bytesOfRead = stream.read(&extraType, sizeof(extraType));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(extraType),
                "Read extra data type of param_indexed_auto failed !");

            // the number of extra data
            uint8_t extraCount = 0;
            bytesOfRead = stream.read(&extraCount, sizeof(extraCount));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(extraCount),
                "Read the number of extra data of param_indexed_auto failed !");

            size_t typeSize = 0;

            switch ((BuiltinType)extraType)
            {
            case BuiltinType::REAL:
                extra = new float32_t[extraCount];
                typeSize = sizeof(float32_t);
                break;
            case BuiltinType::INT:
                extra = new int32_t[extraCount];
                typeSize = sizeof(int32_t);
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid property of param_indexed !");
                }
                break;
            }

            if (ret != T3D_OK)
            {
                break;
            }

            // extra data
            bytesOfRead = stream.read(extra, extraCount * typeSize);
            T3D_CHECK_READ_CONTENT(bytesOfRead, extraCount * typeSize,
                "Read extra data of param_indexed failed !");

            // TODO
        } while (0);

        T3D_SAFE_DELETE_ARRAY(extra);

        return ret;
    }

    //--------------------------------------------------------------------------

    ScriptParserGPUContBufferRefPtr ScriptParserGPUContBufferRef::create()
    {
        ScriptParserGPUContBufferRefPtr parser = new ScriptParserGPUContBufferRef();
        parser->release();
        return parser;
    }

    //--------------------------------------------------------------------------

    ScriptParserGPUContBufferRef::ScriptParserGPUContBufferRef()
        : mSlot(0)
    {

    }

    //--------------------------------------------------------------------------

    ScriptParserGPUContBufferRef::~ScriptParserGPUContBufferRef()
    {

    }

    //--------------------------------------------------------------------------

    TResult ScriptParserGPUContBufferRef::parseObject(
        DataStream &stream, Object *object, uint32_t version)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesOfRead = 0;

            // 属性数量
            uint16_t count = 0;
            bytesOfRead = stream.read(&count, sizeof(count));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(count),
                "Read the number of children of gpu_cbuffer_ref failed !");

            // 名称
            String name;
            ret = parseString(stream, name);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read the name of gpu_cbuffer_ref failed !");
                break;
            }

            if (name.empty())
            {
                ret = T3D_ERR_RES_INVALID_PROPERTY;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid name of gpu_cbuffer_ref ! It must not be empty !");
                break;
            }

            GPUProgramRef *programRef = static_cast<GPUProgramRef*>(object);

            // MD5
            uint8_t hash[16];
            bytesOfRead = stream.read(hash, sizeof(hash));
            T3D_CHECK_READ_CONTENT(bytesOfRead, 16,
                "Read hash of GPU constant buffer reference failed !");

            uint16_t type = UNKNOWN;
            uint16_t i = 0;

            for (i = 0; i < count; ++i)
            {
                // Type
                bytesOfRead = stream.read(&type, sizeof(type));
                T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type),
                    "Read the type of gpu_cbuffer_ref failed !");

                if (type == PROPERTY)
                {
                    ret = parseProperties(stream, programRef, version);
                }
                else if (type == OBJECT)
                {
                    ret = parseObjects(stream, programRef, version);
                }
                else
                {
                    ret = T3D_ERR_RES_INVALID_OBJECT;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid object type in gpu_cbuffer_ref script !");
                    break;
                }

                if (ret != T3D_OK)
                {
                    break;
                }
            }

            GPUConstBufferRefPtr bufferRef;
            ret = programRef->addConstBufferRef(name, mSlot, bufferRef);
            if (ret != T3D_OK)
            {
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParserGPUContBufferRef::parseProperties(
        DataStream &stream, GPUProgramRef *program, uint32_t version)
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
            case E_OP_CBUFFER_SLOT:
                {
                    uint32_t slot = 0;
                    bytesOfRead = stream.read(&slot, sizeof(slot));
                    if (ret != T3D_OK)
                    {
                        T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                            "Read slot of gpu_cbuffer_ref failed !");
                    }
                    mSlot = slot;
                }
                break;
            default:
                {
                    ret = T3D_ERR_RES_INVALID_PROPERTY;
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                        "Invalid property of gpu_cbuffer_ref !");
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
