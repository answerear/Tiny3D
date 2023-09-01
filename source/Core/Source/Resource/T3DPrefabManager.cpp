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


#include "Resource/T3DPrefabManager.h"
#include "Resource/T3DPrefab.h"
#include "Kernel/T3DAgent.h"


namespace Tiny3D
{   
    //--------------------------------------------------------------------------

    PrefabManagerPtr PrefabManager::create()
    {
        return new PrefabManager();
    }
    
    //--------------------------------------------------------------------------

    PrefabPtr PrefabManager::loadPrefab(Archive *archive, const String &name)
    {
        return smart_pointer_cast<Prefab>(load(archive, name, 0));
    }

    //--------------------------------------------------------------------------

    TResult PrefabManager::savePrefab(Archive *archive, Prefab *prefab)
    {
        return save(prefab, archive);
    }

    //--------------------------------------------------------------------------

    ResourcePtr PrefabManager::newResource(const String &name, int32_t argc, va_list args)
    {
        return Prefab::create(name);
    }

    //--------------------------------------------------------------------------

    ResourcePtr PrefabManager::loadResource(const String &name, DataStream &stream, int32_t argc, va_list args)
    {
        return T3D_SERIALIZER_MGR.deserialize<Prefab>(stream);
    }

    //--------------------------------------------------------------------------

    TResult PrefabManager::saveResource(DataStream &stream, Resource *res)
    {
        Prefab *prefab = static_cast<Prefab*>(res);
        T3D_SERIALIZER_MGR.serialize(stream, prefab);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}

