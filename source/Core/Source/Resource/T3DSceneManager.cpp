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


#include "Resource/T3DSceneManager.h"
#include "Resource/T3DScene.h"
#include "Serializer/T3DSerializerManager.h"


namespace  Tiny3D
{
    //--------------------------------------------------------------------------

    TResult SceneManagerImpl::unloadScene(ScenePtr &scene)
    {
        return unload(scene);
    }

    //--------------------------------------------------------------------------

    SceneManagerPtr SceneManager::create()
    {
        return new SceneManager();
    }

    //--------------------------------------------------------------------------

    ScenePtr SceneManager::createScene(const String &name)
    {
        if (mImpl != nullptr)
        {
            return mImpl->createScene(name);
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    ScenePtr SceneManager::loadScene(Archive *archive, const String &name)
    {
        if (mImpl != nullptr)
        {
            return mImpl->loadScene(archive, name);
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult SceneManager::saveScene(Scene *scene, Archive *archive)
    {
        if (mImpl != nullptr)
        {
            return mImpl->saveScene(scene, archive);
        }

        return T3D_ERR_NOT_IMPLEMENT;
    }

    //--------------------------------------------------------------------------

    TResult SceneManager::unloadScene()
    {
        if (mImpl != nullptr)
        {
            return mImpl->unloadScene(mCurrentScene);
        }

        return T3D_ERR_NOT_IMPLEMENT;
    }

    //--------------------------------------------------------------------------

    BuiltinSceneManagerPtr BuiltinSceneManager::create()
    {
        return new BuiltinSceneManager();
    }

    //--------------------------------------------------------------------------

    ScenePtr BuiltinSceneManager::createScene(const String &name)
    {
        return smart_pointer_cast<Scene>(createResource(name, 0));
    }

    //--------------------------------------------------------------------------

    ScenePtr BuiltinSceneManager::loadScene(Archive *archive, const String &name)
    {
        return smart_pointer_cast<Scene>(load(archive, name, 0));
    }

    //--------------------------------------------------------------------------

    TResult BuiltinSceneManager::saveScene(Scene *scene, Archive *archive)
    {
        return save(scene, archive);
    }

    //--------------------------------------------------------------------------

    ResourcePtr BuiltinSceneManager::newResource(const String &name, int32_t argc, va_list args)
    {
        return Scene::create(name);
    }

    //--------------------------------------------------------------------------

    ResourcePtr BuiltinSceneManager::loadResource(const String &name, DataStream &stream, int32_t argc, va_list args)
    {
        return T3D_SERIALIZER_MGR.deserialize<Scene>(stream);
    }

    //--------------------------------------------------------------------------

    TResult BuiltinSceneManager::saveResource(DataStream &stream, Resource *res)
    {
        T3D_ASSERT(res->getType() == Resource::Type::kScene, "Save resource must be scene !");
        Scene *scene = static_cast<Scene*>(res);
        return T3D_SERIALIZER_MGR.serialize(stream, scene);
    }
    
    //--------------------------------------------------------------------------
}
