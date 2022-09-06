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


#include "Resource/T3DResource.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------
    MetaPtr Meta::create()
    {
        MetaPtr meta = new Meta();
        meta->release();
        return meta;
    }

    //--------------------------------------------------------------------------

    Meta::Meta()
        : uuid()
        , type(FileType::kNone)
    {

    }

    //--------------------------------------------------------------------------

    Meta::~Meta()
    {

    }

    //--------------------------------------------------------------------------

    MetaPtr Meta::clone() const
    {
        MetaPtr meta = Meta::create();
        cloneProperties(meta);
        return meta;
    }

    //--------------------------------------------------------------------------

    void Meta::cloneProperties(Meta* meta) const
    {
        meta->uuid = uuid;
        meta->type = type;
    }

    //--------------------------------------------------------------------------

    Resource::Resource(const String &strName)
        : mResReferCount(1)
        , mID()
        , mCloneID()
        , mSize(0)
        , mIsLoaded(false)
        , mName(strName)
    {

    }

    //--------------------------------------------------------------------------

    Resource::~Resource()
    {
        if (mIsLoaded)
        {
            unload();
        }
    }

    //--------------------------------------------------------------------------

    TResult Resource::unload()
    {
        mResReferCount = 0;
        mIsLoaded = false;
        return T3D_OK;
    }
}

//------------------------------------------------------------------------------
//                              Meta RTTR
//------------------------------------------------------------------------------

RTTR_REGISTRATION
{
    using namespace rttr;

    registration::class_<Tiny3D::Meta>("Tiny3D::Meta")
        .enumeration<Tiny3D::Meta::FileType>("Tiny3D::Meta::FileType")
        (
            value("None", Tiny3D::Meta::FileType::kNone),
            value("File", Tiny3D::Meta::FileType::kFile),
            value("Dir", Tiny3D::Meta::FileType::kDir)
        )
        .constructor()(policy::ctor::as_raw_ptr)
        .property("UUID", &Tiny3D::Meta::uuid)
        (
            metadata(TXT_DESCRIPTION, "Universal Unique Identifier")
        )
        .property("Type", &Tiny3D::Meta::type)
        (
            metadata(TXT_DESCRIPTION, "File type corresponding to this meta.")
        );
}

