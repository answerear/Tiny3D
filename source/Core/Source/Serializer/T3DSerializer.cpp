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


#include "Serializer/T3DSerializer.h"


namespace Tiny3D
{
    FileObject::FileObject()
        : magic(T3D_FILE_MAGIC)
        , version(T3D_FILE_CURRENT_VERSION)
        , object(nullptr)
    {

    }
}


//-----------------------------------------------------------------------
// -------
//                              FileObject RTTR
//------------------------------------------------------------------------------

RTTR_REGISTRATION
{
    using namespace rttr;

    registration::class_<Tiny3D::FileObject>("Tiny3D::FileObject")
        .constructor<>()
        .property("Magic", &Tiny3D::FileObject::magic)
        .property("Version", &Tiny3D::FileObject::version)
        .property("Object", &Tiny3D::FileObject::object);
}
