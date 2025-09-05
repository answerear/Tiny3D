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
#include "Component/T3DGeometry.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ScenePtr Scene::create(const String &name)
    {
        return T3D_NEW Scene(name);
    }

    //--------------------------------------------------------------------------

    Scene::Scene(const String &name)
        : Resource(name)
    {
        
    }

    //--------------------------------------------------------------------------

    TResult Scene::init()
    {
        mRootGameObject = GameObject::create("SceneRoot");
        mRootTransform = mRootGameObject->addComponent<Transform3D>();
        mRootGameObjectUUID = mRootGameObject->getUUID();
        return T3D_OK;
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
    
    void Scene::update()
    {
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
        if (mRootGameObject != nullptr)
        {
            GameObject::destroy(mRootGameObject);
            mRootGameObject = nullptr;
            mRootTransform = nullptr;
        }

        mGameObjects.clear();
        mCameras.clear();
        
        return Resource::onUnload();
    }

    //--------------------------------------------------------------------------

    void Scene::onPostLoad()
    {
        // 设置根节点
        if (mRootGameObjectUUID != UUID::INVALID)
        {
            const auto it = mGameObjects.find(mRootGameObjectUUID);
            if (it != mGameObjects.end())
            {
                mRootGameObject = it->second;
                // mRootTransform = mRootGameObject->getComponent<Transform3D>();
                mRootTransform = static_cast<Transform3D *>(mRootGameObject->getTransformNode());
            }
        }

        // 设置整棵场景树的层级关系
        for (const auto &item : mGameObjects)
        {
            item.second->setupHierarchy();
        }
    }

    //--------------------------------------------------------------------------

    void Scene::onPostInit()
    {
#if defined (T3D_EDITOR)
        T3D_EDITOR_SCENE.setRuntimeScene(this);
#endif
    }

    //--------------------------------------------------------------------------

    TResult Scene::addGameObject(GameObject *go)
    {
        TResult ret = T3D_OK;

        do
        {
            if (go == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Game Object is nullptr !");
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }

            mGameObjects.emplace(go->getUUID(), go);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Scene::removeGameObject(GameObject *go)
    {
        mGameObjects.erase(go->getUUID());
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    TResult Scene::removeGameObject(const UUID &uuid)
    {
        mGameObjects.erase(uuid);
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    void Scene::onAddComponentForLoadingResource(Component *component)
    {
        if (RTTRType::get<Geometry>() == component->get_type())
        {
            // geometry 组件才需要加载 mesh 资源
            mNeedToLoadResourceComponents.emplace(component);
        }
    }

    //--------------------------------------------------------------------------
}
