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


#include "Material/T3DPass.h"

#include <d3d11.h>

#include "Material/T3DShaderVariant.h"
#include "Render/T3DRenderState.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    PassPtr Pass::create(const String &name)
    {
        return new Pass(name);
    }

    //--------------------------------------------------------------------------

    Pass::Pass(const String &name)
        : mName(name)
    {
        
    }

    //--------------------------------------------------------------------------

    PassPtr Pass::clone() const
    {
        PassPtr pass = create(getName());
        pass->mShaderName = mShaderName;
        pass->mTags = mTags;
        pass->mRenderState = mRenderState;
        return pass;
    }
    
    //--------------------------------------------------------------------------

    TResult Pass::compile()
    {
        TResult ret = T3D_OK;

        auto compileShader = [this](const ShaderVariants &shaders)
        {
            TResult ret = T3D_OK;
            
            for (auto shader : shaders)
            {
                ret = shader.second->compile();
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }

            return ret;
        };

        do
        {
            T3D_LOG_DEBUG(LOG_TAG_RESOURCE, "Start compiling pass [%s] ...", getName().c_str());
            
            // 编译 vertex shader
            ret = compileShader(mVertexShaders);
            if (T3D_FAILED(ret))
            {
                break;
            }

            T3D_LOG_DEBUG(LOG_TAG_RESOURCE, "Completed compling vertex shader !");

            // 编译 hull shader
            ret = compileShader(mHullShaders);
            if (T3D_FAILED(ret))
            {
                break;
            }

            T3D_LOG_DEBUG(LOG_TAG_RESOURCE, "Completed compling hull shader !");
            
            // 编译 domain shader
            ret = compileShader(mDomainShaders);
            if (T3D_FAILED(ret))
            {
                break;
            }

            T3D_LOG_DEBUG(LOG_TAG_RESOURCE, "Completed compling domain shader !");
            
            // 编译 gemometry shader
            ret = compileShader(mGeometryShaders);
            if (T3D_FAILED(ret))
            {
                break;
            }

            T3D_LOG_DEBUG(LOG_TAG_RESOURCE, "Completed compling geometry shader !");
            
            // 编译 pixel shader
            ret = compileShader(mPixelShaders);
            if (T3D_FAILED(ret))
            {
                break;
            }

            T3D_LOG_DEBUG(LOG_TAG_RESOURCE, "Completed compling pixel shader !");
            
            T3D_LOG_DEBUG(LOG_TAG_RESOURCE, "Completed compiling technique !");
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------
    
    bool Pass::addTag(const String &key, const String &value)
    {
        auto rval = mTags.emplace(key, value);
        return rval.second;
    }
    
    //--------------------------------------------------------------------------

    void Pass::removeTag(const String &key)
    {
        mTags.erase(key);
    }
    
    //--------------------------------------------------------------------------

    bool Pass::getTag(const String &key, String &value) const
    {
        bool ret = false;
        const auto itr = mTags.find(key);
        if (itr != mTags.end())
        {
            value = itr->second;
            ret = true;
        }
        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult Pass::addShaderVariant(const ShaderKeyword &keyword, ShaderVariantPtr variant)
    {
        TResult ret = T3D_OK;

        const auto itr = std::find(mKeywords.begin(), mKeywords.end(), keyword);
        if (itr == mKeywords.end())
        {
            mKeywords.push_back(keyword);
        }
        else
        {
            T3D_LOG_WARNING(LOG_TAG_RESOURCE, "Add shader variant failed ! Keyword duplicated !");
        }

        ShaderVariants *vars = nullptr;

        switch (variant->getShaderStage())
        {
        case SHADER_STAGE::kVertex:
            {
                vars = &mVertexShaders;
            }
            break;
        case SHADER_STAGE::kPixel:
            {
                vars = &mPixelShaders;
            }
            break;
        case SHADER_STAGE::kCompute:
            {
                return T3D_ERR_NOT_IMPLEMENT;
            }
            break;
        case SHADER_STAGE::kGeometry:
            {
                vars = &mGeometryShaders;
            }
            break;
        case SHADER_STAGE::kHull:
            {
                vars = &mHullShaders;
            }
            break;
        case SHADER_STAGE::kDomain:
            {
                vars = &mDomainShaders;
            }
            break;
        default:
            break;
        }

        auto rval = vars->insert(ShaderVariantsValue(keyword, variant));
        if (rval.second)
        {
            variant->setPass(this);
        }
        
        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult Pass::removeShaderVariant(const ShaderKeyword &keyword)
    {
        TResult ret = T3D_OK;

        const auto itr = std::find(mKeywords.begin(), mKeywords.end(), keyword);
        if (itr == mKeywords.end())
        {
            T3D_LOG_WARNING(LOG_TAG_RESOURCE, "Remove shader variant failed ! Keyword did not exist !");
            return T3D_ERR_NOT_FOUND;
        }

        mKeywords.erase(itr);

        auto it = mVertexShaders.find(keyword);
        if (it != mVertexShaders.end())
        {
            mVertexShaders.erase(it);
            it->second->setPass(nullptr);
        }
        
        it = mPixelShaders.find(keyword);
        if (it != mPixelShaders.end())
        {
            mPixelShaders.erase(it);
            it->second->setPass(nullptr);
        }
        
        it = mGeometryShaders.find(keyword);
        if (it != mGeometryShaders.end())
        {
            mGeometryShaders.erase(it);
            it->second->setPass(nullptr);
        }
        
        it = mHullShaders.find(keyword);
        if (it != mHullShaders.end())
        {
            mHullShaders.erase(it);
            it->second->setPass(nullptr);
        }
        
        it = mDomainShaders.find(keyword);
        if (it != mDomainShaders.end())
        {
            mDomainShaders.erase(it);
            it->second->setPass(nullptr);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void Pass::onPostLoad()
    {
        auto setPass = [this](ShaderVariants &shaders)
        {
            for (auto shader : shaders)
            {
                shader.second->setPass(this);
            }
        };
        
        setPass(mVertexShaders);
        setPass(mPixelShaders);
        setPass(mGeometryShaders);
        setPass(mHullShaders);
        setPass(mDomainShaders);
    }

    //--------------------------------------------------------------------------
}


