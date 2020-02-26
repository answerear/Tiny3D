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


#include "Resource/T3DModel.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    T3D_IMPLEMENT_CLASS_1(Model, Resource);

    //--------------------------------------------------------------------------

    ModelPtr Model::create(const String &name)
    {
        ModelPtr mesh = new Model(name);
        mesh->release();
        return mesh;
    }

    //--------------------------------------------------------------------------

    Model::Model(const String &name)
        : Resource(name)
    {

    }

    //--------------------------------------------------------------------------

    Model::~Model()
    {

    }

    //--------------------------------------------------------------------------

    Resource::Type Model::getType() const
    {
        return Type::E_RT_MESHDATA;
    }

    //--------------------------------------------------------------------------

    TResult Model::load()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Model::unload()
    {
        return Resource::unload();
    }

    //--------------------------------------------------------------------------

    ResourcePtr Model::clone() const
    {
        ModelPtr mesh = Model::create(mName);
        return mesh;
    }

}
