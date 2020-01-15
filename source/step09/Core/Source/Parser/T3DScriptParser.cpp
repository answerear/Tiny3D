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

#include "Parser/T3DScriptParser.h"
#include "Kernel/T3DCommon.h"
#include "T3DErrorDef.h"
#include "Parser/T3DScriptParserMaterial.h"
#include "Parser/T3DScriptParserTechnique.h"
#include "Parser/T3DScriptParserPass.h"
#include "Parser/T3DScriptParserTextureUnit.h"
#include "Parser/T3DScriptParserSampler.h"
#include "Parser/T3DScriptParserGPUProgram.h"


namespace Tiny3D
{
    #define T3D_TSC_VERSION_00000100            0x00000100
    #define T3D_TSC_CURRENT_VERSION             T3D_TSC_VERSION_00000100

    //--------------------------------------------------------------------------

    T3D_INIT_SINGLETON(ScriptParser);
    T3D_IMPLEMENT_CLASS_BASE_1(ScriptParserBase, ScriptParser);

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
        mMaterialParser = ScriptParserMaterial::create();
        mTechParser = ScriptParserTechnique::create();
        mPassParser = ScriptParserPass::create();
        mTexUnitParser = ScriptParserTextureUnit::create();
        mSamplerParser = ScriptParserSampler::create();
        mShaderParser = ScriptParserShader::create();
        mGPUParser = ScriptParserGPUProgram::create();
        mGPURefParser = ScriptParserGPUProgramRef::create();
        mCBufferParser = ScriptParserGPUContBuffer::create();
        mCBufferRefParser = ScriptParserGPUContBufferRef::create();
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
        case E_OP_TEXTURE_UNIT:
            parser = mTexUnitParser;
            break;
        case E_OP_SAMPLER:
            parser = mSamplerParser;
            break;
        case E_OP_TEX_ADDRESS_MODE:
        case E_OP_TEX_BORDER_COLOUR:
        case E_OP_FILTERING:
        case E_OP_CMPTEST:
        case E_OP_CMPFUNC:
        case E_OP_COMP_FUNC:
        case E_OP_MAX_ANISOTROPY:
        case E_OP_MIPMAP_BIAS:
            parser = mSamplerParser;
            break;
        case E_OP_FRAGMENT_PROGRAM:
        case E_OP_VERTEX_PROGRAM:
        case E_OP_GEOMETRY_PROGRAM:
        case E_OP_TESSELLATION_HULL_PROGRAM:
        case E_OP_TESSELLATION_DOMAIN_PROGRAM:
        case E_OP_COMPUTE_PROGRAM:
            parser = mShaderParser;
            break;
        case E_OP_VERTEX_PROGRAM_REF:
        case E_OP_FRAGMENT_PROGRAM_REF:
        case E_OP_GEOMETRY_PROGRAM_REF:
        case E_OP_TESSELLATION_HULL_PROGRAM_REF:
        case E_OP_TESSELLATION_DOMAIN_PROGRAM_REF:
        case E_OP_COMPUTE_PROGRAM_REF:
        case E_OP_SHADOW_CASTER_VERTEX_PROGRAM_REF:
        case E_OP_SHADOW_CASTER_FRAGMENT_PROGRAM_REF:
            T3D_ASSERT(0);  // 暫時不可能跑到這裡
            parser = nullptr;
            break;
        case E_OP_GPU_PROGRAM:
            parser = mGPUParser;
            break;
        case E_OP_GPU_PROGRAM_REF:
            parser = mGPURefParser;
            break;
        case E_OP_GPU_CBUFFER:
            parser = mCBufferParser;
            break;
        case E_OP_GPU_CBUFFER_REF:
            parser = mCBufferRefParser;
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
            T3DFileHeader header;
            stream.read(&header, sizeof(header));

            if (stricmp(header.magic, T3D_MAGIC) != 0)
            {
                // 非法的文件类型
                ret = T3D_ERR_RES_INVALID_FILETYPE;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid file type !");
                break;
            }

            if (header.subtype != FileSubType::E_FST_SCRIPT)
            {
                ret = T3D_ERR_RES_INVALID_FILETYPE;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid file subtype !");
                break;
            }

            while (!stream.eof() && ret == T3D_OK)
            {
                ret = parseObject(stream, object, header.version);
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ScriptParser::parse(DataStream &stream)
    {
        return parse(stream, nullptr);
    }

    //--------------------------------------------------------------------------

    TResult ScriptParser::parseObject(
        DataStream &stream, Object *object, uint32_t version)
    {
        TResult ret = T3D_OK;

        do 
        {
            size_t bytesOfRead = 0;

            // 类型
            uint16_t type = 0;
            bytesOfRead = stream.read(&type, sizeof(type));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(type), 
                "Read object type [%d] failed !", type);

            if (type != OBJECT)
            {
                // 错误类型，这里需要对象
                ret = T3D_ERR_RES_INVALID_OBJECT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid object ! Here need an object !");
                break;
            }

            // Op Code
            uint16_t opcode = 0;
            bytesOfRead = stream.read(&opcode, sizeof(opcode));
            T3D_CHECK_READ_CONTENT(bytesOfRead, sizeof(opcode),
                "Read op code failed !");

            ScriptParserBase *parser = getObjectParser(opcode);
            if (parser == nullptr)
            {
                ret = T3D_ERR_RES_INVALID_OBJECT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid op code ! Here need op code of object !");
                break;
            }

            ret = parser->parseObject(stream, object, version);
        } while (0);
        
        return ret;
    }
}
