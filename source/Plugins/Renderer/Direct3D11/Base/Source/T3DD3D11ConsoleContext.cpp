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


#include "T3DD3D11ConsoleContext.h"
#include "T3DD3D11ConsoleWindow.h"
#include "T3DD3D11ConsoleError.h"
#include "T3DD3D11Mapping.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11ConsoleContextPtr D3D11ConsoleContext::create()
    {
        D3D11ConsoleContextPtr renderer = T3D_NEW D3D11ConsoleContext();
        // renderer->release();
        return renderer;
    }

    //--------------------------------------------------------------------------

    D3D11ConsoleContext::D3D11ConsoleContext()
    {
        
    }

    //--------------------------------------------------------------------------

    D3D11ConsoleContext::~D3D11ConsoleContext()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat)
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

    RHIRenderTargetPtr D3D11ConsoleContext::createRenderWindow(RenderWindow *renderWindow)
    {
        return D3D11ConsoleWindow::create();
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer2DPtr D3D11ConsoleContext::createRenderTexture(PixelBuffer2D *buffer)
    {
        return nullptr;
    }
    
    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::setRenderTarget(RenderTarget *renderTarget)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::resetRenderTarget()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11ConsoleContext::setViewport(const Viewport &viewport)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11ConsoleContext::clearColor(const ColorRGB &color)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::clearDepth(Real depth)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11ConsoleContext::clearDepthStencil(Real depth, uint32_t stencil)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIBlendStatePtr D3D11ConsoleContext::createBlendState(BlendState *state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIDepthStencilStatePtr D3D11ConsoleContext::createDepthStencilState(DepthStencilState *state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIRasterizerStatePtr D3D11ConsoleContext::createRasterizerState(RasterizerState *state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHISamplerStatePtr D3D11ConsoleContext::createSamplerState(SamplerState *state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11ConsoleContext::setBlendState(BlendState *state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11ConsoleContext::setDepthStencilState(DepthStencilState *state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11ConsoleContext::setRasterizerState(RasterizerState *state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIVertexDeclarationPtr D3D11ConsoleContext::createVertexDeclaration(VertexDeclaration *decl)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::setVertexDeclaration(VertexDeclaration *decl)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    RHIVertexBufferPtr D3D11ConsoleContext::createVertexBuffer(VertexBuffer *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11ConsoleContext::setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIIndexBufferPtr D3D11ConsoleContext::createIndexBuffer(IndexBuffer *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11ConsoleContext::setIndexBuffer(IndexBuffer *buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIConstantBufferPtr D3D11ConsoleContext::createConstantBuffer(ConstantBuffer *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer1DPtr D3D11ConsoleContext::createPixelBuffer1D(PixelBuffer1D *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer2DPtr D3D11ConsoleContext::createPixelBuffer2D(PixelBuffer2D *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer3DPtr D3D11ConsoleContext::createPixelBuffer3D(PixelBuffer3D *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr D3D11ConsoleContext::createVertexShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11ConsoleContext::setVertexShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11ConsoleContext::setVSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr D3D11ConsoleContext::createPixelShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11ConsoleContext::setPixelShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11ConsoleContext::setPSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    RHIShaderPtr D3D11ConsoleContext::createHullShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11ConsoleContext::setHullShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11ConsoleContext::setHSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr D3D11ConsoleContext::createDomainShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11ConsoleContext::setDomainShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11ConsoleContext::setDSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr D3D11ConsoleContext::createGeometryShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11ConsoleContext::setGeometryShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11ConsoleContext::setGSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr D3D11ConsoleContext::createComputeShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11ConsoleContext::setComputeShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11ConsoleContext::setCSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return T3D_OK;
    }


    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::compileShader(ShaderVariant *shader)
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
                T3D_LOG_ERROR(LOG_TAG_D3D11CONSOLERENDERER, "Invalid shader stage [%d] !", shader->getShaderStage());
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
                T3D_LOG_ERROR(LOG_TAG_D3D11CONSOLERENDERER, "Compile shader failed ! (Keyword:%s, Target:%s, ) DX ERROR [%d] (%s)", shader->getShaderKeyword().getName().c_str(), profile.c_str(), hr, error.c_str());
                D3D_SAFE_RELEASE(shaderBlob);
                D3D_SAFE_RELEASE(errorBlob);
                break;
            }

            // 把编译后的设置到 shader 里面
            shader->setBytesCode(static_cast<const char*>(shaderBlob->GetBufferPointer()), shaderBlob->GetBufferSize());
            D3D_SAFE_RELEASE(shaderBlob);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams)
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
                    // 全局常量缓冲区，全局常量缓冲区的常量命名是以 "_Globals" 开头的
                    offset = 4;
                    count =  cbufferName.size() - offset;
                }
                else
                {
                    // 非全局的，这个命名规则不一样
                    offset = 5;
                    count =  cbufferName.size() - offset;
                }

                // 提取原始的常量缓冲区名称
                originalCBufferName = cbufferName.substr(offset, count);

                // 提取原始的常量名称
                offset = originalCBufferName.size() + 1;
                count = cname.size() - offset;
                originalCName = cname.substr(offset, count);

                return true;
            };
            
            // 创建 shader 字节码 D3D11 对象
            size_t bytesLength = 0;
            const char *bytes = shader->getBytesCode(bytesLength);
            ID3DBlob *pShaderBlob = nullptr;
            HRESULT hr = D3DCreateBlob(bytesLength, &pShaderBlob);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_CREATE_BLOB;
                T3D_LOG_ERROR(LOG_TAG_D3D11CONSOLERENDERER, "Create blob with shader code failed ! DX ERROR [%d]", hr);
                break;
            }

            // 反射 shader
            void *pData = pShaderBlob->GetBufferPointer();
            memcpy(pData, bytes, bytesLength);
            ID3D11ShaderReflection *pReflection = nullptr;
            hr = D3DReflect(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflection);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_SHADER_REFLECTION;
                T3D_LOG_ERROR(LOG_TAG_D3D11CONSOLERENDERER, "Failed to reflect shader ! DX ERROR [%d]", hr);
                D3D_SAFE_RELEASE(pShaderBlob);
                break;
            }

            // 获取 shader 信息
            D3D11_SHADER_DESC shaderDesc;
            hr = pReflection->GetDesc(&shaderDesc);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_GET_SHADER_DESC;
                T3D_LOG_ERROR(LOG_TAG_D3D11CONSOLERENDERER, "Get shader description failed ! DX ERROR [%d]", hr);
                break;
            }
            
            uint32_t samplerIndex = 0;

            for (UINT i = 0; i < shaderDesc.BoundResources; ++i)
            {
                D3D11_SHADER_INPUT_BIND_DESC bindDesc;
                pReflection->GetResourceBindingDesc(i, &bindDesc);

                switch (bindDesc.Type)
                {
                case D3D_SIT_CBUFFER:   // 常量缓冲区
                    {
                        // if (strncmp(bindDesc.Name, TINY3D_CBUFFER_PER_DRAW, TINY3D_CBUFFER_PER_DRAW_LEN) == 0
                        //     || strncmp(bindDesc.Name, TINY3D_CBUFFER_PER_FRAME, TINY3D_CBUFFER_PER_FRAME_LEN) == 0)
                        // {
                        //     // 这里跳开两个 cbuffer ，这两个 cbuffer 内部使用，不反射给外部使用
                        //     break;
                        // }

                        // ShaderConstantBinding constBinding;
                        // constBinding.name = bindDesc.Name;
                        // constBinding.binding = bindDesc.BindPoint;

                        uint32_t size = 0;
                    
                        // 获取常量缓冲区反射对象
                        ID3D11ShaderReflectionConstantBuffer *pConstBufferReflection = pReflection->GetConstantBufferByName(bindDesc.Name);
                        D3D11_SHADER_BUFFER_DESC bufferDesc;
                        pConstBufferReflection->GetDesc(&bufferDesc);
                    
                        // 遍历常量缓冲区中的所有常量
                        for (UINT j = 0; j < bufferDesc.Variables; ++j)
                        {
                            // 获取常量反射对象
                            ID3D11ShaderReflectionVariable* pVariableReflection = pConstBufferReflection->GetVariableByIndex(j);

                            // 获取常量描述
                            D3D11_SHADER_VARIABLE_DESC variableDesc;
                            pVariableReflection->GetDesc(&variableDesc);

                            // 常量类型
                            ID3D11ShaderReflectionType *d3dSRType = pVariableReflection->GetType();
                            D3D11_SHADER_TYPE_DESC d3dSRTypeDesc;
                            d3dSRType->GetDesc(&d3dSRTypeDesc);

                            String cbufferName, cname;
                            bool rval = getOriginalName(bindDesc.Name, variableDesc.Name, cbufferName, cname);
                            T3D_ASSERT(rval);
                            ShaderConstantParamPtr param = ShaderConstantParam::create(cbufferName, cname, bindDesc.BindPoint, variableDesc.Size, variableDesc.StartOffset, D3D11Mapping::get(d3dSRTypeDesc.Type, d3dSRTypeDesc.Rows, d3dSRTypeDesc.Columns, d3dSRTypeDesc.Elements));
                            constantParams.emplace(param->getName(), param);
                            
                            // ShaderVariableBinding varBinding;
                            // varBinding.name = variableDesc.Name;
                            // varBinding.offset = variableDesc.StartOffset;
                            // varBinding.size = variableDesc.Size;
                            // varBinding.type = D3D11Mapping::get(d3dSRTypeDesc.Type, d3dSRTypeDesc.Rows, d3dSRTypeDesc.Columns);
                            // size += varBinding.size;
                            // constBinding.variables.emplace(varBinding.name, varBinding);

                            T3D_LOG_DEBUG(LOG_TAG_D3D11CONSOLERENDERER, "Shader reflection - cbuffer name : %s, variable name : %s, type : %u, size : %u, offset : %u, rows : %u, cols : %u, elements: %u",
                                param->getCBufferName().c_str(), param->getName().c_str(), param->getDataType(), param->getDataSize(), param->getDataOffset(), d3dSRTypeDesc.Rows, d3dSRTypeDesc.Columns, d3dSRTypeDesc.Elements);
                        }

                        // constBinding.size = size;
                        // constantBindings.emplace(constBinding.name, constBinding);
                    }
                    break;
                case D3D_SIT_TEXTURE:   // 纹理
                    {
                        String name = bindDesc.Name;

                        // auto itr = texSamplerBindings.find(name);
                        // if (itr == texSamplerBindings.end())
                        // {
                        //     // 没有，则新建一个
                        //     ShaderTexSamplerBinding texSamplerBinding;
                        //     texSamplerBinding.texBinding.name = name;
                        //     texSamplerBinding.texBinding.binding = bindDesc.BindPoint;
                        //     texSamplerBinding.texBinding.bindingCount = bindDesc.BindCount;
                        //     texSamplerBinding.texBinding.texType = D3D11Mapping::get(bindDesc.Dimension);
                        //
                        //     texSamplerBindings.emplace(name, texSamplerBinding);
                        //
                        //     T3D_LOG_DEBUG(LOG_TAG_D3D11RENDERER, "Shader reflection - New (name:%s). texture name : %s, binding point : %d, binding count : %d, texture type : %d",
                        //         name.c_str(), texSamplerBinding.texBinding.name.c_str(), texSamplerBinding.texBinding.binding, texSamplerBinding.texBinding.bindingCount, texSamplerBinding.texBinding.texType);
                        // }
                        // else
                        // {
                        //     // 已有，用已有的
                        //     ShaderTexSamplerBinding &texSamplerBinding = itr->second;
                        //     texSamplerBinding.texBinding.name = name;
                        //     texSamplerBinding.texBinding.binding = bindDesc.BindPoint;
                        //     texSamplerBinding.texBinding.bindingCount = bindDesc.BindCount;
                        //     texSamplerBinding.texBinding.texType = D3D11Mapping::get(bindDesc.Dimension);
                        //     T3D_LOG_DEBUG(LOG_TAG_D3D11RENDERER, "Shader reflection - Already exists (name:%s). texture name : %s, binding point : %d, binding count : %d, texture type : %d",
                        //         name.c_str(), texSamplerBinding.texBinding.name.c_str(), texSamplerBinding.texBinding.binding, texSamplerBinding.texBinding.bindingCount, texSamplerBinding.texBinding.texType);
                        // }

                        ShaderSamplerParamPtr param;
                        const auto itr = samplerParams.find(name);
                        if (itr == samplerParams.end())
                        {
                            // 没有，新建一个
                            param = ShaderSamplerParam::create(name);
                            samplerParams.emplace(name, param);
                        }
                        else
                        {
                            // 已有，更新信息
                            param = itr->second;
                        }

                        T3D_ASSERT(param != nullptr);

                        param->setTexBinding(samplerIndex);
                        param->setTextureType(D3D11Mapping::get(bindDesc.Dimension));

                        T3D_LOG_DEBUG(LOG_TAG_D3D11CONSOLERENDERER, "Shader reflection - Name:%s, texture binding point : %d, texture type : %d", param->getName().c_str(), param->getTexBinding(), param->getTextureType());
                    }
                    break;
                case D3D_SIT_SAMPLER:   // 纹理采样器
                    {
                        String name = bindDesc.Name;

                        if (!StringUtil::startsWith(name, "sampler"))
                        {
                            // sampler 一定要以 sampler_ 开头
                            ret = T3D_ERR_D3D11_INVALID_SHADER_SAMPLER_NAME;
                            T3D_LOG_ERROR(LOG_TAG_D3D11CONSOLERENDERER, "Invalid shader sampler name (%s) ! It must start with 'sampler_' !", name.c_str());
                            break;
                        }

                        String key = name.substr(7);

                        ShaderSamplerParamPtr param;
                        const auto itr = samplerParams.find(key);
                        if (itr == samplerParams.end())
                        {
                            // 没有，新建一个
                            param = ShaderSamplerParam::create(key);
                            samplerParams.emplace(key, param);
                        }
                        else
                        {
                            // 已有，更新信息
                            param = itr->second;
                        }

                        T3D_ASSERT(param != nullptr);

                        param->setSamplerBinding(samplerIndex);

                        T3D_LOG_DEBUG(LOG_TAG_D3D11CONSOLERENDERER, "Shader reflection - Name:%s, sampler binding point : %d", param->getName().c_str(), param->getSamplerBinding());

                        samplerIndex++;
                    }
                    break;
                }

                if (T3D_FAILED(ret))
                {
                    break;
                }
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::reflectSamplerBindings(ShaderVariant *shader, ShaderSamplerParams &samplerParams)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::setPrimitiveType(PrimitiveType primitive)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11ConsoleContext::render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::render(uint32_t vertexCount, uint32_t startVertex)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::reset()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::blit(Texture *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset, size_t size, size_t dstOffset)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    TResult D3D11ConsoleContext::writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}

