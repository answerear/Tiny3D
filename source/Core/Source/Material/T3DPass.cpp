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


#include "Material/T3DPass.h"


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

        auto itr = std::find(mKeywords.begin(), mKeywords.end(), keyword);
        if (itr != mKeywords.end())
        {
            T3D_LOG_WARNING(LOG_TAG_RESOURCE, "Add shader variant failed ! Keyword duplicated !");
            return T3D_ERR_DUPLICATED_ITEM;
        }

        mKeywords.push_back(keyword);
        auto rval = mVariants.insert(ShaderVariantsValue(keyword, variant));

        if (mCurrentKeyword == nullptr)
        {
            mCurrentKeyword = &(rval.first->first);
            mCurrentVariant = variant;
        }

        return ret;
    }
    
    //--------------------------------------------------------------------------
}


