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


#include "Material/T3DShaderVariant.h"
#include "Material/T3DPass.h"
#include "RHI/T3DRHIContext.h"
#include "Kernel/T3DAgent.h"
#include "Material/T3DTechnique.h"
#include "Resource/T3DShader.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ShaderVariantPtr ShaderVariant::create(ShaderKeyword &&keyword, const String &code)
    {
        return new ShaderVariant(std::move(keyword), code);
    }

    //--------------------------------------------------------------------------

    ShaderVariant::ShaderVariant(ShaderKeyword &&key, const String &code)
    {
        mShaderKeyword = new ShaderKeyword(std::move(key));
        mShaderKeyword->generate();
        setSourceCode(code.c_str(), code.length());
    }

    //--------------------------------------------------------------------------

    ShaderVariant::~ShaderVariant()
    {
        T3D_SAFE_DELETE(mShaderKeyword);
    }

    //--------------------------------------------------------------------------

    TResult ShaderVariant::compile()
    {
        TResult ret = T3D_OK;

        do
        {
            if (mHasCompiled)
            {
                // 已经编译过，不编译了，也不反射常量、纹理、纹理采样
                break;
            }

            RHIContextPtr ctx = T3D_AGENT.getActiveRHIContext();

            // 编译 shader
            ret = ctx->compileShader(this);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_RENDER, "Failed to compile shader ! ERROR [%d]", ret);
                break;
            }

            // if (mConstantParams.empty() && mTexSamplerBindings.empty())
            // {
            //     // 反射获取对应的绑定关系
            //     ret = ctx->reflectShaderAllBindings(this, mConstantParams, mTexSamplerBindings);
            //     if (T3D_FAILED(ret))
            //     {
            //         T3D_LOG_ERROR(LOG_TAG_RENDER, "Failed to reflect shader for all bindings ! ERROR [%d]", ret);
            //         break;
            //     }
            // }

            switch (mShaderStage)
            {
            case SHADER_STAGE::kVertex:
                mRHIShader = ctx->createVertexShader(this);
                break;
            case SHADER_STAGE::kPixel:
                mRHIShader = ctx->createPixelShader(this);
                break;
            case SHADER_STAGE::kCompute:
                mRHIShader = ctx->createComputeShader(this);
                break;
            case SHADER_STAGE::kGeometry:
                mRHIShader = ctx->createGeometryShader(this);
                break;
            case SHADER_STAGE::kHull:
                mRHIShader = ctx->createHullShader(this);
                break;
            case SHADER_STAGE::kDomain:
                mRHIShader = ctx->createDomainShader(this);
                break;
            default:
                T3D_ASSERT(0);
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ShaderVariant::reflect()
    {
        TResult ret = T3D_OK;
        char *code = nullptr;
        size_t bytesCode = 0;
        
        do
        {
            RHIContextPtr ctx = T3D_AGENT.getActiveRHIContext();

            bool isCompiled = hasCompiled();
            if (!isCompiled)
            {
                // 备份 shader 源码
                code = new char[mBytesCodeSize];
                bytesCode = mBytesCodeSize;
                memcpy(code, mBytesCode, bytesCode);

                // 编译 shader
                ret = ctx->compileShader(this);
                if (T3D_FAILED(ret))
                {
                    T3D_LOG_ERROR(LOG_TAG_RENDER, "Failed to compile shader ! ERROR [%d]", ret);
                    break;
                }
            }

            // 反射获取对应的绑定关系
            ret = ctx->reflectShaderAllBindings(this, mConstantParams, mSamplerParams);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_RENDER, "Failed to reflect shader for all bindings ! ERROR [%d]", ret);
                break;
            }

            if (!isCompiled)
            {
                // 还原 shader 源码
                copyCode(code, bytesCode);
            }

            // 根据反射信息，生成 constant param
            // T3D_ASSERT(mPass != nullptr);
            // T3D_ASSERT(mPass->getTechnique() != nullptr);
            // Shader *shader = mPass->getTechnique()->getShader();
            // T3D_ASSERT(shader != nullptr);
            //
            // for (auto binding : mConstantBindings)
            // {
            //     for (auto varBinding : binding.second.variables)
            //     {
            //         auto itr = shader->getConstantParams().find(varBinding.first);;
            //         if (itr == shader->getConstantParams().end())
            //         {
            //             // 没有添加过，则添加
            //             ShaderConstantParamPtr param = ShaderConstantParam::create(varBinding.second.name, varBinding.second.size, varBinding.second.type);
            //             shader->addConstantParam(param);
            //         }
            //     }
            // }
            
        } while (false);

        T3D_SAFE_DELETE(code);

        return ret;
    }

    //--------------------------------------------------------------------------
}


