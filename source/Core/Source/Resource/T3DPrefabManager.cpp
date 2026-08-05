/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#include "Resource/T3DPrefabManager.h"
#include "Resource/T3DPrefab.h"
#include "Kernel/T3DAgent.h"
#include "Serializer/T3DSerializerManager.h"


namespace Tiny3D
{   
    //--------------------------------------------------------------------------

    PrefabManagerPtr PrefabManager::create()
    {
        return T3D_NEW PrefabManager();
    }
    
    //--------------------------------------------------------------------------

    PrefabPtr PrefabManager::createPrefab(const String &name, GameObjectPtr root)
    {
        PrefabPtr prefab = smart_pointer_cast<Prefab>(createResource(name, 0));
        if (prefab != nullptr && root != nullptr)
        {
            prefab->setRootGameObject(root);
        }
        return prefab;
    }

    //--------------------------------------------------------------------------

    PrefabPtr PrefabManager::loadPrefab(Archive *archive, const String &name)
    {
        return smart_pointer_cast<Prefab>(load(archive, name));
    }

    //--------------------------------------------------------------------------

    PrefabPtr PrefabManager::loadPrefab(Archive *archive, const UUID &uuid)
    {
        return smart_pointer_cast<Prefab>(load(archive, uuid));
    }

    //--------------------------------------------------------------------------

    TResult PrefabManager::savePrefab(Archive *archive, Prefab *prefab)
    {
        TResult ret = save(archive, prefab);
        if (T3D_SUCCEEDED(ret))
        {
            ensureCached(prefab);
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult PrefabManager::savePrefab(Archive *archive, const String &filename, Prefab *prefab)
    {
        TResult ret = save(archive, filename, prefab);
        if (T3D_SUCCEEDED(ret))
        {
            ensureCached(prefab);
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    void PrefabManager::ensureCached(Prefab *prefab)
    {
        if (prefab == nullptr || lookup(prefab->getUUID()) != nullptr)
        {
            return;
        }

        insertCache(prefab->getUUID(), ResourcePtr(prefab));
    }

    //--------------------------------------------------------------------------

    ResourcePtr PrefabManager::newResource(const String &name, int32_t argc, va_list args)
    {
        return Prefab::create(name);
    }

    //--------------------------------------------------------------------------

    ResourcePtr PrefabManager::loadResource(const String &name, DataStream &stream)
    {
        return loadResource(stream);
    }

    //--------------------------------------------------------------------------

    ResourcePtr PrefabManager::loadResource(DataStream &stream)
    {
        return T3D_SERIALIZER_MGR.deserialize<Prefab>(stream);
    }
    
    //--------------------------------------------------------------------------

    TResult PrefabManager::saveResource(DataStream &stream, Resource *res)
    {
        Prefab *prefab = static_cast<Prefab*>(res);
        return T3D_SERIALIZER_MGR.serialize(stream, prefab);
    }

    //--------------------------------------------------------------------------

    T3D_INSTANTIATE_SINGLETON(PrefabManager)
}

