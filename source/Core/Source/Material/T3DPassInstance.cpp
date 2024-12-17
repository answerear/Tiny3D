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


#include "Material/T3DPassInstance.h"
#include "Material/T3DShaderVariantInstance.h"
#include "Material/T3DPass.h"
#include "Material/T3DTechniqueInstance.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    PassInstancePtr PassInstance::create(TechniqueInstance *parent, PassPtr pass)
    {
        return new PassInstance(parent, pass);
    }

    //--------------------------------------------------------------------------

    PassInstance::PassInstance(TechniqueInstance *parent, PassPtr pass)
        : mTechInstance(parent)
        , mPass(pass)
    {
        if (mPass != nullptr)
        {
            mCurrentKeyword = pass->getCurrentKeyword();
            switchKeyword(mCurrentKeyword);
        }
    }

    //--------------------------------------------------------------------------
    
    TResult PassInstance::switchShaderVariants(const ShaderKeyword &keyword, const ShaderVariants &shaderVariants, ShaderVariantInstancePtr &currentVariant)
    {
        TResult ret = T3D_OK;

        do
        {
            if (shaderVariants.empty())
            {
                // 没有对应 stage 的着色器
                currentVariant = nullptr;
                break;
            }
            
            auto itr = shaderVariants.find(keyword);
            if (itr == shaderVariants.end())
            {
                // 着色器里面没有对应的 keyword
                T3D_LOG_WARNING(LOG_TAG_RESOURCE, "Could not find shader variant when switch keyword (%s) !", keyword.getName().c_str());
                break;
            }

            // 禁用当前 shader
            currentVariant = nullptr;

            // 使用对应 shader
            if (itr->second != nullptr)
            {
                currentVariant = ShaderVariantInstance::create(this, itr->second);
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult PassInstance::switchKeywords(const StringArray &enableKeys, const StringArray &disableKeys)
    {
        TResult ret = T3D_OK;

        do
        {
            ShaderKeyword kw = mCurrentKeyword;
            for (const auto &key : enableKeys)
            {
                kw.addKeyword(key);
            }

            for (const auto &key : disableKeys)
            {
                kw.removeKeyword(key);
            }

            kw.generate();

            if (kw == mCurrentKeyword)
            {
                // 跟当前相同的关键字，不做任何事情，直接退出
                break;
            }

            ret = switchKeyword(kw);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult PassInstance::switchKeyword(const ShaderKeyword &kw)
    {
        TResult ret = T3D_OK;

        do
        {
            // vertex shader
            ShaderVariantInstancePtr currentVS = mCurrentVS;
            ret = switchShaderVariants(kw, mPass->getVertexShaders(), currentVS);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // hull shader
            ShaderVariantInstancePtr currentHS = mCurrentHS;
            ret = switchShaderVariants(kw, mPass->getHullShaders(), currentHS);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // domain shader
            ShaderVariantInstancePtr currentDS = mCurrentDS;
            ret = switchShaderVariants(kw, mPass->getDomainShaders(), currentDS);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // geometry shader
            ShaderVariantInstancePtr currentGS = mCurrentGS;
            ret = switchShaderVariants(kw, mPass->getGeometryShaders(), currentGS);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // pixel shader
            ShaderVariantInstancePtr currentPS = mCurrentPS;
            ret = switchShaderVariants(kw, mPass->getPixelShaders(), currentPS);
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            // 全部都成功，才统一切当前的 shader
            mCurrentVS = currentVS;
            mCurrentHS = currentHS;
            mCurrentDS = currentDS;
            mCurrentGS = currentGS;
            mCurrentPS = currentPS;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------
}


