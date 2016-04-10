

#include "SG/Visual/T3DSGShape.h"


namespace Tiny3D
{
    SGShapePtr SGShape::create(uint32_t uID /* = E_NID_AUTOMATIC */)
    {
        SGShapePtr shape = new SGShape(uID);
        shape->release();
        return shape;
    }

    SGShape::SGShape(uint32_t uID /* = E_NID_AUTOMATIC */)
        : SGVisual(uID)
    {

    }

    SGShape::~SGShape()
    {

    }

    Node::Type SGShape::getNodeType() const
    {
        return E_NT_SHAPE;
    }

    NodePtr SGShape::clone() const
    {
        SGShapePtr shape = create();
        cloneProperties(shape);
        return shape;
    }

    void SGShape::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {
        if (isVisible())
        {
            SGNode::frustumCulling(bound, queue);
        }
    }
}
