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


#include "Resource/T3DScene.h"
#include "Kernel/T3DGameObject.h"
#include "Component/T3DCamera.h"
#include "Component/T3DTransformNode.h"


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

    void Scene::addRootGameObject(GameObject *gameObject)
    {
        mRootGameObjects.emplace_back(gameObject);
    }

    //--------------------------------------------------------------------------

    GameObjectPtr Scene::addRootGameObject(const String &name)
    {
        return GameObject::create(name);
    }

    //--------------------------------------------------------------------------

    void Scene::removeRootGameObject(const String &name)
    {
        for (auto itr = mRootGameObjects.begin(); itr != mRootGameObjects.end(); ++itr)
        {
            if ((*itr)->getName() == name)
            {
                mRootGameObjects.erase(itr);
                break;
            }
        }
    }

    //--------------------------------------------------------------------------

    void Scene::removeRootGameObject(GameObject *gameObject)
    {
        for (auto itr = mRootGameObjects.begin(); itr != mRootGameObjects.end(); ++itr)
        {
            if ((*itr) == gameObject)
            {
                mRootGameObjects.erase(itr);
                break;
            }
        }
    }

    //--------------------------------------------------------------------------

    void Scene::removeRootGameObject(const UUID &uuid)
    {
        for (auto itr = mRootGameObjects.begin(); itr != mRootGameObjects.end(); ++itr)
        {
            if ((*itr)->getUUID() == uuid)
            {
                mRootGameObjects.erase(itr);
                break;
            }
        }
    }

    //--------------------------------------------------------------------------

    void Scene::update()
    {
        for (auto go : mRootGameObjects)
        {
            TransformNodePtr root = go->getComponent<TransformNode>();

            // 遍历更新
            root->visit();
        }
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
}
