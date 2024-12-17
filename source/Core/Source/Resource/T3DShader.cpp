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


#include "Resource/T3DShader.h"
#include "Material/T3DTechnique.h"
#include "Material/T3DShaderConstantParam.h"
#include "Material/T3DShaderSamplerParam.h"
#include "Resource/T3DTextureManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ShaderPtr Shader::create(const String &name)
    {
        return new Shader(name);
    }

    //--------------------------------------------------------------------------

    Shader::Shader(const String &name)
        : Resource(name)
    {
        
    }

    //--------------------------------------------------------------------------

    Shader::~Shader()
    {
        
    }
    
    //--------------------------------------------------------------------------

    Resource::Type Shader::getType() const
    {
        return Type::kShader;
    }

    //--------------------------------------------------------------------------

    TResult Shader::compile()
    {
        TResult ret = T3D_OK;

        do
        {
            T3D_LOG_DEBUG(LOG_TAG_RESOURCE, "Start compiling shader [%s] ...", getName().c_str());
            
            for (auto tech : mTechniques)
            {
                ret = tech->compile();
                if (T3D_FAILED(ret))
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Failed to compile shader [%s] ! ERROR [%d]", getName().c_str(), ret);
                    break;
                }

                if (mSupportTechnique == nullptr)
                {
                    // 先设置第一个 technique 为支持的 technique ，临时使用
                    mSupportTechnique = tech;
                }
            }

            T3D_LOG_DEBUG(LOG_TAG_RESOURCE, "Completed compiling shader !");
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Shader::reflect()
    {
        TResult ret = T3D_OK;

        do
        {
            for (auto tech : mTechniques)
            {
                ret = tech->reflect();
                if (T3D_FAILED(ret))
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Failed to reflect shader technique [%s] ! ERROR [%d]", tech->getName().c_str(), ret);
                    break;
                }
            }
        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------

    bool Shader::isKeywordEnable(const String &keyword) const
    {
        return false;
    }

    //--------------------------------------------------------------------------

    ResourcePtr Shader::clone() const
    {
        return create(getName());
    }

    //--------------------------------------------------------------------------

    TResult Shader::onLoad(Archive *archive)
    {
        TResult ret;

        do
        {
            ret = Resource::onLoad(archive);
            if (T3D_FAILED(ret))
            {
                break;
            }

            ret = compile();
            if (T3D_FAILED(ret))
            {
                break;
            }

            // for (auto sampler : mSamplers)
            // {
            //     Texture *tex = T3D_TEXTURE_MGR.loadTexture(archive, sampler.second->getTexName());
            //     sampler.second->setTexture(tex);
            // }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void Shader::onPostLoad()
    {
        for (auto tech : mTechniques)
        {
            tech->setShader(this);
        }
    }

    //--------------------------------------------------------------------------

    TResult Shader::addConstantValue(ShaderConstantValuePtr value)
    {
        TResult ret = T3D_OK;

        auto rval = mConstantValues.emplace(value->getName(), value);
        if (!rval.second)
        {
            ret = T3D_ERR_DUPLICATED_ITEM;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Shader::addSamplerParam(ShaderSamplerParamPtr param)
    {
        TResult ret = T3D_OK;

        auto rval = mSamplers.emplace(param->getName(), param);
        if (!rval.second)
        {
            ret = T3D_ERR_DUPLICATED_ITEM;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    bool Shader::addTechnique(TechniquePtr tech)
    {
        bool found = false;
        for (const auto &t : mTechniques)
        {
            if (t->getName() == tech->getName())
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            if (mTechniques.empty())
            {
                mSupportTechnique = tech;
            }
            
            mTechniques.push_back(tech);
            tech->setShader(this);
        }

        return !found;
    }

    //--------------------------------------------------------------------------

    void Shader::removeTechnique(const String &name)
    {
        for (auto itr = mTechniques.begin(); itr != mTechniques.end(); ++itr)
        {
            if ((*itr)->getName() == name)
            {
                mTechniques.erase(itr);
                (*itr)->setShader(nullptr);
                break;
            }
        }
    }

    //--------------------------------------------------------------------------

    bool Shader::getTechnique(const String &name, TechniquePtr &tech) const
    {
        bool found = false;
        
        for (const auto t : mTechniques)
        {
            if (t->getName() == name)
            {
                tech = t;
                found = true;
                break;
            }
        }

        return found;
    }

    //--------------------------------------------------------------------------
}


