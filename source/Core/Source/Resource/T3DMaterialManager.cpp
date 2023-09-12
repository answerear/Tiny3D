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


#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DMaterial.h"
#include "Serializer/T3DSerializerManager.h"


namespace Tiny3D
{   
    //--------------------------------------------------------------------------

    MaterialManagerPtr MaterialManager::create()
    {
        return new MaterialManager();
    }
    
    //--------------------------------------------------------------------------

    MaterialPtr MaterialManager::loadMaterial(Archive *archive, const String &name)
    {
        return smart_pointer_cast<Material>(load(archive, name, 0));
    }

    //--------------------------------------------------------------------------

    TResult MaterialManager::saveMaterial(Archive *archive, Material *material)
    {
        return save(material, archive);
    }

    //--------------------------------------------------------------------------

    ResourcePtr MaterialManager::newResource(const String &name, int32_t argc, va_list args)
    {
        return Material::create(name);
    }

    //--------------------------------------------------------------------------

    ResourcePtr MaterialManager::loadResource(const String &name, DataStream &stream, int32_t argc, va_list args)
    {
        return T3D_SERIALIZER_MGR.deserialize<Material>(stream);
    }

    //--------------------------------------------------------------------------

    TResult MaterialManager::saveResource(DataStream &stream, Resource *res)
    {
        Material *material = static_cast<Material*>(res);
        T3D_SERIALIZER_MGR.serialize(stream, material);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}

