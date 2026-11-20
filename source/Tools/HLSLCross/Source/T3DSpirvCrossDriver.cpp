/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#include "T3DSpirvCrossDriver.h"

#include <spirv_glsl.hpp>
#include <spirv_hlsl.hpp>
#include <spirv_msl.hpp>

#include <cstring>
#include <memory>


namespace Tiny3D
{
    namespace
    {
        bool parseVersion(const String &str, bool &hasVersion, uint32_t &version, String &outMessage)
        {
            hasVersion = !str.empty();
            version = 0;
            if (!hasVersion)
            {
                return true;
            }

            for (char c : str)
            {
                if (c < '0' || c > '9')
                {
                    outMessage = "Invalid target version '" + str + "'.";
                    return false;
                }
            }
            version = (uint32_t)std::stoul(str);
            return true;
        }

        spv::ExecutionModel executionModel(HLSLStage stage)
        {
            switch (stage)
            {
            case HLSLStage::kHull:      return spv::ExecutionModelTessellationControl;
            case HLSLStage::kDomain:    return spv::ExecutionModelTessellationEvaluation;
            case HLSLStage::kGeometry:  return spv::ExecutionModelGeometry;
            case HLSLStage::kPixel:     return spv::ExecutionModelFragment;
            case HLSLStage::kCompute:   return spv::ExecutionModelGLCompute;
            case HLSLStage::kVertex:
            default:                    return spv::ExecutionModelVertex;
            }
        }

        // GLSL 的 SSBO 不区分 StructuredBuffer / ByteAddressBuffer / Append / Consume，DXC 只把种类写在块类型名上，
        // 而同一种类的缓冲共用一个块类型，SPIRV-Cross 只有第一个块用类型名声明，其余退化成变量名，种类随之丢失。
        // 这里把种类与原名写进变量名，并清掉块类型名，让每个块都以变量名声明：
        //   T3D_Buffer_<种类>l<原名长度><原名>
        // 种类为 sb / rwsb / bab / rwbab / asb / csb，cnt 表示 <原名> 的隐藏计数器。
        // 引擎运行期按这个格式解析，改了要同步改 GL4 后端
        void renameStorageBuffers(spirv_cross::Compiler &compiler)
        {
            struct BufferKind
            {
                const char *typeName;
                bool        isPrefix;
                const char *tag;
            };

            static const BufferKind kKinds[] =
            {
                { "type_StructuredBuffer_",         true,   "sb"    },
                { "type_RWStructuredBuffer_",       true,   "rwsb"  },
                { "type_ByteAddressBuffer",         false,  "bab"   },
                { "type_RWByteAddressBuffer",       false,  "rwbab" },
                { "type_AppendStructuredBuffer_",   true,   "asb"   },
                { "type_ConsumeStructuredBuffer_",  true,   "csb"   },
                { "type_ACSBuffer_counter",         false,  "cnt"   },
            };

            static const String kCounterPrefix = "counter_var_";

            TSet<uint32_t> blockTypes;
            const spirv_cross::ShaderResources resources = compiler.get_shader_resources();

            for (const auto &res : resources.storage_buffers)
            {
                const String typeName = compiler.get_name(res.base_type_id);

                const BufferKind *kind = nullptr;
                for (const BufferKind &k : kKinds)
                {
                    if (k.isPrefix ? (typeName.compare(0, ::strlen(k.typeName), k.typeName) == 0) : (typeName == k.typeName))
                    {
                        kind = &k;
                        break;
                    }
                }

                if (kind == nullptr)
                {
                    continue;
                }

                String name = compiler.get_name(res.id);
                if (::strcmp(kind->tag, "cnt") == 0)
                {
                    if (name.compare(0, kCounterPrefix.size(), kCounterPrefix) != 0)
                    {
                        continue;
                    }
                    name = name.substr(kCounterPrefix.size());
                }

                if (name.empty())
                {
                    continue;
                }

                compiler.set_name(res.id, "T3D_Buffer_" + String(kind->tag) + "l" + std::to_string(name.size()) + name);
                blockTypes.insert(compiler.get_type(res.base_type_id).self);
            }

            for (uint32_t id : blockTypes)
            {
                compiler.set_name(id, "");
            }
        }
    }

    //--------------------------------------------------------------------------

    bool SpirvCrossDriver::translate(const TArray<uint8_t> &spirv, HLSLStage stage,
        const String &entryPoint, const HLSLCrossTarget &target,
        String &outSource, String &outMessage)
    {
        if (spirv.empty() || spirv.size() % sizeof(uint32_t) != 0)
        {
            outMessage = "SPIR-V blob size is not a multiple of 4.";
            return false;
        }

        if ((uint32_t)stage >= kHLSLStageCount)
        {
            outMessage = "Invalid shader stage " + std::to_string((uint32_t)stage) + ".";
            return false;
        }

        bool hasVersion = false;
        uint32_t intVersion = 0;
        if (!parseVersion(target.version, hasVersion, intVersion, outMessage))
        {
            return false;
        }

        TArray<uint32_t> words(spirv.size() / sizeof(uint32_t));
        ::memcpy(words.data(), spirv.data(), spirv.size());

        try
        {
            std::unique_ptr<spirv_cross::CompilerGLSL> compiler;
            bool combinedImageSamplers = false;
            bool buildDummySampler = false;

            switch (target.language)
            {
            case HLSLTarget::kHlsl:
                // https://github.com/KhronosGroup/SPIRV-Cross/issues/121
                if (stage == HLSLStage::kGeometry || stage == HLSLStage::kHull || stage == HLSLStage::kDomain)
                {
                    outMessage = "GS, HS, and DS has not been supported yet.";
                    return false;
                }
                if (stage == HLSLStage::kCompute && intVersion < 50)
                {
                    outMessage = "CS in HLSL shader model earlier than 5.0 is not supported.";
                    return false;
                }
                compiler.reset(new spirv_cross::CompilerHLSL(std::move(words)));
                break;

            case HLSLTarget::kGlsl:
            case HLSLTarget::kEssl:
                // 低版本方言要 flatten_buffer_block 才能产出合法代码，这里不实现，直接挡掉
                if (hasVersion && target.language == HLSLTarget::kGlsl && intVersion < 140)
                {
                    outMessage = "GLSL version " + target.version + " is not supported, requires 140 or higher.";
                    return false;
                }
                if (hasVersion && target.language == HLSLTarget::kEssl && intVersion < 300)
                {
                    outMessage = "ESSL version " + target.version + " is not supported, requires 300 or higher.";
                    return false;
                }
                compiler.reset(new spirv_cross::CompilerGLSL(std::move(words)));
                combinedImageSamplers = true;
                buildDummySampler = true;
                break;

            case HLSLTarget::kMslMacOS:
            case HLSLTarget::kMslIOS:
                if (stage == HLSLStage::kGeometry)
                {
                    outMessage = "MSL doesn't have GS.";
                    return false;
                }
                compiler.reset(new spirv_cross::CompilerMSL(std::move(words)));
                break;

            default:
                outMessage = "Unsupported SPIRV-Cross target " + std::to_string((uint32_t)target.language) + ".";
                return false;
            }

            compiler->set_entry_point(entryPoint.empty() ? String("main") : entryPoint, executionModel(stage));

            spirv_cross::CompilerGLSL::Options opts = compiler->get_common_options();
            if (hasVersion)
            {
                opts.version = intVersion;
            }
            opts.es = (target.language == HLSLTarget::kEssl);
            opts.force_temporary = false;
            opts.separate_shader_objects = true;
            opts.flatten_multidimensional_arrays = false;
            opts.enable_420pack_extension =
                (target.language == HLSLTarget::kGlsl) && (!hasVersion || opts.version >= 420);
            opts.vulkan_semantics = false;
            opts.vertex.fixup_clipspace = false;
            opts.vertex.flip_vert_y = false;
            opts.vertex.support_nonzero_base_instance = true;
            compiler->set_common_options(opts);

            if (target.language == HLSLTarget::kHlsl)
            {
                auto *hlsl = static_cast<spirv_cross::CompilerHLSL *>(compiler.get());
                auto hlslOpts = hlsl->get_hlsl_options();
                if (hasVersion)
                {
                    if (opts.version < 30)
                    {
                        outMessage = "HLSL shader model earlier than 3.0 is not supported.";
                        return false;
                    }
                    hlslOpts.shader_model = opts.version;
                }

                if (hlslOpts.shader_model <= 30)
                {
                    combinedImageSamplers = true;
                    buildDummySampler = true;
                }

                hlsl->set_hlsl_options(hlslOpts);
            }
            else if (target.language == HLSLTarget::kMslMacOS || target.language == HLSLTarget::kMslIOS)
            {
                auto *msl = static_cast<spirv_cross::CompilerMSL *>(compiler.get());
                auto mslOpts = msl->get_msl_options();
                if (hasVersion)
                {
                    mslOpts.msl_version = opts.version;
                }
                mslOpts.swizzle_texture_samples = false;
                mslOpts.platform = (target.language == HLSLTarget::kMslIOS)
                    ? spirv_cross::CompilerMSL::Options::iOS
                    : spirv_cross::CompilerMSL::Options::macOS;
                msl->set_msl_options(mslOpts);

                // DXC 给 texture / sampler 的 binding 与 HLSL register 一致（t0、s0...），
                // Metal 的 texture 与 sampler 各自从 0 编号
                const auto resources = msl->get_shader_resources();

                uint32_t textureBinding = 0;
                for (const auto &image : resources.separate_images)
                {
                    msl->set_decoration(image.id, spv::DecorationBinding, textureBinding++);
                }

                uint32_t samplerBinding = 0;
                for (const auto &sampler : resources.separate_samplers)
                {
                    msl->set_decoration(sampler.id, spv::DecorationBinding, samplerBinding++);
                }
            }

            // 顺序不能反：先建 dummy sampler，再合并
            uint32_t dummySampler = 0;
            if (buildDummySampler)
            {
                dummySampler = compiler->build_dummy_sampler_for_combined_images();
                if (dummySampler != 0)
                {
                    compiler->set_decoration(dummySampler, spv::DecorationDescriptorSet, 0);
                    compiler->set_decoration(dummySampler, spv::DecorationBinding, 0);
                }
            }

            if (combinedImageSamplers)
            {
                compiler->build_combined_image_samplers();

                // 合并后的 sampler2D 不带 binding，HLSL 的 t# / s# 只能经名字带到运行期：
                //   T3D_Combined_t<t#>s<s#|x>l<纹理名长度><纹理名><采样器名>
                // x 表示没有采样器（只做 texelFetch）。纹理名前不能放 '_'：纹理名可能以 '_' 开头，
                // SPIRV-Cross 会把连续的 "__" 合并掉。引擎运行期按这个格式解析，改了要同步改 GL4 / GLES3 后端
                for (const auto &remap : compiler->get_combined_image_samplers())
                {
                    const String texName = compiler->get_name(remap.image_id);
                    const String samplerName = (remap.sampler_id == dummySampler)
                        ? String() : compiler->get_name(remap.sampler_id);

                    String name;
                    if (compiler->has_decoration(remap.image_id, spv::DecorationBinding)
                        && (samplerName.empty() || compiler->has_decoration(remap.sampler_id, spv::DecorationBinding)))
                    {
                        name = "T3D_Combined_t"
                            + std::to_string(compiler->get_decoration(remap.image_id, spv::DecorationBinding))
                            + "s" + (samplerName.empty() ? String("x")
                                : std::to_string(compiler->get_decoration(remap.sampler_id, spv::DecorationBinding)))
                            + "l" + std::to_string(texName.size()) + texName + samplerName;
                    }
                    else
                    {
                        name = "SPIRV_Cross_Combined" + texName + compiler->get_name(remap.sampler_id);
                    }
                    compiler->set_name(remap.combined_id, name);
                }
            }

            if (target.language == HLSLTarget::kGlsl || target.language == HLSLTarget::kEssl)
            {
                renameStorageBuffers(*compiler);
            }

            if (target.language == HLSLTarget::kHlsl)
            {
                auto *hlsl = static_cast<spirv_cross::CompilerHLSL *>(compiler.get());
                const uint32_t newBuiltin = hlsl->remap_num_workgroups_builtin();
                if (newBuiltin != 0)
                {
                    compiler->set_decoration(newBuiltin, spv::DecorationDescriptorSet, 0);
                    compiler->set_decoration(newBuiltin, spv::DecorationBinding, 0);
                }
            }

            outSource = compiler->compile();
            return true;
        }
        catch (const std::exception &e)
        {
            outMessage = e.what();
            return false;
        }
    }
}
