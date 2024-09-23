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


#include "Resource/T3DSceneManager.h"
#include "Resource/T3DScene.h"
#include "Serializer/T3DSerializerManager.h"


namespace  Tiny3D
{
    //--------------------------------------------------------------------------

    TResult SceneManagerImpl::unloadScene(ScenePtr &scene)
    {
#if defined(T3D_EDITOR)
        return unload(scene->getRuntimeScene());
#else
        return unload(scene);
#endif
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

#if defined(T3D_EDITOR)
    ScenePtr SceneManager::createEditorScene(const String &name, const EditorSceneCreator &creator)
    {
        if (mImpl != nullptr)
        {
            return mImpl->createEditorScene(name, creator);
        }

        return nullptr;
    }

#endif
    
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

    TResult SceneManager::saveScene(Archive *archive, Scene *scene)
    {
        if (mImpl != nullptr)
        {
            return mImpl->saveScene(archive, scene);
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

    TResult SceneManager::unloadAllScenes()
    {
        if (mImpl != nullptr)
        {
            return mImpl->unloadAllResources();
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

#if defined(T3D_EDITOR)
    ScenePtr BuiltinSceneManager::createEditorScene(const String &name, const EditorSceneCreator &creator)
    {
        return smart_pointer_cast<Scene>(createResource(name, 1, &creator));
    }

#endif
    
    //--------------------------------------------------------------------------

    ScenePtr BuiltinSceneManager::loadScene(Archive *archive, const String &name)
    {
        return smart_pointer_cast<Scene>(load(archive, name));
    }

    //--------------------------------------------------------------------------

    TResult BuiltinSceneManager::saveScene(Archive *archive, Scene *scene)
    {
        return save(archive, scene);
    }

    //--------------------------------------------------------------------------

    ResourcePtr BuiltinSceneManager::newResource(const String &name, int32_t argc, va_list args)
    {
#if defined(T3D_EDITOR)
        T3D_ASSERT(argc <= 1);
        ScenePtr scene;
        if (argc == 1)
        {
            EditorSceneCreator *creator = va_arg(args, EditorSceneCreator *);
            if (creator == nullptr)
            {
                scene = nullptr;
            }
            else
            {
                scene = (*creator)(name);
            }
        }
        else
        {
            scene = Scene::create(name);
        }
        return scene;
#else
        T3D_ASSERT(argc == 0);
        return Scene::create(name);
#endif
    }

    //--------------------------------------------------------------------------

    ResourcePtr BuiltinSceneManager::loadResource(const String &name, DataStream &stream)
    {
        return loadResource(stream);
    }

    //--------------------------------------------------------------------------

    ResourcePtr BuiltinSceneManager::loadResource(DataStream &stream)
    {
        return T3D_SERIALIZER_MGR.deserialize<Scene>(stream);
    }

    //--------------------------------------------------------------------------

    TResult BuiltinSceneManager::saveResource(DataStream &stream, Resource *res)
    {
        T3D_ASSERT(res->getType() == Resource::Type::kScene);
        Scene *scene = static_cast<Scene*>(res);
        return T3D_SERIALIZER_MGR.serialize(stream, scene);
    }
    
    //--------------------------------------------------------------------------
}
