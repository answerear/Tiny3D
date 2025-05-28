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


#include "EditorSceneImpl.h"
#include "EditorEventDefine.h"
#include "ProjectManager.h"


#define UVN_CAMERA
#define PERSPECTIVE_CAMERA
#define ASSETS_FROM_FILE

const char *SUB_MESH_NAME = "#0";


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------

    #define EDITOR_SCENE_ROOT_NAME      "__@#SceneRoot#@__"
    #define EDITOR_BUILTIN_ROOT_NAME    "__@#Builtin#@__"
    #define EDITOR_SCENE_CAMERA_NAME    "__@#SceneCamera#@__"
    #define EDITOR_GAME_ROOT_NAME       "__@#GameRoot#@__"
    #define EDITOR_BUILTIN_CAMERA_NAME  "__@#BuiltinCamera#@__"
    
    //--------------------------------------------------------------------------

    EditorSceneImplPtr EditorSceneImpl::create(const String &name)
    {
        return new EditorSceneImpl(name);
    }

    //--------------------------------------------------------------------------

    EditorSceneImpl::EditorSceneImpl(const String &name)
        : EditorScene(name)
    {
    }
    
    //--------------------------------------------------------------------------

    EditorSceneImpl::~EditorSceneImpl()
    {
        mSceneRT = nullptr;
        if (mSceneTarget != nullptr)
        {
            T3D_TEXTURE_MGR.unload(mSceneTarget->getRenderTexture());
        }
        mSceneTarget = nullptr;
        
        // T3D_SCENE_MGR.unloadScene();
    }

    //--------------------------------------------------------------------------

    GameObject *EditorSceneImpl::getRootGameObject() const
    {
        if (mRuntimeScene != nullptr)
        {
            return mRuntimeScene->getRootGameObject();
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------

    Transform3D *EditorSceneImpl::getRootTransform() const
    {
        if (mRuntimeScene != nullptr)
        {
            return mRuntimeScene->getRootTransform();
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------

    GameObject *EditorSceneImpl::getRuntimeRootGameObject() const
    {
        return mRuntimeRootGO;
    }

    //--------------------------------------------------------------------------

    Transform3D *EditorSceneImpl::getRuntimeRootTransform() const
    {
        return mRuntimeRootXform;
    }
    
    //--------------------------------------------------------------------------

    bool EditorSceneImpl::isSceneRoot(TransformNode *node) const
    {
        return node->getParent() == mRuntimeRootXform;
    }

    //--------------------------------------------------------------------------

    void EditorSceneImpl::update()
    {
        mRootGameObject->update();
    }

    //--------------------------------------------------------------------------

    TResult EditorSceneImpl::addCamera(Camera *camera)
    {
        if (mRuntimeScene != nullptr)
        {
            return mRuntimeScene->addCamera(camera);
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult EditorSceneImpl::removeCamera(Camera *camera)
    {
        if (mRuntimeScene != nullptr)
        {
            return mRuntimeScene->removeCamera(camera);
        }
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    TResult EditorSceneImpl::removeCamera(const UUID &uuid)
    {
        if (mRuntimeScene != nullptr)
        {
            return mRuntimeScene->removeCamera(uuid);
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult EditorSceneImpl::removeCamera(const String &name)
    {
        if (mRuntimeScene != nullptr)
        {
            return mRuntimeScene->removeCamera(name);
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    const CameraList &EditorSceneImpl::getCameras() const
    {
        static CameraList cameras;
        if (mRuntimeScene != nullptr)
        {
            return mRuntimeScene->getCameras();
        }
        return cameras;
    }

    //--------------------------------------------------------------------------

    TResult EditorSceneImpl::addGameObject(GameObject *go)
    {
        if (mRuntimeScene != nullptr)
        {
            return mRuntimeScene->addGameObject(go);
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult EditorSceneImpl::removeGameObject(GameObject *go)
    {
        if (mRuntimeScene != nullptr)
        {
            return mRuntimeScene->removeGameObject(go);
        }

        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    TResult EditorSceneImpl::removeGameObject(const UUID &uuid)
    {
        if (mRuntimeScene != nullptr)
        {
            return mRuntimeScene->removeGameObject(uuid);
        }

        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    GameObject *EditorSceneImpl::getGameObject(const UUID &uuid) const
    {
        GameObject *go = nullptr;

        if (mRuntimeScene != nullptr)
        {
            go = mRuntimeScene->getGameObject(uuid);
        }

        return go;
    }

    //--------------------------------------------------------------------------

    void EditorSceneImpl::build()
    {
        // 生成 scene 使用的 render texture
        refreshSceneRenderTarget();
        
        // 创建场景
        // ScenePtr scene = T3D_SCENE_MGR.createScene("__Scene__");
        T3D_SCENE_MGR.setCurrentScene(this);
    
        // 根节点
        GameObjectPtr go = GameObject::create(EDITOR_SCENE_ROOT_NAME, false);
        // scene->addRootGameObject(go);
        Transform3DPtr root = go->addComponent<Transform3D>();
        mRootTransform->addChild(root);

        // 编辑器内部用的根节点
        go = GameObject::create(EDITOR_BUILTIN_ROOT_NAME, false);
        Transform3DPtr builtinNode = go->addComponent<Transform3D>();
        root->addChild(builtinNode);

        // 游戏用的根节点
        go = GameObject::create(EDITOR_GAME_ROOT_NAME, false);
        Transform3DPtr gameNode = go->addComponent<Transform3D>();
        root->addChild(gameNode);
        mRuntimeRootXform = gameNode;

        // 内置的 scene 相机 
        go = GameObject::create(EDITOR_BUILTIN_CAMERA_NAME, false);
        Transform3DPtr cameraNode = go->addComponent<Transform3D>();
        builtinNode->addChild(cameraNode);
        CameraPtr camera = go->addComponent<Camera>();
        camera->setOrder(0xFFFFFFFF);
        Viewport vp {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
        camera->setViewport(vp);
        camera->setClearColor(ColorRGB(0.133f, 0.231f, 0.329f));
        camera->setRenderTarget(mSceneTarget);
        // scene->addCamera(camera);

        // camera for perspective
        camera->setProjectionType(Camera::Projection::kPerspective);
        camera->setFovY(Radian(Math::PI / 3.0f));
        Real as = mGameRTWidth / mGameRTHeight;// Real(rw->getDescriptor().Width) / Real(rw->getDescriptor().Height);
        camera->setAspectRatio(as);
        // camera->setFovY(Radian(Math::PI * 0.5f));
        camera->setNearPlaneDistance(0.1f);
        camera->setFarPlaneDistance(1000.0f);
        
        // construct camera position & orientation & scaling
        Vector3 eye(2.0f, 2.0f, -4.0f);
        Vector3 obj(0.0f, 0.0f, 0.0f);
        camera->lookAt(eye, obj, Vector3::UP);
        
        // construct frustum bound
        auto frustum = go->addComponent<FrustumBound>();
        T3D_ASSERT(frustum != nullptr);
        
        mSceneCamera = camera;

#if defined (TEST_SCENE)
        // For test
        // camera
        buildCamera(gameNode);

        // cube
        buildCube(gameNode);
#endif
    }

    //--------------------------------------------------------------------------

    void EditorSceneImpl::refreshSceneRenderTarget()
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
            
            RenderTexturePtr renderTex = T3D_TEXTURE_MGR.createRenderTexture("__@#SceneRT#@__", width, height, PixelFormat::E_PF_B8G8R8A8);
            RenderTexturePtr renderDSTex = T3D_TEXTURE_MGR.createRenderTexture("__@#SceneRT_DS#@__", width, height, PixelFormat::E_PF_D24_UNORM_S8_UINT);
            mSceneTarget = RenderTarget::create(renderTex, renderDSTex);
            mSceneRT = renderTex->getPixelBuffer()->getRHIResource()->getNativeObject();

            if (mSceneCamera != nullptr)
            {
                mSceneCamera->setRenderTarget(mSceneTarget);
            }
        }
    }

    //--------------------------------------------------------------------------

    void EditorSceneImpl::refreshGameRenderTarget(const ImVec2 &size)
    {
        bool rtIsDirty = false;
        // if (mGameTarget == nullptr
        //     || mGameTarget->getRenderTexture()->getWidth() != static_cast<uint32_t>(size.x)
        //     || mGameTarget->getRenderTexture()->getHeight() != static_cast<uint32_t>(size.y))
        // {
        //     // 大小发生变化，重新生成
        //     if (mGameTarget != nullptr)
        //     {
        //         T3D_TEXTURE_MGR.unload(mGameTarget->getRenderTexture());
        //         mGameTarget = nullptr;
        //     }
        //
        //     RenderTexturePtr renderTex = T3D_TEXTURE_MGR.createRenderTexture("__GameRT__", static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y), PixelFormat::E_PF_R8G8B8A8);
        //     mGameTarget = RenderTarget::create(renderTex);
        //     mGameRT = renderTex->getPixelBuffer()->getRHIResource()->getNativeObject();
        //     rtIsDirty = true;
        // }
        if (mGameTarget == nullptr)
        {
            RenderTexturePtr renderTex = T3D_TEXTURE_MGR.createRenderTexture("__@#GameRT#@__", static_cast<uint32_t>(mGameRTWidth), static_cast<uint32_t>(mGameRTHeight), PixelFormat::E_PF_B8G8R8A8);
            RenderTexturePtr renderDSTex = T3D_TEXTURE_MGR.createRenderTexture("__@#GameRT_DS#@__", static_cast<uint32_t>(mGameRTWidth), static_cast<uint32_t>(mGameRTHeight), PixelFormat::E_PF_D24_UNORM_S8_UINT);
            mGameTarget = RenderTarget::create(renderTex, renderDSTex);
            mGameRT = renderTex->getPixelBuffer()->getRHIResource()->getNativeObject();
            rtIsDirty = true;
        }
        
        auto scene = T3D_SCENE_MGR.getCurrentScene();

        if (scene != nullptr)
        {
            // 做个 trick ，把渲染到屏幕的改成渲染到纹理
            if (rtIsDirty)
            {
                for (auto item : scene->getCameras())
                {
                    // if (item.second->getGameObject()->getName() != "__SceneCamera__")
                    {
                        // 要重新设置 render target ，并且不是编辑器 scene 视图使用的那个 render target
                        item.second->setRenderTarget(mGameTarget);
                    }
                }
            }
        }
    }
    
    //--------------------------------------------------------------------------

    NS_END
}
