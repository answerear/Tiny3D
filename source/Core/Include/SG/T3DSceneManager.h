

#ifndef __T3D_SCENE_MANAGER_H__
#define __T3D_SCENE_MANAGER_H__


#include "Misc/T3DObject.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SceneManager : public Singleton<SceneManager>
    {
    public:

        SceneManager();
        virtual ~SceneManager();

        virtual void renderScene(const SGCameraPtr &camera, const ViewportPtr &viewport);

        const SGCameraPtr &getCurCamera() const { return mCurCamera; }

        const SGNodePtr &getRoot() const { return mRoot; }

        void setRenderer(Renderer *renderer)    { mRenderer = renderer; }

    protected:
        SGCameraPtr mCurCamera;

        SGNodePtr   mRoot;

        Renderer    *mRenderer;

        RenderQueuePtr  mRenderQueue;
    };

    #define T3D_SCENE_MGR           SceneManager::getInstance()
}


#endif  /*__T3D_SCENE_MANAGER_H__*/
