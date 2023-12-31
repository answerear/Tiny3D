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


#include "Material/T3DTechnique.h"
#include "Material/T3DPass.h"
#include "Render/T3DRenderState.h"


namespace Tiny3D
{
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

    TResult Technique::compile()
    {
        TResult ret = T3D_OK;

        do
        {
            for (auto pass : mPasses)
            {
                ret = pass->compile();
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    bool Technique::addTag(const String &key, const String &value)
    {
        auto rval = mTags.emplace(key, value);
        if (key == SHADER_TAG_QUEUE)
        {
            mRenderQueue = toRenderQueue(value);
        }
        return rval.second;
    }
    
    //--------------------------------------------------------------------------

    void Technique::removeTag(const String &key)
    {
        mTags.erase(key);
        if (key == SHADER_TAG_QUEUE)
        {
            mRenderQueue = -1;
        }
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

    bool Technique::setTag(const String &key, const String &value)
    {
        bool ret = false;
        const auto itr = mTags.find(key);
        if (itr != mTags.end())
        {
            itr->second = value;
            ret = true;
            if (key == SHADER_TAG_QUEUE)
            {
                mRenderQueue = toRenderQueue(value);
            }
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    bool Technique::addPass(PassPtr pass)
    {
        bool found = false;
        for (const auto p : mPasses)
        {
            if (p->getName() == pass->getName())
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            mPasses.emplace_back(pass);
        }

        return !found;
    }
    
    //--------------------------------------------------------------------------

    void Technique::removePass(const String &name)
    {
        for (auto itr = mPasses.begin(); itr != mPasses.end(); ++itr)
        {
            if ((*itr)->getName() == name)
            {
                mPasses.erase(itr);
                break;
            }
        }
    }
    
    //--------------------------------------------------------------------------

    bool Technique::getPass(const String &name, PassPtr &pass) const
    {
        bool ret = false;
        for (const auto p : mPasses)
        {
            if (p->getName() == name)
            {
                pass = p;
                ret = true;
                break;
            }
        }
        return ret;
    }
    
    //--------------------------------------------------------------------------

    uint32_t Technique::toRenderQueue(const String &tag)
    {
        uint32_t queue = -1;

        if (tag == "Background")
        {
            queue = 1000;
        }
        else if (tag == "Geometry")
        {
            queue = 2000;
        }
        else if (tag == "GeometryLast")
        {
            queue = 2449;
        }
        else if (tag == "AlphaTest")
        {
            queue = 2450;
        }
        else if (tag == "Transparent")
        {
            queue = 3000;
        }
        else if (tag == "Overlay")
        {
            queue = 4000;
        }
        else
        {
            queue = std::stoul(tag);
        }

        return queue;
    }

    //--------------------------------------------------------------------------
}


