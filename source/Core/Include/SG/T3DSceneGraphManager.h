

#ifndef __T3D_SCENE_GRAPH_MANAGER_H__
#define __T3D_SCENE_GRAPH_MANAGER_H__


#include "Misc/T3DObject.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SceneGraphManager : public Object
    {
    public:
        SceneGraphManager();
        virtual ~SceneGraphManager();

        virtual void renderScene(SGCamera *camera, Viewport *viewport);
    };
}


#endif  /*__T3D_SCENE_GRAPH_MANAGER_H__*/
