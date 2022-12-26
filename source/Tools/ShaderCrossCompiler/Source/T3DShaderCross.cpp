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


#include "T3DShaderCross.h"
#include "SLParser.h"
#include "T3DShaderCompiler.h"
#include "FileScriptObject.pb.h"
#include <google/protobuf/util/json_util.h>


namespace Tiny3D
{
    using namespace shaderlab;

    #define SCC_VERSION_00000100            0x00000100
    #define SCC_CURRENT_VERSION             SCC_VERSION_00000100

    //--------------------------------------------------------------------------

    ShaderCrossPtr ShaderCross::create()
    {
        ShaderCrossPtr ptr = new ShaderCross();
        // ptr->release();
        return ptr;
    }

    //--------------------------------------------------------------------------

    ShaderCross::ShaderCross()
    {

    }

    //--------------------------------------------------------------------------

    ShaderCross::~ShaderCross()
    {

    }

    //--------------------------------------------------------------------------

    bool ShaderCross::compile(int32_t argc, const char *argv[])
    {
        bool ret = false;

        do 
        {
            Args args;

            // parse arguments
            ret = parse(argc, argv, args);
            if (!ret)
            {
                break;
            }

            // 编译生成对应平台的shader以及 render state 文件
            ret = compile(mInputFile, mOutputDir, args);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ShaderCross::compile(const String& input, const Args& args)
    {
        return compile(input, "", args);
    }

    //--------------------------------------------------------------------------

    bool ShaderCross::compile(const String& input, const String& output, const Args& args)
    {
        bool ret = false;

        do 
        {
            if (input.empty())
            {
                // 没有输入文件
                SCC_LOG_ERROR("Empty input file ! Use -h for help.");
                break;
            }

            // 读取源文件内容
            String content;
            ret = readSourceFile(content, input);
            if (!ret)
            {
                break;
            }

            // 解析 Shader Lab 语法，解析出 shader 来
            auto source = ParseShaderLab(content.c_str(), content.size());

            // 记录参数
            mArgs = args;

            // 生成输出文件目录和文件名
            String dir, title, ext;
            Dir::parsePath(input, dir, title, ext);

            if (output.empty())
            {
                // 输出文件夹为空
                mOutputDir = dir;
            }

            if (mArgs.baseName.empty())
            {
                mArgs.baseName = title;
            }

            // 处理源数据
            ret = compile(source);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ShaderCross::readSourceFile(String& content, const String& input)
    {
        bool ret = false;

        FileDataStream fs;
        uint8_t* data = nullptr;

        do
        {
            if (!Dir::exists(input))
            {
                SCC_LOG_ERROR("%s does not exist !", input.c_str());
                ret = false;
                break;
            }

            // 打开文件
            if (!fs.open(input.c_str(), FileDataStream::E_MODE_READ_ONLY))
            {
                SCC_LOG_ERROR("Open %s failed !", input.c_str());
                ret = false;
                break;
            }

            // 读取文件
            size_t size = (size_t)fs.size();
            data = new uint8_t[size];
            size_t bytesOfRead = fs.read(data, size);
            if (size != bytesOfRead)
            {
                SCC_LOG_ERROR("Read %s data failed !", input.c_str());
                ret = false;
                break;
            }

            fs.close();

            content.assign((const char*)data, size);

            delete []data;
            data = nullptr;

            ret = true;
        } while (0);


        if (fs.isOpened())
        {
            fs.close();
        }

        if (data != nullptr)
        {
            delete []data;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ShaderCross::compile(shaderlab::SLShader* source)
    {
        bool ret = true;

        do 
        {
            MemoryDataStream stream(10 * 1024 * 1024);

            uint8_t* buffer = nullptr;
            size_t bufSize = 0;
            stream.getBuffer(buffer, bufSize);

            uint8_t* data = nullptr;
            size_t dataLen = 0;

            Script::FileFormat::FileShader file;

            Script::ShaderSystem::Shader* shader = nullptr;

            // 写文件头
            T3DFileHeader header;

            bool isBinary = mArgs.hasOptions(Args::OPT_BINARY_FILE);

            if (isBinary)
            {
                // 二进制文件
                memcpy(header.magic, T3D_FILE_MAGIC, 3);
                header.magic[3] = 0;
                header.subtype = T3D_FILE_SUBTYPE_SCC;
                header.version = SCC_CURRENT_VERSION;
                header.filesize = sizeof(header);

                data = buffer + sizeof(header);
                dataLen = bufSize - sizeof(header);

                shader = new Script::ShaderSystem::Shader();
            }
            else
            {
                // 文本文件
                Script::FileFormat::FileHeader* fileHeader = file.mutable_header();
                fileHeader->set_magic(T3D_FILE_MAGIC);
                fileHeader->set_type(Script::FileFormat::FileHeader::FileType::FileHeader_FileType_Shader);
                fileHeader->set_version(SCC_CURRENT_VERSION);

                shader = file.mutable_shader();
            }

            // 解析成对应的 pb 格式
            ret = translate(*source, shader);
            if (!ret)
            {
                break;
            }

            // 编译 shader 到对应平台
            ShaderCompilerPtr compiler = ShaderCompiler::create();
            ret = compiler->compile(shader, mInputFile, mOutputDir, mArgs);
            if (!ret)
            {
                break;
            }

            if (isBinary)
            {
                // 二进制文件
                ret = shader->SerializeToArray(data, dataLen);
                if (!ret)
                {
                    SCC_LOG_ERROR("Serialize to array failed !");
                    break;
                }

                uint32_t len = (uint32_t)shader->ByteSizeLong();
                header.filesize += len;
                data += len;
                dataLen -= len;
            }

            String output = mOutputDir + Dir::getNativeSeparator() + mArgs.baseName + ".t3d";

            FileDataStream fs;
            if (!fs.open(output.c_str(), FileDataStream::E_MODE_WRITE_ONLY))
            {
                SCC_LOG_ERROR("Open file %s failed !", output.c_str());
                ret = false;
                break;
            }

            if (isBinary)
            {
                // 写二进制文件
                stream.seek(0, false);
                stream.write(&header, sizeof(header));

                fs.write(buffer, header.filesize);
            }
            else
            {
                // 转成 json 格式
                String str;
                google::protobuf::util::JsonOptions opts;
                opts.add_whitespace = true;
                opts.always_print_enums_as_ints = false;
                opts.always_print_primitive_fields = true;
                google::protobuf::util::Status status = google::protobuf::util::MessageToJsonString(file, &str, opts);

                // 写 json 文件
                fs.write((void*)str.c_str(), str.length());
            }

            fs.close();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ShaderCross::translate(const shaderlab::SLShader& src, Script::ShaderSystem::Shader* dst)
    {
        bool ret = true;

        do 
        {
            // shader name
            dst->set_name(src.shaderName);

            // properties
            for (auto itr = src.properties.props.begin(); itr != src.properties.props.end(); ++itr)
            {
                auto dstProp = dst->add_properties();
                auto srcProp = *itr;
                ret = ret && translate(srcProp, dstProp);
            }

            // Sub Shader
            for (auto itr = src.subShaders.begin(); itr != src.subShaders.end(); ++itr)
            {
                auto dstSubShader = dst->add_subshaders();
                auto srcSubShader = *itr;
                ret = ret && translate(*srcSubShader, dstSubShader);
            }

            if (!ret)
            {
                break;
            }

            // Fallback
            if (src.fallbackName.empty())
            {
                auto fallback = dst->mutable_fallback();
                fallback->mutable_enable()->set_value(false);
            }
            else
            {
                auto fallback = dst->mutable_fallback();
                fallback->mutable_name()->set_value(src.fallbackName);
            }

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ShaderCross::translate(const SLPropValue& src, Script::ShaderSystem::Property* dst)
    {
        // property name
        dst->set_name(src.name);

        // property display name
        dst->set_display_name(src.description);

        // property value
        auto value = dst->mutable_values();
        switch (src.type)
        {
        case SLPropValue::kColor:
            {
                auto val = value->mutable_color();
                val->set_r(src.value[0]);
                val->set_g(src.value[1]);
                val->set_b(src.value[2]);
                val->set_a(src.value[3]);
            }
            break;
        case SLPropValue::kVector:
            {
                auto val = value->mutable_vector();
                val->set_x(src.value[0]);
                val->set_y(src.value[1]);
                val->set_z(src.value[2]);
                val->set_w(src.value[3]);
            }
            break;
        case SLPropValue::kFloat:
            {
                auto val = value->mutable_fval();
                val->set_value(src.value[0]);
            }
            break;
        case SLPropValue::kRange:
            {
                auto val = value->mutable_rval();
                val->set_value(src.value[0]);
                val->set_lower(src.value[1]);
                val->set_upper(src.value[2]);
            }
            break;
        case SLPropValue::kTexture:
            {
                switch (src.texture.dimension)
                {
                case TextureDimension::kTexDim2D:
                    {
                        auto val = value->mutable_tex2d();
                        val->set_value(src.texture.name);
                    }
                    break;
                case TextureDimension::kTexDim2DArray:
                    {
                        auto val = value->mutable_tex2d_array();
                        val->set_value(src.texture.name);
                    }
                    break;
                case TextureDimension::kTexDim3D:
                    {
                        auto val = value->mutable_tex3d();
                        val->set_value(src.texture.name);
                    }
                    break;
                case TextureDimension::kTexDimCUBE:
                    {
                        auto val = value->mutable_cubemap();
                        val->set_value(src.texture.name);
                    }
                    break;
                case TextureDimension::kTexDimCUBEArray:
                    {
                        auto val = value->mutable_cubemap_array();
                        val->set_value(src.texture.name);
                    }
                    break;
                }
            }
            break;
        }

        return true;
    }

    //--------------------------------------------------------------------------

    bool ShaderCross::translate(const shaderlab::SLSubShader& src, Script::ShaderSystem::SubShader* dst)
    {
        bool ret = true;

        // LOD
        dst->set_lod(src.lod);

        // tags
        for (auto itr = src.tags.begin(); itr != src.tags.end(); ++itr)
        {
            auto key = itr->first;
            auto value = itr->second;
            dst->mutable_tags()->insert({ key, value });
        }

        // render states
        
        // passes
        for (auto itr = src.passes.begin(); itr != src.passes.end(); ++itr)
        {
            auto srcPass = *itr;
            auto dstPass = dst->add_passes();
            ret = ret && translate(*srcPass, dstPass);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ShaderCross::translate(const shaderlab::SLPassBase& src, Script::ShaderSystem::Pass* dst)
    {
        bool ret = true;

        switch (src.type)
        {
        case SLPassBase::kPassNormal:
            {
                const SLNormalPass& normalPass = dynamic_cast<const SLNormalPass&>(src);
                ret = translate(normalPass, dst);
            }
            break;
        case SLPassBase::kPassUse:
            {
                const SLUsePass& usePass = dynamic_cast<const SLUsePass&>(src);
                ret = translate(usePass, dst);
            }
            break;
        case SLPassBase::kPassCompiled:
            break;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ShaderCross::translate(const shaderlab::SLUsePass& src, Script::ShaderSystem::Pass* dst)
    {
        auto pass = dst->mutable_use_pass();
        pass->set_value(src.useName);

        return true;
    }

    //--------------------------------------------------------------------------

    bool ShaderCross::translate(const shaderlab::SLNormalPass& src, Script::ShaderSystem::Pass* dst)
    {
        bool ret = true;

        // name
        dst->set_name(src.state.name);

        // tags
        for (auto itr = src.state.tags.begin(); itr != src.state.tags.end(); ++itr)
        {
            auto key = itr->first;
            auto value = itr->second;
            dst->mutable_tags()->insert({ key, value });
        }

        // render states
        ret = ret && translate(src.state, dst->mutable_state());

        // program
        ret = ret && translate(src.program, dst->mutable_program());

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ShaderCross::translate(const shaderlab::SLShaderState& src, Script::ShaderSystem::State* dst)
    {
        bool ret = true;

        // alpha to mask
        dst->set_alpha2mask((bool)src.alphaToMask.val);

        // blend
        auto blend = dst->mutable_blend();
        auto blend_state = blend->mutable_state();
        blend_state->set_src_rgb(getBlendFactor(src.srcBlend));
        blend_state->set_dst_rgb(getBlendFactor(src.destBlend));
        blend_state->set_src_alpha(getBlendFactor(src.srcBlendAlpha));
        blend_state->set_dst_alpha(getBlendFactor(src.destBlendAlpha));

        // blend op
        dst->set_blend_op(getBlendOp(src.blendOp));

        // color mask
        auto colorMask = dst->mutable_color_mask();
        auto color_mask_state = colorMask->mutable_state1();
        color_mask_state->set_channels(src.colMask.val);

        // conservative
        dst->set_conservative(false);
        
        // culling
        dst->set_cull(getCulling(src.culling));

        // depth bias
        auto depth_bias = dst->mutable_depth_bias();
        depth_bias->set_factor(src.offsetFactor.val);
        depth_bias->set_units(src.offsetUnits.val);
        
        // stencil
        auto stencil = dst->mutable_stencil();
        stencil->set_ref(src.stencilRef.val);
        stencil->set_read_mask(src.stencilReadMask.val);
        stencil->set_write_mask(src.stencilWriteMask.val);
        auto stencil_op = stencil->mutable_op();
        ret = ret && translate(src.stencilOp, stencil_op);
        auto stencil_op_back = stencil->mutable_op_back();
        ret = ret && translate(src.stencilOpBack, stencil_op_back);
        auto stencil_op_front = stencil->mutable_op_front();
        ret = ret && translate(src.stencilOpFront, stencil_op_front);

        // z clip
        dst->set_z_clip(true);
        
        // z test
        dst->set_z_test(getCompare(src.zTest));

        // z write
        dst->set_z_write((bool)src.zWrite.val);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ShaderCross::translate(const shaderlab::SLStencilOperation& src, Script::ShaderSystem::StencilOp* dst)
    {
        dst->set_comp(getCompare(src.comp));
        dst->set_pass(getStencilOp(src.pass));
        dst->set_fail(getStencilOp(src.fail));
        dst->set_z_fail(getStencilOp(src.zFail));
        return true;
    }

    //--------------------------------------------------------------------------

    Script::ShaderSystem::BlendFactor ShaderCross::getBlendFactor(shaderlab::SLFloat src) const
    {
        Script::ShaderSystem::BlendFactor factor = Script::ShaderSystem::BlendFactor::ZERO;

        shaderlab::BlendMode m = (shaderlab::BlendMode)src.val;

        switch (m)
        {
        case shaderlab::BlendMode::kBlendZero:
            factor = Script::ShaderSystem::BlendFactor::ZERO;
            break;
        case shaderlab::BlendMode::kBlendOne:
            factor = Script::ShaderSystem::BlendFactor::ONE;
            break;
        case shaderlab::BlendMode::kBlendDstColor:
            factor = Script::ShaderSystem::BlendFactor::DST_COLOR;
            break;
        case shaderlab::BlendMode::kBlendSrcColor:
            factor = Script::ShaderSystem::BlendFactor::SRC_COLOR;
            break;
        case shaderlab::BlendMode::kBlendOneMinusDstColor:
            factor = Script::ShaderSystem::BlendFactor::ONE_MINUS_DST_COLOR;
            break;
        case shaderlab::BlendMode::kBlendSrcAlpha:
            factor = Script::ShaderSystem::BlendFactor::SRC_ALPHA;
            break;
        case shaderlab::BlendMode::kBlendOneMinusSrcColor:
            factor = Script::ShaderSystem::BlendFactor::ONE_MINUS_SRC_COLOR;
            break;
        case shaderlab::BlendMode::kBlendDstAlpha:
            factor = Script::ShaderSystem::BlendFactor::DST_ALPHA;
            break;
        case shaderlab::BlendMode::kBlendOneMinusDstAlpha:
            factor = Script::ShaderSystem::BlendFactor::ONE_MINUS_DST_ALPHA;
            break;
        case shaderlab::BlendMode::kBlendSrcAlphaSaturate:
            break;
        case shaderlab::BlendMode::kBlendOneMinusSrcAlpha:
            factor = Script::ShaderSystem::BlendFactor::ONE_MINUS_SRC_ALPHA;
            break;
        }

        return factor;
    }

    //--------------------------------------------------------------------------

    Script::ShaderSystem::BlendOp ShaderCross::getBlendOp(shaderlab::SLFloat src) const
    {
        Script::ShaderSystem::BlendOp op = Script::ShaderSystem::BlendOp::ADD;

        shaderlab::BlendOp o = (shaderlab::BlendOp)src.val;

        switch (o)
        {
        case shaderlab::BlendOp::kBlendOpAdd:
            op = Script::ShaderSystem::BlendOp::ADD;
            break;
        case shaderlab::BlendOp::kBlendOpSub:
            op = Script::ShaderSystem::BlendOp::SUB;
            break;
        case shaderlab::BlendOp::kBlendOpRevSub:
            op = Script::ShaderSystem::BlendOp::REV_SUB;
            break;
        case shaderlab::BlendOp::kBlendOpMin:
            op = Script::ShaderSystem::BlendOp::MIN;
            break;
        case shaderlab::BlendOp::kBlendOpMax:
            op = Script::ShaderSystem::BlendOp::MAX;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalClear:
            op = Script::ShaderSystem::BlendOp::CLEAR;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalSet:
            op = Script::ShaderSystem::BlendOp::SET;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalCopy:
            op = Script::ShaderSystem::BlendOp::COPY;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalCopyInverted:
            op = Script::ShaderSystem::BlendOp::COPY_INVERTED;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalNoop:
            op = Script::ShaderSystem::BlendOp::NOOP;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalInvert:
            op = Script::ShaderSystem::BlendOp::INVERT;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalAnd:
            op = Script::ShaderSystem::BlendOp::AND;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalNand:
            op = Script::ShaderSystem::BlendOp::NAND;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalOr:
            op = Script::ShaderSystem::BlendOp::OR;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalNor:
            op = Script::ShaderSystem::BlendOp::NOR;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalXor:
            op = Script::ShaderSystem::BlendOp::XOR;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalEquiv:
            op = Script::ShaderSystem::BlendOp::EQUIV;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalAndReverse:
            op = Script::ShaderSystem::BlendOp::AND_REVERSE;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalAndInverted:
            op = Script::ShaderSystem::BlendOp::AND_INVERTED;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalOrReverse:
            op = Script::ShaderSystem::BlendOp::OR_REVERSE;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalOrInverted:
            op = Script::ShaderSystem::BlendOp::OR_INVERTED;
            break;
        }

        return op;
    }

    //--------------------------------------------------------------------------

    Script::ShaderSystem::Cull ShaderCross::getCulling(shaderlab::SLFloat src) const
    {
        Script::ShaderSystem::Cull culling = Script::ShaderSystem::Cull::BACK;

        shaderlab::CullMode mode = (shaderlab::CullMode)src.val;

        switch (mode)
        {
        case shaderlab::CullMode::kCullOff:
            culling = Script::ShaderSystem::Cull::OFF;
            break;
        case shaderlab::CullMode::kCullFront:
            culling = Script::ShaderSystem::Cull::FRONT;
            break;
        case shaderlab::CullMode::kCullBack:
            culling = Script::ShaderSystem::Cull::BACK;
            break;
        case shaderlab::CullMode::kCullFrontAndBack:
            break;
        }

        return culling;
    }

    //--------------------------------------------------------------------------

    Script::ShaderSystem::Comp ShaderCross::getCompare(shaderlab::SLFloat src) const
    {
        Script::ShaderSystem::Comp comp = Script::ShaderSystem::Comp::ALWAYS;

        shaderlab::CompareFunction c = (shaderlab::CompareFunction)src.val;

        switch (c)
        {
        case shaderlab::CompareFunction::kFuncDisabled:
            break;
        case shaderlab::CompareFunction::kFuncNever:
            comp = Script::ShaderSystem::Comp::NEVER;
            break;
        case shaderlab::CompareFunction::kFuncLess:
            comp = Script::ShaderSystem::Comp::LESS;
            break;
        case shaderlab::CompareFunction::kFuncEqual:
            comp = Script::ShaderSystem::Comp::EQUAL;
            break;
        case shaderlab::CompareFunction::kFuncLEqual:
            comp = Script::ShaderSystem::Comp::L_EQUAL;
            break;
        case shaderlab::CompareFunction::kFuncGreater:
            comp = Script::ShaderSystem::Comp::GREATER;
            break;
        case shaderlab::CompareFunction::kFuncNotEqual:
            comp = Script::ShaderSystem::Comp::NOT_EQUALT;
            break;
        case shaderlab::CompareFunction::kFuncGEqual:
            comp = Script::ShaderSystem::Comp::G_EQUAL;
            break;
        case shaderlab::CompareFunction::kFuncAlways:
            comp = Script::ShaderSystem::Comp::ALWAYS;
            break;
        }

        return comp;
    }

    //--------------------------------------------------------------------------

    Script::ShaderSystem::StencilOp_Op ShaderCross::getStencilOp(shaderlab::SLFloat src) const
    {
        Script::ShaderSystem::StencilOp_Op op = Script::ShaderSystem::StencilOp_Op::StencilOp_Op_KEEP;

        shaderlab::StencilOp o = (shaderlab::StencilOp)src.val;

        switch (o)
        {
        case shaderlab::StencilOp::kStencilOpKeep:
            op = Script::ShaderSystem::StencilOp_Op_KEEP;
            break;
        case shaderlab::StencilOp::kStencilOpZero:
            op = Script::ShaderSystem::StencilOp_Op_ZERO;
            break;
        case shaderlab::StencilOp::kStencilOpReplace:
            op = Script::ShaderSystem::StencilOp_Op_REPLACE;
            break;
        case shaderlab::StencilOp::kStencilOpIncrSat:
            op = Script::ShaderSystem::StencilOp_Op_INCR_SAT;
            break;
        case shaderlab::StencilOp::kStencilOpDecrSat:
            op = Script::ShaderSystem::StencilOp_Op_DECR_SAT;
            break;
        case shaderlab::StencilOp::kStencilOpInvert:
            op = Script::ShaderSystem::StencilOp_Op_INVERT;
            break;
        case shaderlab::StencilOp::kStencilOpIncrWrap:
            op = Script::ShaderSystem::StencilOp_Op_INCR_WRAP;
            break;
        case shaderlab::StencilOp::kStencilOpDecrWrap:
            op = Script::ShaderSystem::StencilOp_Op_DECR_WRAP;
            break;
        }

        return op;
    }

    //--------------------------------------------------------------------------

    Script::ShaderSystem::ProgramType ShaderCross::getProgramType(shaderlab::ProgramType src) const
    {
        Script::ShaderSystem::ProgramType type = Script::ShaderSystem::ProgramType::HLSL;

        switch (src)
        {
        case shaderlab::ProgramType::kCG:
            type = Script::ShaderSystem::ProgramType::CG;
            break;
        case shaderlab::ProgramType::kHLSL:
            type = Script::ShaderSystem::ProgramType::HLSL;
            break;
        case shaderlab::ProgramType::kGLSL:
            type = Script::ShaderSystem::ProgramType::GLSL;
            break;
        }

        return type;
    }

    //--------------------------------------------------------------------------

    bool ShaderCross::translate(const shaderlab::SLProgram& src, Script::ShaderSystem::Program* dst)
    {
        dst->mutable_source()->set_type(getProgramType(src.type));
        dst->mutable_source()->set_code(src.source);

        return true;
    }

    //--------------------------------------------------------------------------

    void ShaderCross::printUsage()
    {
        printf("Usage : ");
        printf("  scc input_file -t target [options]");
        printf("    input_file : The source file.");
        printf("    -t target_language : Target shading language (glsl, hlsl, essl, dxil, spirv, msl_macos, msl_ios)");
        printf("    Options : ");
        printf("      -v : Print version.");
        printf("      -h : Print help.");
        printf("      -o output_dir : Set the output dir.");
        printf("      -n base_name : Output file base name.");
        printf("      -On : n is the optimization level from 0 to 3.");
        printf("      -d : Translate high level shader language to target shader language with debug information.");
        printf("      -b : Output file in binary file format. Default is json file format.");
        printf("      -i : Including file path.");
    }

    //--------------------------------------------------------------------------

    void ShaderCross::printVersion()
    {
        printf("Shader Cross Compiler Version : %s", CURRENT_VERSION_STR);
    }

    //--------------------------------------------------------------------------

    bool ShaderCross::parse(int32_t argc, const char* argv[], Args& args)
    {
        bool ret = true;

        if (argc == 0)
        {
            // 没参数，只能直接打印帮助，让用户知道如何使用
            args.options = Args::Options::OPT_PRINT_HELP;
            ret = false;
        }

        int i = 0;

        while (ret && i < argc)
        {
            if (strcmp(argv[i], "-h") == 0)
            {
                // 打印帮助信息
                args.options = Args::OPT_PRINT_HELP;
            }
            else if (strcmp(argv[i], "-v") == 0)
            {
                // 打印版本号
                args.options = Args::OPT_PRINT_VERSION;
            }
            else if (strcmp(argv[i], "-d") == 0)
            {
                // 设置输出调试信息
                args.options |= Args::OPT_ENABLE_DEBUG_INFO;
            }
            else if (strcmp(argv[i], "-t") == 0)
            {
                if (argc - 1 == i)
                {
                    // 参数不够
                    SCC_LOG_ERROR("Missing target platform ! Use -h for help.");
                    ret = false;
                    break;
                }

                ++i;
                args.target = argv[i];
            }
            else if (strncmp(argv[i], "-O", 2) == 0)
            {
                // shader 优化级别
                args.optimizeLevel = atoi(argv[i] + 2);
            }
            else if (strcmp(argv[i], "-b") == 0)
            {
                // 是否输出二进制文件
                args.options |= Args::OPT_BINARY_FILE;
            }
            else if (strcmp(argv[i], "-i") == 0)
            {
                // 包含头文件路径
                if (argc - 1 == i)
                {
                    // 参数不够
                    SCC_LOG_ERROR("Missing include path ! Use -h for help.");
                    ret = false;
                    break;
                }

                ++i;
                args.include = argv[i];
            }
            else if (strcmp(argv[i], "-n") == 0)
            {
                // 输出文件的基础名称，所有输出文济名都是这个名称开头
                if (argc - 1 == i)
                {
                    // 参数不够
                    SCC_LOG_ERROR("Missing base name ! Use -h for help.");
                    ret = false;
                    break;
                }

                ++i;
                args.baseName = argv[i];
            }
            else if (strcmp(argv[i], "-o") == 0)
            {
                // 输出路径
                if (argc - 1 == i)
                {
                    // 参数不够
                    SCC_LOG_ERROR("Missing output directory ! Use -h for help.");
                    ret = false;
                    break;
                }

                ++i;
                mOutputDir = argv[i];
            }
            else if (mInputFile.empty())
            {
                // 输入文件列表
                mInputFile = argv[i];
            }

            ++i;
        }

        return ret;
    }
}
