

#ifndef __T3D_SG_GEOMETRY_H__
#define __T3D_SG_GEOMETRY_H__


#include "SceneGraph/T3DSGRenderable.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGGeometry : public SGRenderable
    {
    public:
        virtual ~SGGeometry();

    protected:
        SGGeometry(uint32_t uID = E_NID_AUTOMATIC);

        virtual void frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue) override;
    };
}


#endif  /*__T3D_SG_GEOMETRY_H__*/
