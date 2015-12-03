

#include "SG/T3DSGRenderable.h"


namespace Tiny3D
{
    SGRenderable::SGRenderable(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGNode(unID)
    {

    }

    SGRenderable::~SGRenderable()
    {

    }

    void SGRenderable::updateTransform()
    {
        SGNode::updateTransform();
    }
}
