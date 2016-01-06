

#include "SG/Renderable/T3DSGRenderable.h"
#include "SG/Visual/T3DSGVisual.h"
#include "SG/T3DRenderQueue.h"


namespace Tiny3D
{
    SGRenderable::SGRenderable(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGNode(unID)
    {

    }

    SGRenderable::~SGRenderable()
    {

    }

    const Matrix4 &SGRenderable::getWorldMatrix() const
    {
        if (getParent() != nullptr)
        {
            SGNode *p = getParent();
            SGVisual *parent = (SGVisual *)p;
            return parent->getWorldTransform().getAffineMatrix();
        }

        return Matrix4::IDENTITY;
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
