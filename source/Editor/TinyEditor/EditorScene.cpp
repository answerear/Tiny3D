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


#include "EditorScene.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------

    EditorScene::~EditorScene()
    {
        mSceneRT = nullptr;
        if (mSceneTarget != nullptr)
        {
            T3D_TEXTURE_MGR.unload(mSceneTarget->getRenderTexture());
        }
        mSceneTarget = nullptr;
        
        T3D_SCENE_MGR.unloadScene();
    }

    //--------------------------------------------------------------------------

    void EditorScene::build()
    {
        // 生成 scene 使用的 render texture
        refreshSceneRenderTarget();
        
        // 创建场景
        ScenePtr scene = T3D_SCENE_MGR.createScene("__Scene__");
        T3D_SCENE_MGR.setCurrentScene(scene);
    
        // 根节点
        GameObjectPtr go = GameObject::create("__SceneRoot__");
        scene->addRootGameObject(go);
        Transform3DPtr root = go->addComponent<Transform3D>();

        // 编辑器内部用的根节点
        go = GameObject::create("__Builtin__");
        Transform3DPtr builtinNode = go->addComponent<Transform3D>();
        root->addChild(builtinNode);

        // 游戏用的根节点
        go = GameObject::create("__Game__");
        Transform3DPtr gameNode = go->addComponent<Transform3D>();
        root->addChild(gameNode);

        // 内置的 scene 相机 
        go = GameObject::create("__SceneCamera__");
        Transform3DPtr cameraNode = go->addComponent<Transform3D>();
        builtinNode->addChild(cameraNode);
        CameraPtr camera = go->addComponent<Camera>();
        camera->setOrder(65535);
        Viewport vpCenter {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
        camera->setViewport(vpCenter);
        camera->setClearColor(ColorRGB::GRAY);
        camera->setRenderTarget(mSceneTarget);
        scene->addCamera(camera);
        mSceneCamera = camera;

        // For test
        go = GameObject::create("root");
        Transform3DPtr node = go->addComponent<Transform3D>();
        gameNode->addChild(node);
        camera = go->addComponent<Camera>();
        camera->setOrder(0);
        Viewport vp {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
        camera->setViewport(vp);
        camera->setClearColor(ColorRGB::BLUE);
        auto rw = T3D_AGENT.getDefaultRenderWindow();
        RenderTargetPtr rt = RenderTarget::create(rw);
        camera->setRenderTarget(rt);
        scene->addCamera(camera);
    }

    //--------------------------------------------------------------------------

    void EditorScene::refreshSceneRenderTarget()
    {
        RenderWindow *rw = T3D_AGENT.getDefaultRenderWindow();
        const uint32_t width = static_cast<uint32_t>(rw->getDescriptor().Width);
        const uint32_t height = static_cast<uint32_t>(rw->getDescriptor().Height);
        if (mSceneTarget == nullptr
            || width != mSceneTarget->getRenderTexture()->getWidth()
            || height != mSceneTarget->getRenderTexture()->getHeight())
        {
            if (mSceneTarget != nullptr)
            {
                T3D_TEXTURE_MGR.unload(mSceneTarget->getRenderTexture());
                mSceneTarget = nullptr;
            }
            
            RenderTexturePtr renderTex = T3D_TEXTURE_MGR.createRenderTexture("__SceneRT__", width, height, PixelFormat::E_PF_R8G8B8A8);
            mSceneTarget = RenderTarget::create(renderTex);
            mSceneRT = renderTex->getPixelBuffer()->getRHIResource()->getNativeObject();

            if (mSceneCamera != nullptr)
            {
                mSceneCamera->setRenderTarget(mSceneTarget);
            }
        }
    }

    //--------------------------------------------------------------------------

    void EditorScene::refreshGameRenderTarget(const ImVec2 &size)
    {
        bool rtIsDirty = false;
        if (mGameTarget == nullptr
            || mGameTarget->getRenderTexture()->getWidth() != static_cast<uint32_t>(size.x)
            || mGameTarget->getRenderTexture()->getHeight() != static_cast<uint32_t>(size.y))
        {
            if (mGameTarget != nullptr)
            {
                T3D_TEXTURE_MGR.unload(mGameTarget->getRenderTexture());
                mGameTarget = nullptr;
            }

            RenderTexturePtr renderTex = T3D_TEXTURE_MGR.createRenderTexture("__GameRT__", static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y), PixelFormat::E_PF_R8G8B8A8);
            mGameTarget = RenderTarget::create(renderTex);
            mGameRT = renderTex->getPixelBuffer()->getRHIResource()->getNativeObject();
            rtIsDirty = true;
        }
        
        auto scene = T3D_SCENE_MGR.getCurrentScene();

        if (scene != nullptr)
        {
            for (auto item : scene->getCameras())
            {
                if (item.second->getRenderTarget()->getType() == RenderTarget::Type::E_RT_WINDOW
                    || (rtIsDirty && item.second->getRenderTarget()->getRenderTexture()->getName() != "__SceneRT__"))
                {
                    // 要重新设置 render target ，并且不是编辑器 scene 视图使用的那个 render target
                    item.second->setRenderTarget(mGameTarget);
                }
            }
        }
    }

    //--------------------------------------------------------------------------

    NS_END
}
