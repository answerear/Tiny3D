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

#include "Resource/T3DMaterial.h"
#include "Resource/T3DShader.h"


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
        pass->mShader = mShader;
        return pass;
    }
    
    //--------------------------------------------------------------------------
    
    void Pass::addTag(const String &key, const String &value)
    {
        mTags.emplace(key, value);
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

    TechniquePtr Technique::create(const String &name)
    {
        return new Technique(name);
    }

    //--------------------------------------------------------------------------

    Technique::Technique(const String &name)
        : mName(name)
    {
        
    }

    //--------------------------------------------------------------------------

    TechniquePtr Technique::clone() const
    {
        TechniquePtr tech = create(getName());
        tech->mLOD = mLOD;
        tech->mTags = mTags;
        for (auto pass : mPasses)
        {
            tech->mPasses.emplace_back(pass->clone());
        }
        return tech;
    }

    //--------------------------------------------------------------------------

    void Technique::addTag(const String &key, const String &value)
    {
        mTags.emplace(key, value);
    }
    
    //--------------------------------------------------------------------------

    void Technique::removeTag(const String &key)
    {
        mTags.erase(key);
    }
    
    //--------------------------------------------------------------------------

    bool Technique::getTag(const String &key, String &value) const
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

    void Technique::addPass(PassPtr pass)
    {
        mPasses.emplace_back(pass);
    }
    
    //--------------------------------------------------------------------------

    void Technique::removePass(PassPtr pass)
    {
        for (auto itr = mPasses.begin(); itr != mPasses.end(); ++itr)
        {
            if ((*itr)->getName() == pass->getName())
            {
                mPasses.erase(itr);
                break;
            }
        }
    }
    
    //--------------------------------------------------------------------------

    bool Technique::getPass(const String &name) const
    {
        bool ret = false;
        for (auto itr = mPasses.begin(); itr != mPasses.end(); ++itr)
        {
            if ((*itr)->getName() == name)
            {
                ret = true;
                break;
            }
        }
        return ret;
    }
    
    //--------------------------------------------------------------------------

    MaterialPtr Material::create(const String &name)
    {
        return new Material(name);
    }

    //--------------------------------------------------------------------------

    Material::Material(const String &name)
        : Resource(name)
    {
        
    }

    //--------------------------------------------------------------------------

    Resource::Type Material::getType() const
    {
        return Type::kMaterial;
    }

    //--------------------------------------------------------------------------
    
    ResourcePtr Material::clone() const
    {
        MaterialPtr material = create(getName());
        material->cloneProperties(this);
        return material;
    }
    
    //--------------------------------------------------------------------------

    void Material::cloneProperties(const Resource *const src)
    {
        Resource::cloneProperties(src);
        const Material *material = static_cast<const Material*>(src);
        mTechniques.clear();
        for (auto tech : material->mTechniques)
        {
            mTechniques.emplace_back(tech);
        }
    }

    //--------------------------------------------------------------------------
}
