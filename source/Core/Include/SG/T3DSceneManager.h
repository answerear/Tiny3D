

#ifndef __T3D_SCENE_MANAGER_H__
#define __T3D_SCENE_MANAGER_H__


#include "Misc/T3DObject.h"


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

    protected:
        SGCameraPtr mCurCamera;

        SGNodePtr   mRoot;
    };

    #define T3D_SCENE_MGR           SceneManager::getInstance()
}


#endif  /*__T3D_SCENE_MANAGER_H__*/
