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


#include "Material/T3DPassInstance.h"
#include "Material/T3DShaderVariantInstance.h"
#include "Material/T3DPass.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    PassInstancePtr PassInstance::create(PassPtr pass)
    {
        return new PassInstance(pass);
    }

    //--------------------------------------------------------------------------

    PassInstance::PassInstance(PassPtr pass)
        : mPass(pass)
    {
        
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
                T3D_LOG_WARNING(LOG_TAG_RESOURCE, "Could not find shader variant when switch keyword !");
                break;
            }

            // 禁用当前 shader
            currentVariant = nullptr;

            // 使用对应 shader
            if (itr->second != nullptr)
            {
                currentVariant = ShaderVariantInstance::create(itr->second);
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


