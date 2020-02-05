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
#include "Scene/T3DSceneNode.h"


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

    SceneNodePtr SceneManager::createSceneNode(SceneNodePtr parent, 
        bool autoAddTransform /* = true */, ID uID /* = Node::E_NID_AUTOMATIC */)
    {
        if (mImpl != nullptr)
        {
            return mImpl->createSceneNode(parent, autoAddTransform, uID);
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult SceneManager::addRenderable(Renderable *renderable)
    {
        if (mImpl != nullptr)
        {
            return mImpl->addRenderable(renderable);
        }

        return T3D_ERR_SYS_NOT_INIT;
    }

    //--------------------------------------------------------------------------

    TResult SceneManager::removeRenderable(Renderable *renderable)
    {
        if (mImpl != nullptr)
        {
            return mImpl->removeRenderable(renderable);
        }

        return T3D_ERR_SYS_NOT_INIT;
    }
}
