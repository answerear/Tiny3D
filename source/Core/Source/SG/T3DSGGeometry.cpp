

#include "SG/T3DSGGeometry.h"
#include "SG/T3DRenderQueue.h"


namespace Tiny3D
{
    SGGeometry::SGGeometry(uint32_t uID /* = E_NID_AUTOMATIC */)
        : SGRenderable(uID)
    {

    }

    SGGeometry::~SGGeometry()
    {

    }

    void SGGeometry::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {
        queue->addRenderable(RenderQueue::E_GRPID_SOLID, this);
    }
}
