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


#include "Resource/T3DSampler.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    T3D_IMPLEMENT_CLASS_1(Sampler, Resource);
    T3D_IMPLEMENT_CLASS_0(SamplerCreator);

    //--------------------------------------------------------------------------

    Sampler::Sampler(const String &name)
        : Resource(name)
        , mAddressMode()
        , mBorderColor(ColorRGBA::WHITE)
        , mMinFilter(FilterOptions::LINEAR)
        , mMagFilter(FilterOptions::LINEAR)
        , mMipFilter(FilterOptions::LINEAR)
        , mCompareFunc(CompareFunction::ALWAYS_FAIL)
        , mAnisotropy(1)
        , mMipmapBias(0.0f)
        , mIsDirty(true)
    {

    }

    //--------------------------------------------------------------------------

    Sampler::~Sampler()
    {

    }

    //--------------------------------------------------------------------------

    Resource::Type Sampler::getType() const
    {
        return Type::E_RT_SAMPLER;
    }

    //--------------------------------------------------------------------------

    TResult Sampler::cloneProperties(SamplerPtr newObj) const
    {
        TResult ret = T3D_OK;

        do
        {
            newObj->mAddressMode = mAddressMode;
            newObj->mBorderColor = mBorderColor;
            newObj->mMinFilter = mMinFilter;
            newObj->mMagFilter = mMagFilter;
            newObj->mMipFilter = mMipFilter;
            newObj->mCompareFunc = mCompareFunc;
            newObj->mAnisotropy = mAnisotropy;
            newObj->mMipmapBias = mMipmapBias;
            newObj->mIsDirty = true;
        } while (0);

        return ret;
    }
}

