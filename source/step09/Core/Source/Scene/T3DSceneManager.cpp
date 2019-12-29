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


#include "Scene/T3DSceneManager.h"
#include "Scene/T3DSceneTransform3D.h"
#include "Scene/T3DSceneCamera.h"
#include "Scene/T3DSceneLight.h"
#include "Scene/T3DSceneModel.h"
#include "Scene/T3DSceneMesh.h"
#include "Scene/T3DSceneAxis.h"
#include "Scene/T3DSceneQuad.h"
#include "Scene/T3DSceneBillboard.h"
#include "Scene/T3DSceneBox.h"
#include "Scene/T3DSceneSphere.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(SceneManager);

    //--------------------------------------------------------------------------

    SceneManagerPtr SceneManager::create(SceneManagerBase *impl)
    {
        SceneManagerPtr mgr = new SceneManager(impl);
        mgr->release();
        return mgr;
    }

    //--------------------------------------------------------------------------

    SceneManager::SceneManager(SceneManagerBase *impl)
        : mImpl(impl)
    {

    }

    //--------------------------------------------------------------------------

    SceneManager::~SceneManager()
    {
        mImpl = nullptr;
    }

    //--------------------------------------------------------------------------

    TResult SceneManager::update()
    {
        TResult ret = T3D_ERR_SYS_NOT_INIT;

        if (mImpl != nullptr)
        {
            ret = mImpl->update();
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult SceneManager::render(ViewportPtr viewport)
    {
        TResult ret = T3D_ERR_SYS_NOT_INIT;

        if (mImpl != nullptr)
        {
            ret = mImpl->render(viewport);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    SceneNodePtr SceneManager::getRoot() const
    {
        if (mImpl != nullptr)
        {
            return mImpl->getRoot();
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    SceneTransform3DPtr SceneManager::createTransform3D(SceneNodePtr parent, 
        ID uID /* = Node::E_NID_AUTOMATIC */)
    {
        if (mImpl != nullptr)
        {
            return mImpl->createTransform3D(parent, uID);
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    SceneCameraPtr SceneManager::createCamera(SceneNodePtr parent,
        ID uID /* = Node::E_NID_AUTOMATIC */)
    {
        if (mImpl != nullptr)
        {
            return mImpl->createCamera(parent, uID);
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    SceneLightPtr SceneManager::createLight(SceneNodePtr parent,
        ID uID /* = Node::E_NID_AUTOMATIC */)
    {
        if (mImpl != nullptr)
        {
            return mImpl->createLight(parent, uID);
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    SceneModelPtr SceneManager::createModel(SceneNodePtr parent,
        ID uID /* = Node::E_NID_AUTOMATIC */)
    {
        if (mImpl != nullptr)
        {
            return mImpl->createModel(parent, uID);
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    SceneMeshPtr SceneManager::createMesh(SceneNodePtr parent,
        ID uID /* = Node::E_NID_AUTOMATIC */)
    {
        if (mImpl != nullptr)
        {
            return mImpl->createMesh(parent, uID);
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    SceneAxisPtr SceneManager::createAxis(Real X, Real Y, Real Z, SceneNodePtr parent,
        ID uID /* = Node::E_NID_AUTOMATIC */)
    {
        if (mImpl != nullptr)
        {
            return mImpl->createAxis(X, Y, Z, parent, uID);
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    SceneQuadPtr SceneManager::createQuad(const SceneQuad::Quad &quad,
        const String &materialName, SceneNodePtr parent,
        ID uID /* = Node::E_NID_AUTOMATIC */)
    {
        if (mImpl != nullptr)
        {
            return mImpl->createQuad(quad, materialName, parent, uID);
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    SceneBillboardPtr SceneManager::createBillboard(SceneNodePtr parent,
        ID uID /* = Node::E_NID_AUTOMATIC */)
    {
        if (mImpl != nullptr)
        {
            return mImpl->createBillboard(parent, uID);
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    SceneBoxPtr SceneManager::createBox(const Vector3 &center, 
        const Vector3 &extent, SceneNodePtr parent, 
        ID uID /* = Node::E_NID_AUTOMATIC */)
    {
        if (mImpl != nullptr)
        {
            return mImpl->createBox(center, extent, parent, uID);
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    SceneSpherePtr SceneManager::createSphere(const Vector3 &center, Real radius,
        SceneNodePtr parent, ID uID /* = Node::E_NID_AUTOMATIC */)
    {
        if (mImpl != nullptr)
        {
            return mImpl->createSphere(center, radius, parent, uID);
        }

        return nullptr;
    }
}
