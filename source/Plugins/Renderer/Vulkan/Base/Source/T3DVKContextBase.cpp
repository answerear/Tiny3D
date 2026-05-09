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


#include "T3DVKContextBase.h"
#include "T3DVKError.h"
#include "T3DVKMapping.h"

#include "spirv_reflect.h"


namespace Tiny3D
{
    #define LOG_TAG_VKCONTEXTBASE        "VKContextBase"

    //--------------------------------------------------------------------------

    namespace
    {
        ShaderConstantParam::DATA_TYPE getDataType(
            const SpvReflectTypeDescription *typeDesc,
            uint32_t numOfElements)
        {
            ShaderConstantParam::DATA_TYPE type = ShaderConstantParam::DATA_TYPE::DT_FLOAT;

            if (typeDesc->type_flags & SPV_REFLECT_TYPE_FLAG_BOOL)
            {
                type = (numOfElements > 1)
                    ? ShaderConstantParam::DATA_TYPE::DT_BOOL_ARRAY
                    : ShaderConstantParam::DATA_TYPE::DT_BOOL;
            }
            else if (typeDesc->type_flags & SPV_REFLECT_TYPE_FLAG_INT)
            {
                type = (numOfElements > 1)
                    ? ShaderConstantParam::DATA_TYPE::DT_INTEGER_ARRAY
                    : ShaderConstantParam::DATA_TYPE::DT_INTEGER;
            }
            else if (typeDesc->type_flags & SPV_REFLECT_TYPE_FLAG_FLOAT)
            {
                if (typeDesc->type_flags & SPV_REFLECT_TYPE_FLAG_MATRIX)
                {
                    // Matrix type
                    if (typeDesc->traits.numeric.matrix.row_count == 4
                        && typeDesc->traits.numeric.matrix.column_count == 4)
                    {
                        type = (numOfElements > 1)
                            ? ShaderConstantParam::DATA_TYPE::DT_MATRIX4_ARRAY
                            : ShaderConstantParam::DATA_TYPE::DT_MATRIX4;
                    }
                    else
                    {
                        type = (numOfElements > 1)
                            ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY
                            : ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                    }
                }
                else if (typeDesc->type_flags & SPV_REFLECT_TYPE_FLAG_VECTOR)
                {
                    // Vector type
                    if (typeDesc->traits.numeric.vector.component_count == 4)
                    {
                        type = (numOfElements > 1)
                            ? ShaderConstantParam::DATA_TYPE::DT_VECTOR4_ARRAY
                            : ShaderConstantParam::DATA_TYPE::DT_VECTOR4;
                    }
                    else
                    {
                        type = (numOfElements > 1)
                            ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY
                            : ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                    }
                }
                else
                {
                    // Scalar float
                    type = (numOfElements > 1)
                        ? ShaderConstantParam::DATA_TYPE::DT_FLOAT_ARRAY
                        : ShaderConstantParam::DATA_TYPE::DT_FLOAT;
                }
            }
            else if (typeDesc->type_flags & SPV_REFLECT_TYPE_FLAG_STRUCT)
            {
                type = ShaderConstantParam::DATA_TYPE::DT_STRUCT;
            }

            return type;
        }
    }

    //--------------------------------------------------------------------------

    VKContextBase::VKContextBase()
    {
    }

    //--------------------------------------------------------------------------

    VKContextBase::~VKContextBase()
    {
    }

    //--------------------------------------------------------------------------

    TResult VKContextBase::setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat)
    {
        // Vulkan uses [0,1] depth range (same as D3D11)
        static Matrix4 conversionMat(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.5f, 0.5f,
            0.0f, 0.0f, 0.0f, 1.0f
            );
        mViewMatrix = viewMat;
        mProjMatrix = conversionMat * projMat;
        mProjViewMatrix = mProjMatrix * mViewMatrix;

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VKContextBase::compileShader(ShaderVariant *shader)
    {
        TResult ret = T3D_OK;

        do
        {
            VkShaderStageFlagBits shaderStage;

            switch (shader->getShaderStage())
            {
            case SHADER_STAGE::kVertex:
                shaderStage = VK_SHADER_STAGE_VERTEX_BIT;
                break;
            case SHADER_STAGE::kPixel:
                shaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
            case SHADER_STAGE::kCompute:
                shaderStage = VK_SHADER_STAGE_COMPUTE_BIT;
                break;
            case SHADER_STAGE::kGeometry:
                shaderStage = VK_SHADER_STAGE_GEOMETRY_BIT;
                break;
            case SHADER_STAGE::kHull:
                shaderStage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
                break;
            case SHADER_STAGE::kDomain:
                shaderStage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
                break;
            case SHADER_STAGE::kUnknown:
            default:
                T3D_LOG_ERROR(LOG_TAG_VKCONTEXTBASE, "Invalid shader stage [%d] !", shader->getShaderStage());
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }

            if (T3D_FAILED(ret))
            {
                break;
            }

            size_t bytesLength = 0;
            const char *bytes = shader->getBytesCode(bytesLength);

            if (bytes == nullptr || bytesLength == 0)
            {
                ret = T3D_ERR_VK_COMPILE_SHADER;
                T3D_LOG_ERROR(LOG_TAG_VKCONTEXTBASE, "Shader bytecode is empty !");
                break;
            }

            // Verify SPIR-V magic number (0x07230203)
            if (bytesLength >= 4)
            {
                const uint32_t *pMagic = reinterpret_cast<const uint32_t*>(bytes);
                if (*pMagic != 0x07230203)
                {
                    ret = T3D_ERR_VK_COMPILE_SHADER;
                    T3D_LOG_ERROR(LOG_TAG_VKCONTEXTBASE, "Shader bytecode does not have valid SPIR-V magic number !");
                    break;
                }
            }

            // Validate SPIR-V using spirv-reflect
            SpvReflectShaderModule module;
            SpvReflectResult spvResult = spvReflectCreateShaderModule(bytesLength, bytes, &module);
            if (spvResult != SPV_REFLECT_RESULT_SUCCESS)
            {
                ret = T3D_ERR_VK_COMPILE_SHADER;
                T3D_LOG_ERROR(LOG_TAG_VKCONTEXTBASE, "Failed to parse SPIR-V bytecode ! Error code : %d", spvResult);
                break;
            }

            spvReflectDestroyShaderModule(&module);

            // SPIR-V bytecode is valid. The bytecode stays as-is in the shader.
            // VkShaderModule creation will happen when the shader is bound in VKContext.

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult VKContextBase::reflectShaderAllBindings(ShaderVariant *shader,
        ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams)
    {
        TResult ret = T3D_OK;

        do
        {
            size_t bytesLength = 0;
            const char *bytes = shader->getBytesCode(bytesLength);

            if (bytes == nullptr || bytesLength == 0)
            {
                ret = T3D_ERR_VK_SHADER_REFLECTION;
                T3D_LOG_ERROR(LOG_TAG_VKCONTEXTBASE, "Cannot reflect shader: bytecode is empty !");
                break;
            }

            // Parse SPIR-V with spirv-reflect
            SpvReflectShaderModule module;
            SpvReflectResult spvResult = spvReflectCreateShaderModule(bytesLength, bytes, &module);
            if (spvResult != SPV_REFLECT_RESULT_SUCCESS)
            {
                ret = T3D_ERR_VK_SHADER_REFLECTION;
                T3D_LOG_ERROR(LOG_TAG_VKCONTEXTBASE, "Failed to create reflection module ! Error code : %d", spvResult);
                break;
            }

            // Enumerate all descriptor bindings
            uint32_t bindingCount = 0;
            spvResult = spvReflectEnumerateDescriptorBindings(&module, &bindingCount, nullptr);
            if (spvResult != SPV_REFLECT_RESULT_SUCCESS)
            {
                ret = T3D_ERR_VK_SHADER_REFLECTION;
                T3D_LOG_ERROR(LOG_TAG_VKCONTEXTBASE, "Failed to enumerate descriptor bindings ! Error code : %d", spvResult);
                spvReflectDestroyShaderModule(&module);
                break;
            }

            TArray<SpvReflectDescriptorBinding*> bindings(bindingCount);
            spvResult = spvReflectEnumerateDescriptorBindings(&module, &bindingCount, bindings.data());
            if (spvResult != SPV_REFLECT_RESULT_SUCCESS)
            {
                ret = T3D_ERR_VK_SHADER_REFLECTION;
                T3D_LOG_ERROR(LOG_TAG_VKCONTEXTBASE, "Failed to get descriptor bindings ! Error code : %d", spvResult);
                spvReflectDestroyShaderModule(&module);
                break;
            }

            for (uint32_t i = 0; i < bindingCount; ++i)
            {
                SpvReflectDescriptorBinding *binding = bindings[i];

                switch (binding->descriptor_type)
                {
                case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                    {
                        // Uniform buffer (equivalent to D3D11 cbuffer)
                        String cbufferName = (binding->type_description->type_name != nullptr)
                            ? binding->type_description->type_name : binding->name;

                        // Strip "type." prefix from cbuffer name if present.
                        // SPIR-V compiled by ShaderConductor (DXC -> SPIR-V) prepends
                        // "type." to the cbuffer type name (e.g. "type.Tiny3DPerDraw").
                        // The engine material system uses the original name without
                        // this prefix (e.g. "Tiny3DPerDraw").
                        const String kTypePrefix = "type.";
                        if (cbufferName.size() > kTypePrefix.size()
                            && cbufferName.substr(0, kTypePrefix.size()) == kTypePrefix)
                        {
                            cbufferName = cbufferName.substr(kTypePrefix.size());
                        }

                        for (uint32_t j = 0; j < binding->block.member_count; ++j)
                        {
                            SpvReflectBlockVariable &member = binding->block.members[j];

                            // SPIR-V member names from spirv-reflect are already the
                            // original short names (e.g. "tiny3d_ObjectToWorld"),
                            // no prefix stripping needed.
                            String memberName = (member.name != nullptr) ? member.name : "";

                            uint32_t numOfElements = 1;
                            if (member.array.dims_count > 0)
                            {
                                numOfElements = member.array.dims[0];
                            }

                            ShaderConstantParam::DATA_TYPE dataType =
                                getDataType(member.type_description, numOfElements);

                            ShaderConstantParamPtr param = ShaderConstantParam::create(
                                cbufferName,
                                memberName,
                                binding->binding,
                                member.size,
                                member.offset,
                                dataType);
                            constantParams.emplace(param->getName(), param);

                            T3D_LOG_DEBUG(LOG_TAG_VKCONTEXTBASE,
                                "Shader reflection - cbuffer name : %s, "
                                "variable name : %s, type : %u, size : %u, "
                                "offset : %u",
                                param->getCBufferName().c_str(),
                                param->getName().c_str(),
                                param->getDataType(),
                                param->getDataSize(),
                                param->getDataOffset());
                        }
                    }
                    break;
                case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                    {
                        // Texture (equivalent to D3D11 SIT_TEXTURE)
                        String name = (binding->name != nullptr) ? binding->name : "";

                        ShaderSamplerParamPtr param;
                        const auto itr = samplerParams.find(name);
                        if (itr == samplerParams.end())
                        {
                            param = ShaderSamplerParam::create(name);
                            samplerParams.emplace(name, param);
                        }
                        else
                        {
                            param = itr->second;
                        }

                        T3D_ASSERT(param != nullptr);

                        param->setTexBinding(binding->binding);

                        // Map image dimension to TEXTURE_TYPE
                        TEXTURE_TYPE texType = TEXTURE_TYPE::TT_2D;
                        switch (binding->image.dim)
                        {
                        case SpvDim1D:
                            texType = TEXTURE_TYPE::TT_1D;
                            break;
                        case SpvDim2D:
                            texType = TEXTURE_TYPE::TT_2D;
                            break;
                        case SpvDim3D:
                            texType = TEXTURE_TYPE::TT_3D;
                            break;
                        case SpvDimCube:
                            texType = TEXTURE_TYPE::TT_CUBE;
                            break;
                        default:
                            texType = TEXTURE_TYPE::TT_2D;
                            break;
                        }

                        if (binding->image.arrayed)
                        {
                            if (texType == TEXTURE_TYPE::TT_2D)
                            {
                                texType = TEXTURE_TYPE::TT_2D_ARRAY;
                            }
                            else if (texType == TEXTURE_TYPE::TT_CUBE)
                            {
                                texType = TEXTURE_TYPE::TT_CUBE_ARRAY;
                            }
                        }

                        param->setTextureType(texType);

                        // For combined image sampler, also set the sampler binding
                        if (binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
                        {
                            param->setSamplerBinding(binding->binding);
                        }

                        T3D_LOG_DEBUG(LOG_TAG_VKCONTEXTBASE,
                            "Shader reflection - Name:%s, texture binding "
                            "point : %d, texture type : %d",
                            param->getName().c_str(),
                            param->getTexBinding(),
                            param->getTextureType());
                    }
                    break;
                case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
                    {
                        // Separate sampler (equivalent to D3D11 SIT_SAMPLER)
                        String name = (binding->name != nullptr) ? binding->name : "";

                        // Strip "sampler" prefix if present, to match texture name
                        String key = name;
                        if (StringUtil::startsWith(name, "sampler"))
                        {
                            key = name.substr(7);
                        }

                        ShaderSamplerParamPtr param;
                        const auto itr = samplerParams.find(key);
                        if (itr == samplerParams.end())
                        {
                            param = ShaderSamplerParam::create(key);
                            samplerParams.emplace(key, param);
                        }
                        else
                        {
                            param = itr->second;
                        }

                        T3D_ASSERT(param != nullptr);

                        param->setSamplerBinding(binding->binding);

                        T3D_LOG_DEBUG(LOG_TAG_VKCONTEXTBASE,
                            "Shader reflection - Name:%s, sampler binding "
                            "point : %d",
                            param->getName().c_str(),
                            param->getSamplerBinding());
                    }
                    break;
                default:
                    break;
                }

                if (T3D_FAILED(ret))
                {
                    break;
                }
            }

            spvReflectDestroyShaderModule(&module);

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult VKContextBase::reflectSamplerBindings(ShaderVariant *shader,
        ShaderSamplerParams &samplerParams)
    {
        // Sampler bindings are already handled in reflectShaderAllBindings
        // (via COMBINED_IMAGE_SAMPLER and separate SAMPLER types).
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
