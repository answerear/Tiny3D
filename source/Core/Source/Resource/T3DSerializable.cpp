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

#include "Resource/T3DSerializable.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SerializablePtr Serializable::create(const String &name)
    {
        return new Serializable(name);
    }

    //--------------------------------------------------------------------------

    Serializable::~Serializable()
    {
        T3D_SAFE_DELETE(mObject);
    }

    //--------------------------------------------------------------------------

    Serializable::Serializable(const String &name)
        : Resource(name)
    {
        
    }

    //--------------------------------------------------------------------------

    Resource::Type Serializable::getType() const
    {
        return Type::kSerializable;
    }

    //--------------------------------------------------------------------------

    TResult Serializable::loadData(DataStream &stream)
    {
        mObject = new RTTRObject(T3D_SERIALIZER_MGR.deserializeObject(stream));
        if (!mObject->is_valid())
        {
            return T3D_ERR_FAIL;
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    ResourcePtr Serializable::clone() const
    {
        SerializablePtr serializable = create(getName());
        serializable->mMeta = mMeta->clone();
        serializable->mObject = new RTTRObject(*mObject);
        return serializable;
    }
    
    //--------------------------------------------------------------------------
}
