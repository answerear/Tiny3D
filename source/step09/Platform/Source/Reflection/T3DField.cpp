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


#include "Reflection/T3DField.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Field::Field(uint32_t offset, const Class *cls, const Class *owner, 
        AccessType access, const char *type, const char *name)
        : MemberBase(owner, access, type, name)
        , mClass(cls)
        , mOffset(offset)
    {

    }

    //--------------------------------------------------------------------------

    StaticField::StaticField(void *address, const Class *cls, const Class *owner,
        AccessType access, const char *type, const char *name)
        : MemberBase(owner, access, type, name)
        , mClass(cls)
        , mAddress(address)
    {

    }
}
