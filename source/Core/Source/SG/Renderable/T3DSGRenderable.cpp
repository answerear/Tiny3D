

#include "SG/Renderable/T3DSGRenderable.h"
#include "SG/Visual/T3DSGVisual.h"


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
            Node *p = getParent();
            SGVisual *parent = (SGVisual *)p;
            return parent->getWorldTransform().getAffineMatrix();
        }

        return Matrix4::IDENTITY;
    }
}
