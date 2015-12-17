

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

        virtual void renderScene(SGCamera *camera, Viewport *viewport);

        SGCamera *getCurCamera()    { return mCurCamera; }

        SGNode *getRoot()  { return mRoot; }

    protected:
        SGCamera    *mCurCamera;

        SGNode      *mRoot;
    };

    #define T3D_SCENE_MGR           SceneManager::getInstance()
}


#endif  /*__T3D_SCENE_MANAGER_H__*/
