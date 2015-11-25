

#include "SG/T3DSceneManager.h"
#include "SG/T3DSGCamera.h"
#include "SG/T3DSGNode.h"
#include "SG/T3DSGTransformNode.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(SceneManager);

    SceneManager::SceneManager()
        : mRoot(nullptr)
    {
        mRoot = SGTransformNode::create();
        T3D_SAFE_ACQUIRE(mRoot);
    }

    SceneManager::~SceneManager()
    {
        T3D_SAFE_RELEASE(mRoot);
    }

    void SceneManager::renderScene(SGCamera *camera, Viewport *viewport)
    {
        mCurCamera = camera;
        mRoot->update();
    }
}
