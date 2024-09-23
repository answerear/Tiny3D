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


#include "Resource/T3DScene.h"
#include "Kernel/T3DGameObject.h"
#include "Component/T3DCamera.h"
#include "Component/T3DTransform3D.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ScenePtr Scene::create(const String &name)
    {
        return new Scene(name);
    }

    //--------------------------------------------------------------------------

    Scene::Scene(const String &name)
        : Resource(name)
    {
        mRootGameObject = GameObject::create("SceneRoot");
        mRootTransform = mRootGameObject->addComponent<Transform3D>();
    }

    //--------------------------------------------------------------------------

    Resource::Type Scene::getType() const
    {
        return Type::kScene;
    }

    //--------------------------------------------------------------------------

    ResourcePtr Scene::clone() const
    {
        return create(getName());
    }

    //--------------------------------------------------------------------------

    void Scene::cloneProperties(const Resource *const src)
    {
        
    }

    //--------------------------------------------------------------------------

    const Transform3DPtr &Scene::getRootTransform() const
    {
        return mRootTransform;
    }

    //--------------------------------------------------------------------------

    // void Scene::addRootGameObject(GameObject *gameObject)
    // {
    //     mRootGameObjects.emplace_back(gameObject);
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // GameObjectPtr Scene::addRootGameObject(const String &name)
    // {
    //     return GameObject::create(name);
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // void Scene::removeRootGameObject(const String &name)
    // {
    //     for (auto itr = mRootGameObjects.begin(); itr != mRootGameObjects.end(); ++itr)
    //     {
    //         if ((*itr)->getName() == name)
    //         {
    //             mRootGameObjects.erase(itr);
    //             break;
    //         }
    //     }
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // void Scene::removeRootGameObject(GameObject *gameObject)
    // {
    //     for (auto itr = mRootGameObjects.begin(); itr != mRootGameObjects.end(); ++itr)
    //     {
    //         if ((*itr) == gameObject)
    //         {
    //             mRootGameObjects.erase(itr);
    //             break;
    //         }
    //     }
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // void Scene::removeRootGameObject(const UUID &uuid)
    // {
    //     for (auto itr = mRootGameObjects.begin(); itr != mRootGameObjects.end(); ++itr)
    //     {
    //         if ((*itr)->getUUID() == uuid)
    //         {
    //             mRootGameObjects.erase(itr);
    //             break;
    //         }
    //     }
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // void Scene::removeAll()
    // {
    //     for (auto itr = mRootGameObjects.rbegin(); itr != mRootGameObjects.rend(); ++itr)
    //     {
    //         GameObject::destroy(*itr);
    //     }
    //     
    //     mRootGameObjects.clear();
    //
    //     mCameras.clear();
    // }

    //--------------------------------------------------------------------------

    void Scene::update()
    {
        // for (auto go : mRootGameObjects)
        // {
        //     go->update();
        // }
        mRootGameObject->update();
    }

    //--------------------------------------------------------------------------

    TResult Scene::addCamera(Camera *camera)
    {
        TResult ret = T3D_OK;

        do
        {
            for (auto itr = mCameras.begin(); itr != mCameras.end(); ++itr)
            {
                if (camera->getUUID() == itr->second->getUUID())
                {
                    ret = T3D_ERR_DUPLICATED_ITEM;
                    break;
                }
            }

            if (T3D_FAILED(ret))
            {
                break;
            }

            mCameras.emplace(camera->getOrder(), camera);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Scene::removeCamera(Camera *camera)
    {
        TResult ret = T3D_OK;

        do
        {
            auto n = mCameras.erase(camera->getOrder());
            if (n == 0)
            {
                ret = T3D_ERR_NOT_FOUND;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Scene::removeCamera(const UUID &uuid)
    {
        for (auto itr = mCameras.begin(); itr != mCameras.end(); ++itr)
        {
            if (itr->second->getUUID() == uuid)
            {
                mCameras.erase(itr);
                return T3D_OK;
            }
        }
        
        return T3D_ERR_NOT_FOUND;
    }

    //--------------------------------------------------------------------------

    TResult Scene::removeCamera(const String &name)
    {
        for (auto itr = mCameras.begin(); itr != mCameras.end(); ++itr)
        {
            if (itr->second->getGameObject()->getName() == name)
            {
                mCameras.erase(itr);
                return T3D_OK;
            }
        }
        
        return T3D_ERR_NOT_FOUND;
    }

    //--------------------------------------------------------------------------

    TResult Scene::onLoad(Archive *archive)
    {
        return Resource::onLoad(archive);
    }

    //--------------------------------------------------------------------------

    TResult Scene::onUnload()
    {
        // removeAll();
        if (mRootGameObject != nullptr)
        {
            GameObject::destroy(mRootGameObject);
            mRootGameObject = nullptr;
            mRootTransform = nullptr;
        }
        
        return Resource::onUnload();
    }

    //--------------------------------------------------------------------------
}
