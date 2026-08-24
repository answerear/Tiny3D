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


#include "T3DD3D11ContextBase.h"
#include "T3DD3D11Error.h"
#include "T3DD3D11Mapping.h"


namespace Tiny3D
{
    #define LOG_TAG_D3D11CONTEXTBASE        "D3D11ContextBase"

    //--------------------------------------------------------------------------

    D3D11ContextBase::D3D11ContextBase()
    {
    }

    //--------------------------------------------------------------------------

    D3D11ContextBase::~D3D11ContextBase()
    {
    }

    //--------------------------------------------------------------------------

    TResult D3D11ContextBase::setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat)
    {
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

    TResult D3D11ContextBase::compileShader(ShaderVariant *shader)
    {
        TResult ret = T3D_OK;
        
        do
        {
            String profile;
        
            switch (shader->getShaderStage())
            {
            case SHADER_STAGE::kVertex:
                profile = "vs_5_0";
                break;
            case SHADER_STAGE::kPixel:
                profile = "ps_5_0";
                break;
            case SHADER_STAGE::kCompute:
                profile = "cs_5_0";
                break;
            case SHADER_STAGE::kGeometry:
                profile = "gs_5_0";
                break;
            case SHADER_STAGE::kHull:
                profile = "hs_5_0";
                break;
            case SHADER_STAGE::kDomain:
                profile = "ds_5_0";
                break;
            case SHADER_STAGE::kUnknown:
            default:
                T3D_LOG_ERROR(LOG_TAG_D3D11CONTEXTBASE, "Invalid shader stage [%d] !", shader->getShaderStage());
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }

            if (T3D_FAILED(ret))
            {
                break;
            }
            
            size_t bytesLength = 0;
            const char *bytes = shader->getBytesCode(bytesLength);

#if defined (T3D_DEBUG)
            UINT shaderCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_OPTIMIZATION_LEVEL0;
#else
            UINT shaderCompileFlags = 0;
#endif

            String sourceName = shader->getPass()->getTechnique()->getShader()->getName();
            sourceName = shader->getShaderKeyword().getName() + (!shader->getShaderKeyword().getName().empty() ? "-" : "") + profile + "-" + sourceName;
            
            ID3DBlob *shaderBlob = nullptr;
            ID3DBlob *errorBlob = nullptr;
            HRESULT hr = D3DCompile(bytes, bytesLength, sourceName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", profile.c_str(), shaderCompileFlags, 0, &shaderBlob, &errorBlob);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_COMPILE_SHADER;
                String error;
                if (errorBlob != nullptr)
                {
                    error.assign(static_cast<const char *>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());
                }
                T3D_LOG_ERROR(LOG_TAG_D3D11CONTEXTBASE, "Compile shader failed ! (Keyword:%s, Target:%s, ) DX ERROR [%d] (%s)", shader->getShaderKeyword().getName().c_str(), profile.c_str(), hr, error.c_str());
                D3D_SAFE_RELEASE(shaderBlob);
                D3D_SAFE_RELEASE(errorBlob);
                break;
            }

            shader->setBytesCode(static_cast<const char*>(shaderBlob->GetBufferPointer()), shaderBlob->GetBufferSize());
            D3D_SAFE_RELEASE(shaderBlob);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ContextBase::reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams)
    {
        TResult ret = T3D_OK;

        do
        {
            auto getOriginalName = [](const String &cbufferName, const String &cname, String &originalCBufferName, String &originalCName)
            {
                if (cbufferName.empty() || cname.empty())
                {
                    return false;
                }

                String::size_type offset = 0, count = 0;
                
                if (cbufferName == "type_Globals")
                {
                    offset = 4;
                    count =  cbufferName.size() - offset;
                }
                else
                {
                    offset = 5;
                    count =  cbufferName.size() - offset;
                }

                originalCBufferName = cbufferName.substr(offset, count);

                offset = originalCBufferName.size() + 1;
                count = cname.size() - offset;
                originalCName = cname.substr(offset, count);

                return true;
            };
            
            size_t bytesLength = 0;
            const char *bytes = shader->getBytesCode(bytesLength);
            ID3DBlob *pShaderBlob = nullptr;
            HRESULT hr = D3DCreateBlob(bytesLength, &pShaderBlob);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_CREATE_BLOB;
                T3D_LOG_ERROR(LOG_TAG_D3D11CONTEXTBASE, "Create blob with shader code failed ! DX ERROR [%d]", hr);
                break;
            }

            void *pData = pShaderBlob->GetBufferPointer();
            memcpy(pData, bytes, bytesLength);
            ID3D11ShaderReflection *pReflection = nullptr;
            hr = D3DReflect(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflection);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_SHADER_REFLECTION;
                T3D_LOG_ERROR(LOG_TAG_D3D11CONTEXTBASE, "Failed to reflect shader ! DX ERROR [%d]", hr);
                D3D_SAFE_RELEASE(pShaderBlob);
                break;
            }

            D3D11_SHADER_DESC shaderDesc;
            hr = pReflection->GetDesc(&shaderDesc);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_GET_SHADER_DESC;
                T3D_LOG_ERROR(LOG_TAG_D3D11CONTEXTBASE, "Get shader description failed ! DX ERROR [%d]", hr);
                D3D_SAFE_RELEASE(pReflection);
                D3D_SAFE_RELEASE(pShaderBlob);
                break;
            }

            for (UINT i = 0; i < shaderDesc.BoundResources; ++i)
            {
                D3D11_SHADER_INPUT_BIND_DESC bindDesc;
                pReflection->GetResourceBindingDesc(i, &bindDesc);

                switch (bindDesc.Type)
                {
                case D3D_SIT_CBUFFER:
                    {
                        ID3D11ShaderReflectionConstantBuffer *pConstBufferReflection = pReflection->GetConstantBufferByName(bindDesc.Name);
                        D3D11_SHADER_BUFFER_DESC bufferDesc;
                        pConstBufferReflection->GetDesc(&bufferDesc);
                    
                        for (UINT j = 0; j < bufferDesc.Variables; ++j)
                        {
                            ID3D11ShaderReflectionVariable* pVariableReflection = pConstBufferReflection->GetVariableByIndex(j);

                            D3D11_SHADER_VARIABLE_DESC variableDesc;
                            pVariableReflection->GetDesc(&variableDesc);

                            ID3D11ShaderReflectionType *d3dSRType = pVariableReflection->GetType();
                            D3D11_SHADER_TYPE_DESC d3dSRTypeDesc;
                            d3dSRType->GetDesc(&d3dSRTypeDesc);

                            String cbufferName, cname;
                            bool rval = getOriginalName(bindDesc.Name, variableDesc.Name, cbufferName, cname);
                            T3D_ASSERT(rval);
                            ShaderConstantParamPtr param = ShaderConstantParam::create(cbufferName, cname, bindDesc.BindPoint, variableDesc.Size, variableDesc.StartOffset, D3D11Mapping::get(d3dSRTypeDesc.Type, d3dSRTypeDesc.Rows, d3dSRTypeDesc.Columns, d3dSRTypeDesc.Elements));
                            constantParams.emplace(param->getName(), param);

                            T3D_LOG_DEBUG(LOG_TAG_D3D11CONTEXTBASE, "Shader reflection - cbuffer name : %s, variable name : %s, type : %u, size : %u, offset : %u, rows : %u, cols : %u, elements: %u",
                                param->getCBufferName().c_str(), param->getName().c_str(), param->getDataType(), param->getDataSize(), param->getDataOffset(), d3dSRTypeDesc.Rows, d3dSRTypeDesc.Columns, d3dSRTypeDesc.Elements);
                        }
                    }
                    break;
                case D3D_SIT_TEXTURE:
                    {
                        String name = bindDesc.Name;

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

                        param->setTexBinding(bindDesc.BindPoint);
                        param->setTextureType(D3D11Mapping::get(bindDesc.Dimension));

                        T3D_LOG_DEBUG(LOG_TAG_D3D11CONTEXTBASE, "Shader reflection - Name:%s, texture binding point : %d, texture type : %d", param->getName().c_str(), param->getTexBinding(), param->getTextureType());
                    }
                    break;
                case D3D_SIT_SAMPLER:
                    {
                        String name = bindDesc.Name;

                        if (!StringUtil::startsWith(name, "sampler"))
                        {
                            ret = T3D_ERR_D3D11_INVALID_SHADER_SAMPLER_NAME;
                            T3D_LOG_ERROR(LOG_TAG_D3D11CONTEXTBASE, "Invalid shader sampler name (%s) ! It must start with 'sampler_' !", name.c_str());
                            break;
                        }

                        String key = name.substr(7);

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

                        param->setSamplerBinding(bindDesc.BindPoint);

                        T3D_LOG_DEBUG(LOG_TAG_D3D11CONTEXTBASE, "Shader reflection - Name:%s, sampler binding point : %d", param->getName().c_str(), param->getSamplerBinding());
                    }
                    break;
                }

                if (T3D_FAILED(ret))
                {
                    break;
                }
            }

            D3D_SAFE_RELEASE(pReflection);
            D3D_SAFE_RELEASE(pShaderBlob);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ContextBase::reflectSamplerBindings(ShaderVariant *shader, ShaderSamplerParams &samplerParams)
    {
        TResult ret = T3D_OK;

        ID3DBlob *pShaderBlob = nullptr;
        ID3D11ShaderReflection *pReflection = nullptr;

        do
        {
            size_t bytesLength = 0;
            const char *bytes = shader->getBytesCode(bytesLength);
            HRESULT hr = D3DCreateBlob(bytesLength, &pShaderBlob);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_CREATE_BLOB;
                T3D_LOG_ERROR(LOG_TAG_D3D11CONTEXTBASE, "reflectSamplerBindings: D3DCreateBlob failed ! DX ERROR [%d]", hr);
                break;
            }

            memcpy(pShaderBlob->GetBufferPointer(), bytes, bytesLength);

            hr = D3DReflect(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void **)&pReflection);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_SHADER_REFLECTION;
                T3D_LOG_ERROR(LOG_TAG_D3D11CONTEXTBASE, "reflectSamplerBindings: D3DReflect failed ! DX ERROR [%d]", hr);
                break;
            }

            D3D11_SHADER_DESC shaderDesc;
            hr = pReflection->GetDesc(&shaderDesc);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_GET_SHADER_DESC;
                T3D_LOG_ERROR(LOG_TAG_D3D11CONTEXTBASE, "reflectSamplerBindings: GetDesc failed ! DX ERROR [%d]", hr);
                break;
            }

            // 只回填已有的 sampler 参数，不新建 —— 新建是 reflectShaderAllBindings 的职责
            for (UINT i = 0; i < shaderDesc.BoundResources; ++i)
            {
                D3D11_SHADER_INPUT_BIND_DESC bindDesc;
                pReflection->GetResourceBindingDesc(i, &bindDesc);

                if (bindDesc.Type == D3D_SIT_TEXTURE)
                {
                    const auto itr = samplerParams.find(String(bindDesc.Name));
                    if (itr != samplerParams.end())
                    {
                        itr->second->setTexBinding(bindDesc.BindPoint);
                        itr->second->setTextureType(D3D11Mapping::get(bindDesc.Dimension));
                    }
                }
                else if (bindDesc.Type == D3D_SIT_SAMPLER)
                {
                    String name = bindDesc.Name;
                    if (StringUtil::startsWith(name, "sampler"))
                    {
                        const auto itr = samplerParams.find(name.substr(7));
                        if (itr != samplerParams.end())
                        {
                            itr->second->setSamplerBinding(bindDesc.BindPoint);
                        }
                    }
                }
            }
        } while (false);

        D3D_SAFE_RELEASE(pReflection);
        D3D_SAFE_RELEASE(pShaderBlob);

        return ret;
    }

    //--------------------------------------------------------------------------
}
