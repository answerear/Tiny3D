/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    MaterialPtr Material::create(const String &name, MaterialType type)
    {
        MaterialPtr material = new Material(name);
        material->release();

        if (material->init() != T3D_OK)
        {
            material = nullptr;

        }
        return material;
    }

    //--------------------------------------------------------------------------

    Material::Material(const String &name)
        : Resource(name)
    {

    }

    //--------------------------------------------------------------------------

    Material::~Material()
    {

    }

    TResult Material::init()
    {
        TResult ret = T3D_OK;
        return ret;
    }

    //--------------------------------------------------------------------------

    Resource::Type Material::getType() const
    {
        return E_RT_MATERIAL;
    }

    //--------------------------------------------------------------------------

    TResult Material::load()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Material::unload()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    ResourcePtr Material::clone() const
    {
        return Material::create(mName, mMaterialType);
    }
}
