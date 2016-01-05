

#include "SG/T3DSceneManager.h"
#include "SG/Visual/T3DSGCamera.h"
#include "SG/Node/T3DSGTransformNode.h"
#include "Render/T3DRenderer.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(SceneManager);

    SceneManager::SceneManager()
        : mRoot(nullptr)
        , mRenderer(nullptr)
    {
        mRoot = SGTransformNode::create();
    }

    SceneManager::~SceneManager()
    {
        mRoot = nullptr;
    }

    void SceneManager::renderScene(const SGCameraPtr &camera, const ViewportPtr &viewport)
    {
        mCurCamera = camera;
        mRenderer->setViewport(viewport);

        mRoot->updateTransform();

        mRenderer->beginRender();

        mRenderer->endRender();
    }
}
