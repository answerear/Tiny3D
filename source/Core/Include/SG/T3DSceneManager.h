

#ifndef __T3D_SCENE_MANAGER_H__
#define __T3D_SCENE_MANAGER_H__


#include "Misc/T3DObject.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SceneManager : public Singleton<SceneManager>
    {
    public:
        enum RenderPriority
        {
            E_PRIORITY_BACKGROUND = 0,
            E_PRIORITY_SKY = 10,
            E_PRIORITY_NORMAL = 20,
            E_PRIORITY_ = 50
        };

        SceneManager();
        virtual ~SceneManager();

        virtual void renderScene(const SGCameraPtr &camera, const ViewportPtr &viewport);

        const SGCameraPtr &getCurCamera() const { return mCurCamera; }

        const SGNodePtr &getRoot() const { return mRoot; }

        void setRenderer(Renderer *renderer)    { mRenderer = renderer; }

        void addRenderQueue(RenderPriority priority, const SGRenderablePtr &renderable);

    protected:
        typedef std::map<int32_t, SGRenderablePtr>  RenderQueue;
        typedef RenderQueue::iterator               RenderQueueItr;
        typedef RenderQueue::const_iterator         RenderQueueConstItr;

        RenderQueue mRenderQueue;

        SGCameraPtr mCurCamera;

        SGNodePtr   mRoot;

        Renderer    *mRenderer;
    };

    #define T3D_SCENE_MGR           SceneManager::getInstance()
}


#endif  /*__T3D_SCENE_MANAGER_H__*/
