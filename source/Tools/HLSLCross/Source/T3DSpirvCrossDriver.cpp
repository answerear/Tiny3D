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
            if (buildDummySampler)
            {
                const uint32_t sampler = compiler->build_dummy_sampler_for_combined_images();
                if (sampler != 0)
                {
                    compiler->set_decoration(sampler, spv::DecorationDescriptorSet, 0);
                    compiler->set_decoration(sampler, spv::DecorationBinding, 0);
                }
            }

            if (combinedImageSamplers)
            {
                compiler->build_combined_image_samplers();

                // 引擎运行期按这个名字找 uniform，改了要同步改 GL 后端
                for (const auto &remap : compiler->get_combined_image_samplers())
                {
                    compiler->set_name(remap.combined_id, "SPIRV_Cross_Combined"
                        + compiler->get_name(remap.image_id) + compiler->get_name(remap.sampler_id));
                }
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
