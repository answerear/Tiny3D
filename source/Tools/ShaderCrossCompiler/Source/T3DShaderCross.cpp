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
            String shaderName = mArgs.baseName + ".t3d";
            ShaderPtr shader = T3D_SHADER_MGR.createShader(shaderName);

            ret = translate(*source, shader);
            if (!ret)
            {
                break;
            }

            ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(mOutputDir, "FileSystem", Archive::AccessMode::kReadTruncate);
            T3D_SHADER_MGR.saveShader(shader, archive);

            // ArchivePtr arch = T3D_ARCHIVE_MGR.loadArchive(mOutputDir, "FileSystem", Archive::AccessMode::kReadOnly);
            // ShaderPtr shader1 = T3D_SHADER_MGR.loadShader(arch, shaderName);
            //
            // T3D_SHADER_MGR.saveShader(shader1, archive);
            // int a = 0;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ShaderCross::translate(const shaderlab::SLShader& src, ShaderPtr dst)
    {
        bool ret = true;

        do 
        {
            // properties
            for (auto itr = src.properties.props.begin(); itr != src.properties.props.end(); ++itr)
            {
                auto srcProp = *itr;
                ret = ret && translate(srcProp, dst);
            }

            // Sub Shader
            for (auto itr = src.subShaders.begin(); itr != src.subShaders.end(); ++itr)
            {
                auto srcSubShader = *itr;
                ret = ret && translate(*srcSubShader, dst);
            }

            if (!ret)
            {
                break;
            }

            // Fallback
            // if (src.fallbackName.empty())
            // {
            //     auto fallback = dst->mutable_fallback();
            //     fallback->mutable_enable()->set_value(false);
            // }
            // else
            // {
            //     auto fallback = dst->mutable_fallback();
            //     fallback->mutable_name()->set_value(src.fallbackName);
            // }

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ShaderCross::translate(const SLPropValue& src, ShaderPtr dst)
    {        
        // property name

        // property display name

        // property value
        switch (src.type)
        {
        case SLPropValue::kColor:
            {
                Buffer buffer;
                ShaderConstantParam::DATA_TYPE dataType = ShaderConstantParam::DATA_TYPE::DT_COLOR;
                buffer.setData(src.value, sizeof(ColorRGBA));
                ShaderConstantParamPtr param = ShaderConstantParam::create(src.name, buffer.Data, buffer.DataSize, dataType);
                dst->addConstantParam(param);
            }
            break;
        case SLPropValue::kVector:
            {
                Buffer buffer;
                buffer.setData(src.value, sizeof(Vector4));
                ShaderConstantParam::DATA_TYPE dataType = ShaderConstantParam::DATA_TYPE::DT_VECTOR4;
                ShaderConstantParamPtr param = ShaderConstantParam::create(src.name, buffer.Data, buffer.DataSize, dataType);
                dst->addConstantParam(param);
            }
            break;
        case SLPropValue::kFloat:
            {
                Buffer buffer;
                buffer.setData(&src.value[0], sizeof(float32_t));
                ShaderConstantParam::DATA_TYPE dataType = ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                ShaderConstantParamPtr param = ShaderConstantParam::create(src.name, buffer.Data, buffer.DataSize, dataType);
                dst->addConstantParam(param);
            }
            break;
        case SLPropValue::kRange:
            {
                // val->set_value(src.value[0]);
                // val->set_lower(src.value[1]);
                // val->set_upper(src.value[2]);
                Buffer buffer;
                buffer.setData(&src.value[0], sizeof(float32_t));
                ShaderConstantParam::DATA_TYPE dataType = ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                ShaderConstantParamPtr param = ShaderConstantParam::create(src.name, buffer.Data, buffer.DataSize, dataType);
                dst->addConstantParam(param);
            }
            break;
        case SLPropValue::kTexture:
            {
                TEXTURE_TYPE texType = TEXTURE_TYPE::TT_2D;
                
                switch (src.texture.dimension)
                {
                case TextureDimension::kTexDim2D:
                    {
                        // auto val = value->mutable_tex2d();
                        // val->set_value(src.texture.name);
                        texType = TEXTURE_TYPE::TT_2D;
                    }
                    break;
                case TextureDimension::kTexDim2DArray:
                    {
                        // auto val = value->mutable_tex2d_array();
                        // val->set_value(src.texture.name);
                        texType = TEXTURE_TYPE::TT_2D_ARRAY;
                    }
                    break;
                case TextureDimension::kTexDim3D:
                    {
                        // auto val = value->mutable_tex3d();
                        // val->set_value(src.texture.name);
                        texType = TEXTURE_TYPE::TT_3D;
                    }
                    break;
                case TextureDimension::kTexDimCUBE:
                    {
                        // auto val = value->mutable_cubemap();
                        // val->set_value(src.texture.name);
                        texType = TEXTURE_TYPE::TT_CUBE;
                    }
                    break;
                case TextureDimension::kTexDimCUBEArray:
                    {
                        // auto val = value->mutable_cubemap_array();
                        // val->set_value(src.texture.name);
                        texType = TEXTURE_TYPE::TT_CUBE_ARRAY;
                    }
                    break;
                }

                ShaderSamplerParamPtr param = ShaderSamplerParam::create(src.texture.name, texType);
                dst->addSamplerParam(param);
            }
            break;
        }

        return true;
    }

    //--------------------------------------------------------------------------

    bool ShaderCross::translate(const shaderlab::SLSubShader& src, ShaderPtr dst)
    {
        bool ret = true;

        std::stringstream ss;
        ss << dst->getTechniques().size();
        TechniquePtr tech = Technique::create(ss.str());
        dst->addTechnique(tech);

        // LOD
        // dst->set_lod(src.lod);
        tech->setLOD(src.lod);

        // tags
        for (auto val : src.tags)
        {
            const auto &key = val.first;
            const auto &value = val.second;
            // dst->mutable_tags()->insert({ key, value });
            tech->addTag(key, value);
        }

        // render states

        // passes
        for (auto val : src.passes)
        {
            // auto dstPass = dst->add_passes();
            ret = ret && translate(*val, tech);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ShaderCross::translate(const shaderlab::SLPassBase& src, TechniquePtr tech)
    {
        bool ret = true;

        switch (src.type)
        {
        case SLPassBase::kPassNormal:
            {
                const SLNormalPass& normalPass = dynamic_cast<const SLNormalPass&>(src);
                ret = translate(normalPass, tech);
            }
            break;
        case SLPassBase::kPassUse:
            {
                // const SLUsePass& usePass = dynamic_cast<const SLUsePass&>(src);
                // ret = translate(usePass, tech);
            }
            break;
        case SLPassBase::kPassCompiled:
            break;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    // bool ShaderCross::translate(const shaderlab::SLUsePass& src, Script::ShaderSystem::Pass* dst)
    // {
    //     auto pass = dst->mutable_use_pass();
    //     pass->set_value(src.useName);
    //
    //     return true;
    // }

    //--------------------------------------------------------------------------

    bool ShaderCross::translate(const shaderlab::SLNormalPass& src, TechniquePtr tech)
    {
        bool ret = true;

        // name
        // dst->set_name(src.state.name);
        PassPtr pass = Pass::create(src.state.name);

        // tags
        for (auto itr = src.state.tags.begin(); itr != src.state.tags.end(); ++itr)
        {
            auto key = itr->first;
            auto value = itr->second;
            // dst->mutable_tags()->insert({ key, value });
            pass->addTag(key, value);
        }

        // render states
        RenderStatePtr state = RenderState::create();
        ret = ret && translate(src.state, state);

        // program
        ret = ret && translate(src.program, pass);

        if (ret)
        {
            tech->addPass(pass);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ShaderCross::translate(const shaderlab::SLShaderState& src, RenderStatePtr state)
    {
        bool ret = true;

        BlendDesc blend;
        // alpha to mask
        // dst->set_alpha2mask((bool)src.alphaToMask.val);
        blend.AlphaToCoverageEnable = src.alphaToMask.val;

        // blend
        // auto blend = dst->mutable_blend();
        // auto blend_state = blend->mutable_state();
        // blend_state->set_src_rgb(getBlendFactor(src.srcBlend));
        // blend_state->set_dst_rgb(getBlendFactor(src.destBlend));
        // blend_state->set_src_alpha(getBlendFactor(src.srcBlendAlpha));
        // blend_state->set_dst_alpha(getBlendFactor(src.destBlendAlpha));
        blend.RenderTargetStates[0].SrcBlend = getBlendFactor(src.srcBlend);
        blend.RenderTargetStates[0].DestBlend = getBlendFactor(src.destBlend);
        blend.RenderTargetStates[0].SrcBlendAlpha = getBlendFactor(src.srcBlendAlpha);
        blend.RenderTargetStates[0].DstBlendAlpha = getBlendFactor(src.destBlendAlpha);

        // blend op
        // dst->set_blend_op(getBlendOp(src.blendOp));
        blend.RenderTargetStates[0].BlendOp = getBlendOp(src.blendOp);

        // color mask
        // auto colorMask = dst->mutable_color_mask();
        // auto color_mask_state = colorMask->mutable_state1();
        // color_mask_state->set_channels(src.colMask.val);
        blend.RenderTargetStates[0].ColorMask = src.colMask.val;

        state->setBlendDesc(blend);

        RasterizerDesc rasterizer;
        // conservative
        // dst->set_conservative(false);
        rasterizer.Conservative = false;
        
        // culling
        // dst->set_cull(getCulling(src.culling));
        rasterizer.CullMode = getCulling(src.culling);

        // depth bias
        // auto depth_bias = dst->mutable_depth_bias();
        // depth_bias->set_factor(src.offsetFactor.val);
        // depth_bias->set_units(src.offsetUnits.val);
        rasterizer.DepthBias = 0;                  // src.offsetUnits.val;
        rasterizer.SlopeScaledDepthBias = 0; // src.offsetFactor.val;
        
        state->setRasterizerDesc(rasterizer);

        DepthStencilDesc depthStencil;

        // stencil
        // auto stencil = dst->mutable_stencil();
        // stencil->set_ref(src.stencilRef.val);
        // stencil->set_read_mask(src.stencilReadMask.val);
        // stencil->set_write_mask(src.stencilWriteMask.val);
        // auto stencil_op = stencil->mutable_op();
        // ret = ret && translate(src.stencilOp, stencil_op);
        // auto stencil_op_back = stencil->mutable_op_back();
        // ret = ret && translate(src.stencilOpBack, stencil_op_back);
        // auto stencil_op_front = stencil->mutable_op_front();
        // ret = ret && translate(src.stencilOpFront, stencil_op_front);
        depthStencil.StencilRef = src.stencilRef.val;
        depthStencil.StencilReadMask = src.stencilReadMask.val;
        depthStencil.StencilWriteMask = src.stencilWriteMask.val;
        translate(src.stencilOpFront, depthStencil.FrontFace);

        // z clip
        // dst->set_z_clip(true);
        shaderlab::CompareFunction c = (shaderlab::CompareFunction)src.zTest.val;
        if (c != shaderlab::CompareFunction::kFuncDisabled)
        {
            depthStencil.DepthTestEnable = true;
        }
        
        // z test
        // dst->set_z_test(getCompare(src.zTest));
        depthStencil.DepthFunc = getCompare(src.zTest);

        // z write
        // dst->set_z_write((bool)src.zWrite.val);
        depthStencil.DepthWriteEnable = src.zWrite.val;

        state->setDepthStencilDesc(depthStencil);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ShaderCross::translate(const shaderlab::SLStencilOperation& src, DepthStencilDesc::StencilOpDesc &dst)
    {
        // dst->set_comp(getCompare(src.comp));
        // dst->set_pass(getStencilOp(src.pass));
        // dst->set_fail(getStencilOp(src.fail));
        // dst->set_z_fail(getStencilOp(src.zFail));
        dst.StencilFunc = getCompare(src.comp);
        dst.StencilPassOp = getStencilOp(src.pass);
        dst.StencilFailOp = getStencilOp(src.fail);
        dst.StencilDepthFailOp = getStencilOp(src.zFail);
        return true;
    }

    //--------------------------------------------------------------------------

    BlendFactor ShaderCross::getBlendFactor(shaderlab::SLFloat src) const
    {
        BlendFactor factor = BlendFactor::kZero;

        shaderlab::BlendMode m = (shaderlab::BlendMode)src.val;

        switch (m)
        {
        case shaderlab::BlendMode::kBlendZero:
            factor = BlendFactor::kZero;
            break;
        case shaderlab::BlendMode::kBlendOne:
            factor = BlendFactor::kOne;
            break;
        case shaderlab::BlendMode::kBlendDstColor:
            factor = BlendFactor::kDstColor;
            break;
        case shaderlab::BlendMode::kBlendSrcColor:
            factor = BlendFactor::kSrcColor;
            break;
        case shaderlab::BlendMode::kBlendOneMinusDstColor:
            factor = BlendFactor::kOneMinusDstColor;
            break;
        case shaderlab::BlendMode::kBlendSrcAlpha:
            factor = BlendFactor::kSrcAlpha;
            break;
        case shaderlab::BlendMode::kBlendOneMinusSrcColor:
            factor = BlendFactor::kOneMinusSrcColor;
            break;
        case shaderlab::BlendMode::kBlendDstAlpha:
            factor = BlendFactor::kDstAlpha;
            break;
        case shaderlab::BlendMode::kBlendOneMinusDstAlpha:
            factor = BlendFactor::kOneMinusDstAlpha;
            break;
        case shaderlab::BlendMode::kBlendSrcAlphaSaturate:
            break;
        case shaderlab::BlendMode::kBlendOneMinusSrcAlpha:
            factor = BlendFactor::kOneMinusSrcAlpha;
            break;
        default:
            break;
        }

        return factor;
    }

    //--------------------------------------------------------------------------

    BlendOperation ShaderCross::getBlendOp(shaderlab::SLFloat src) const
    {
        BlendOperation op = BlendOperation::kAdd;

        shaderlab::BlendOp o = (shaderlab::BlendOp)src.val;

        switch (o)
        {
        case shaderlab::BlendOp::kBlendOpAdd:
            op = BlendOperation::kAdd;
            break;
        case shaderlab::BlendOp::kBlendOpSub:
            op = BlendOperation::kSubtract;
            break;
        case shaderlab::BlendOp::kBlendOpRevSub:
            op = BlendOperation::kReverseSubtract;
            break;
        case shaderlab::BlendOp::kBlendOpMin:
            op = BlendOperation::kMin;
            break;
        case shaderlab::BlendOp::kBlendOpMax:
            op = BlendOperation::kMax;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalClear:
            // op = BlendOperation::kClear;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalSet:
            // op = BlendOperation::SET;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalCopy:
            // op = BlendOperation::COPY;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalCopyInverted:
            // op = BlendOperation::COPY_INVERTED;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalNoop:
            // op = BlendOperation::NOOP;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalInvert:
            // op = BlendOperation::INVERT;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalAnd:
            // op = BlendOperation::AND;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalNand:
            // op = BlendOperation::NAND;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalOr:
            // op = BlendOperation::OR;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalNor:
            // op = BlendOperation::NOR;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalXor:
            // op = BlendOperation::XOR;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalEquiv:
            // op = BlendOperation::EQUIV;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalAndReverse:
            // op = BlendOperation::AND_REVERSE;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalAndInverted:
            // op = BlendOperation::AND_INVERTED;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalOrReverse:
            // op = BlendOperation::OR_REVERSE;
            break;
        case shaderlab::BlendOp::kBlendOpLogicalOrInverted:
            // op = BlendOperation::OR_INVERTED;
            break;
        default:
            break;
        }

        return op;
    }

    //--------------------------------------------------------------------------

    CullingMode ShaderCross::getCulling(shaderlab::SLFloat src) const
    {
        CullingMode culling = CullingMode::kAnticlockwise;

        shaderlab::CullMode mode = (shaderlab::CullMode)src.val;

        switch (mode)
        {
        case shaderlab::CullMode::kCullOff:
            culling = CullingMode::kNone;
            break;
        case shaderlab::CullMode::kCullFront:
            culling = CullingMode::kClockwise;
            break;
        case shaderlab::CullMode::kCullBack:
            culling = CullingMode::kAnticlockwise;
            break;
        case shaderlab::CullMode::kCullFrontAndBack:
            break;
        }

        return culling;
    }

    //--------------------------------------------------------------------------

    CompareFunction ShaderCross::getCompare(shaderlab::SLFloat src) const
    {
        CompareFunction comp = CompareFunction::kAlwaysPass;

        shaderlab::CompareFunction c = (shaderlab::CompareFunction)src.val;

        switch (c)
        {
        case shaderlab::CompareFunction::kFuncDisabled:
            break;
        case shaderlab::CompareFunction::kFuncNever:
            comp = CompareFunction::kAlwaysFail;
            break;
        case shaderlab::CompareFunction::kFuncLess:
            comp = CompareFunction::kLess;
            break;
        case shaderlab::CompareFunction::kFuncEqual:
            comp = CompareFunction::kEqual;
            break;
        case shaderlab::CompareFunction::kFuncLEqual:
            comp = CompareFunction::kLessEqual;
            break;
        case shaderlab::CompareFunction::kFuncGreater:
            comp = CompareFunction::kGreater;
            break;
        case shaderlab::CompareFunction::kFuncNotEqual:
            comp = CompareFunction::kNotEqual;
            break;
        case shaderlab::CompareFunction::kFuncGEqual:
            comp = CompareFunction::kGreaterEqual;
            break;
        case shaderlab::CompareFunction::kFuncAlways:
            comp = CompareFunction::kAlwaysPass;
            break;
        default:
            break;
        }

        return comp;
    }

    //--------------------------------------------------------------------------

    StencilOp ShaderCross::getStencilOp(shaderlab::SLFloat src) const
    {
        StencilOp op = StencilOp::kKeep;

        shaderlab::StencilOp o = (shaderlab::StencilOp)src.val;

        switch (o)
        {
        case shaderlab::StencilOp::kStencilOpKeep:
            op = StencilOp::kKeep;
            break;
        case shaderlab::StencilOp::kStencilOpZero:
            op = StencilOp::kZero;
            break;
        case shaderlab::StencilOp::kStencilOpReplace:
            op = StencilOp::kReplace;
            break;
        case shaderlab::StencilOp::kStencilOpIncrSat:
            op = StencilOp::kInc;
            break;
        case shaderlab::StencilOp::kStencilOpDecrSat:
            op = StencilOp::kDec;
            break;
        case shaderlab::StencilOp::kStencilOpInvert:
            op = StencilOp::kInvert;
            break;
        case shaderlab::StencilOp::kStencilOpIncrWrap:
            op = StencilOp::kIncWrap;
            break;
        case shaderlab::StencilOp::kStencilOpDecrWrap:
            op = StencilOp::kDecWrap;
            break;
        }

        return op;
    }

    //--------------------------------------------------------------------------

    // Script::ShaderSystem::ProgramType ShaderCross::getProgramType(shaderlab::ProgramType src) const
    // {
    //     Script::ShaderSystem::ProgramType type = Script::ShaderSystem::ProgramType::HLSL;
    //
    //     switch (src)
    //     {
    //     case shaderlab::ProgramType::kCG:
    //         type = Script::ShaderSystem::ProgramType::CG;
    //         break;
    //     case shaderlab::ProgramType::kHLSL:
    //         type = Script::ShaderSystem::ProgramType::HLSL;
    //         break;
    //     case shaderlab::ProgramType::kGLSL:
    //         type = Script::ShaderSystem::ProgramType::GLSL;
    //         break;
    //     }
    //
    //     return type;
    // }

    //--------------------------------------------------------------------------

    bool ShaderCross::translate(const shaderlab::SLProgram& src, PassPtr pass)
    {
        // dst->mutable_source()->set_type(getProgramType(src.type));
        // dst->mutable_source()->set_code(src.source);

        ShaderCompilerPtr compiler = ShaderCompiler::create();
        return compiler->compile(src.source, pass, mInputFile, mOutputDir, mArgs);
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
