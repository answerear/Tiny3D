

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

    SGNode::Type SGCamera::getNodeType() const
    {
        return E_NT_CAMERA;
    }

    void SGCamera::cloneProperties(SGNode *node)
    {
        SGVisual::cloneProperties(node);

        SGCamera *src = (SGCamera *)node;


    }

    void SGCamera::addChild(SGNode *child)
    {
        // Can't add any type node for child node.
        T3D_ASSERT(0);
    }

    void SGCamera::updateBound()
    {

    }

    void SGCamera::renderScene(Viewport *viewport)
    {

    }
}
