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


#include "Resource/T3DShader.h"
#include "Material/T3DTechnique.h"
#include "Material/T3DShaderConstantParam.h"
#include "Material/T3DShaderSamplerParam.h"


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
            for (auto tech : mTechniques)
            {
                ret = tech->compile();
                if (mSupportTechnique == nullptr && T3D_SUCCEEDED(ret))
                {
                    mSupportTechnique = tech;
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

    bool Shader::onPostLoad()
    {
        compile();
        return true;
    }

    //--------------------------------------------------------------------------

    TResult Shader::addConstantParam(ShaderConstantParamPtr param)
    {
        TResult ret = T3D_OK;

        auto rval = mConstants.emplace(param->getName(), param);
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
        for (const auto t : mTechniques)
        {
            if (t->getName() == tech->getName())
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            mTechniques.push_back(tech);
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


