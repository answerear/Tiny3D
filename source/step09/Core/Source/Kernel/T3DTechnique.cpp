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


#include "Kernel/T3DTechnique.h"
#include "Kernel/T3DPass.h"
#include "T3DErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    TechniquePtr Technique::create(const String &name, Material *material)
    {
        TechniquePtr tech = new Technique(name, material);
        tech->release();
        return tech;
    }

    //--------------------------------------------------------------------------

    Technique::Technique(const String &name, Material *material)
        : mParent(material)
        , mName(name)
        , mLodIndex(0)
        , mSchemeIndex(0)
    {

    }

    //--------------------------------------------------------------------------

    Technique::~Technique()
    {

    }

    //--------------------------------------------------------------------------

    TResult Technique::addPass(const String &name, PassPtr &pass)
    {
        TResult ret = T3D_OK;

        do 
        {
            pass = Pass::create(name, this);
            if (pass == nullptr)
            {
                ret = T3D_ERR_RES_CREATE_PASS;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Create pass [%s] object failed !", name.c_str());
                break;
            }

            mPasses.push_back(pass);
        } while (0);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Technique::removePass(const String &name)
    {
        TResult ret = T3D_ERR_NOT_FOUND;

        auto itr = mPasses.begin();
        while (itr != mPasses.end())
        {
            PassPtr pass = *itr;
            if (name == pass->getName())
            {
                mPasses.erase(itr);
                ret = T3D_OK;
                break;
            }

            ++itr;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Technique::removePass(size_t index)
    {
        TResult ret = T3D_ERR_NOT_FOUND;

        size_t i = 0;
        auto itr = mPasses.begin();

        while (itr != mPasses.end())
        {
            if (i == index)
            {
                mPasses.erase(itr);
                ret = T3D_OK;
                break;
            }

            ++i;
            ++itr;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    PassPtr Technique::getPass(const String &name) const
    {
        PassPtr pass;

        auto itr = mPasses.begin();
        while (itr != mPasses.end())
        {
            const PassPtr &p = *itr;
            if (name == pass->getName())
            {
                pass = p;
                break;
            }

            ++itr;
        }

        return pass;
    }

    //--------------------------------------------------------------------------

    PassPtr Technique::getPass(size_t index) const
    {
        PassPtr pass;

        size_t i = 0;
        auto itr = mPasses.begin();

        while (itr != mPasses.end())
        {
            if (i == index)
            {
                pass = *itr;
                break;
            }

            ++i;
            ++itr;
        }

        return pass;
    }
}
