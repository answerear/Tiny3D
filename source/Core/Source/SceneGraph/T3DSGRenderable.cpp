

#include "SceneGraph/T3DSGRenderable.h"
#include "SceneGraph/T3DSGTransformNode.h"


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
        NodePtr node = getParent();
        while (node != nullptr && node->getNodeType() != E_NT_TRANSFORM)
        {
            node = node->getParent();
        }

        if (node != nullptr)
        {
            T3D_ASSERT(node->getNodeType() == E_NT_TRANSFORM);
            SGTransformNodePtr parent = smart_pointer_cast<SGTransformNode>(node);
            return parent->getLocalToWorldTransform().getAffineMatrix();
        }

        return Matrix4::IDENTITY;
    }
}
