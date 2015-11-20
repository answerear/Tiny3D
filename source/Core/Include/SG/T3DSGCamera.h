

#ifndef __T3D_SG_CAMERA_H__
#define __T3D_SG_CAMERA_H__


#include "SG/T3DSGNode.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGCamera : public SGNode
    {
        friend class Viewport;

    public:
        SGCamera();
        virtual ~SGCamera();

    protected:
        virtual void update();
    };
}

#endif  /*__T3D_SG_CAMERA_H__*/

