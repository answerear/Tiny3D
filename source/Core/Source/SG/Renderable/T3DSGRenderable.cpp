

#include "SG/Renderable/T3DSGRenderable.h"


namespace Tiny3D
{
    SGRenderable::SGRenderable(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGNode(unID)
    {

    }

    SGRenderable::~SGRenderable()
    {

    }

    void SGRenderable::cloneProperties(SGNode *node) const
    {
        SGNode::cloneProperties(node);

        SGRenderable *newNode = (SGRenderable *)node;
        newNode->mMaterial = mMaterial;
    }

    void SGRenderable::updateTransform()
    {
        SGNode::updateTransform();
    }
}
