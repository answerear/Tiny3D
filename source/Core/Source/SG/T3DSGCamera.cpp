

#include "SG/T3DSGCamera.h"


namespace Tiny3D
{
    SGCamera::SGCamera(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGVisual(unID)
    {

    }

    SGCamera::~SGCamera()
    {

    }

    void SGCamera::addChild(SGNode *child)
    {
        // Can't add any type node for child node.
        T3D_ASSERT(0);
    }

    void SGCamera::removeChild(SGNode *child)
    {
        T3D_ASSERT(0);
    }

    void SGCamera::removeChild(uint32_t nodeID)
    {
        T3D_ASSERT(0);
    }

    void SGCamera::removeAllChildren()
    {
        T3D_ASSERT(0);
    }
}
