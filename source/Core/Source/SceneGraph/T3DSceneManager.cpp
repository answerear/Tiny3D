/***************************************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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
 **************************************************************************************************/

#include "SceneGraph/T3DSceneManager.h"
#include "SceneGraph/T3DSGCamera.h"
#include "SceneGraph//T3DSGTransformNode.h"
#include "SceneGraph/T3DSGRenderable.h"
#include "SceneGraph/T3DSGTransform2D.h"
#include "SceneGraph/T3DSGText2D.h"
#include "Render/T3DRenderer.h"
#include "Render/T3DRenderQueue.h"
#include "Resource/T3DFontManager.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(SceneManager);

    SceneManager::SceneManager()
        : mRoot(nullptr)
        , mRenderer(nullptr)
        , mRenderQueue(nullptr)
    {
        mRenderQueue = RenderQueue::create();
        mRoot = SGTransformNode::create();
        mRoot->setName("Root");
    }

    SceneManager::~SceneManager()
    {
        mRoot->removeAllChildren(true);
        mRoot = nullptr;

        mRenderQueue = nullptr;
    }

    void SceneManager::renderScene(const SGCameraPtr &camera, const ViewportPtr &viewport)
    {
        mCurCamera = camera;
        mRenderer->setViewport(viewport);

        // 优先更新相机变换
        mCurCamera->updateTransform();

        // 更新scene graph上所有结点
        mRoot->updateTransform();

        // 对scene graph上所有结点做frustum culling
        mRoot->frustumCulling(mCurCamera->getBound(), mRenderQueue);

        // 直接对渲染队列的对象渲染
        mRenderer->beginRender(viewport->getBackgroundColor());
        mRenderQueue->render(mRenderer);
        mRenderer->endRender();
    }
}
