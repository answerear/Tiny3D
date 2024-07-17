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


#include "SceneWindow.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------

    void SceneWindow::refreshRenderTarget()
    {
#if 0
        RenderWindowPtr rw = T3D_AGENT.getDefaultRenderWindow();
        RenderTargetPtr rt = RenderTarget::create(rw);
#else
        RenderWindow *rw = T3D_AGENT.getDefaultRenderWindow();
        const int32_t width = rw->getDescriptor().Width;
        const int32_t height = rw->getDescriptor().Height;
        if (mRenderTarget == nullptr || width != mRenderTarget->getRenderTexture()->getWidth() || height != mRenderTarget->getRenderTexture()->getHeight())
        {
            if (mRenderTarget != nullptr)
            {
                T3D_TEXTURE_MGR.unload(mRenderTarget->getRenderTexture());
                mRenderTarget = nullptr;
            }
            
            RenderTexturePtr renderTex = T3D_TEXTURE_MGR.createRenderTexture("__SceneRT__", width, height, PixelFormat::E_PF_R8G8B8A8);
            mRenderTarget = RenderTarget::create(renderTex);
            mSceneRT = renderTex->getPixelBuffer()->getRHIResource()->getNativeObject();

            if (mDefaultCamera != nullptr)
            {
                mDefaultCamera->setRenderTarget(mRenderTarget);
            }
        }
#endif
    }

    //--------------------------------------------------------------------------

    TResult SceneWindow::buildScene()
    {
        TResult ret = T3D_OK;

        do
        {
            // create scene
            ScenePtr scene = T3D_SCENE_MGR.createScene("__Scene__");
            T3D_SCENE_MGR.setCurrentScene(scene);
    
            // root game object
            GameObjectPtr go = GameObject::create("__SceneRoot__");
            scene->addRootGameObject(go);
            Transform3DPtr root = go->addComponent<Transform3D>();

            refreshRenderTarget();
    
            CameraPtr camera;

            // center camera
            go = GameObject::create("__SceneCamera__");
            Transform3DPtr center = go->addComponent<Transform3D>();
            root->addChild(center);
            camera = go->addComponent<Camera>();
            camera->setOrder(1);
            Viewport vpCenter {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
            camera->setViewport(vpCenter);
            camera->setClearColor(ColorRGB::GRAY);
            camera->setRenderTarget(mRenderTarget);
            scene->addCamera(camera);
            mDefaultCamera = camera;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult SceneWindow::onCreate()
    {
        TResult ret = T3D_OK;

        do
        {
            ret = buildScene();
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void SceneWindow::onDestroy()
    {
        mSceneRT = nullptr;
        mRenderTarget = nullptr;
    }

    //--------------------------------------------------------------------------

    void SceneWindow::onGUI()
    {
        refreshRenderTarget();
        
        int32_t rtWidth = mRenderTarget->getRenderTexture()->getWidth();
        int32_t rtHeight = mRenderTarget->getRenderTexture()->getHeight();

        auto region = ImGui::GetContentRegionAvail();
        float u0 = (rtWidth - region.x) * 0.5f;
        float v0 = (rtHeight - region.y) * 0.5f;
        float u1 = u0 + region.x;
        float v1 = v0 + region.y;
        ImVec2 uv0(u0, v0);
        ImVec2 uv1(u1, v1);
        ImGui::Image(mSceneRT, region, uv0, uv1);
    }
    
    //--------------------------------------------------------------------------
    
    NS_END
}
