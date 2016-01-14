

#include "SG/T3DSceneManager.h"
#include "SG/Visual/T3DSGCamera.h"
#include "SG/Node/T3DSGTransformNode.h"
#include "SG/Renderable/T3DSGRenderable.h"
#include "Render/T3DRenderer.h"
#include "SG/T3DRenderQueue.h"


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
    }

    SceneManager::~SceneManager()
    {
        mRenderQueue = nullptr;
        mRoot = nullptr;
    }

    void SceneManager::renderScene(const SGCameraPtr &camera, const ViewportPtr &viewport)
    {
        mCurCamera = camera;
        mRenderer->setViewport(viewport);

        mRoot->updateTransform();
        mRoot->frustumCulling(mCurCamera->getBound(), mRenderQueue);

        mRenderer->beginRender();
        mRenderQueue->render(mRenderer);
        mRenderer->endRender();
    }
}
