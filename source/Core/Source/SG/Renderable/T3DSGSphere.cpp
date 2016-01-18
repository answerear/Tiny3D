

#include "SG/Renderable/T3DSGSphere.h"


namespace Tiny3D
{
    SGSpherePtr SGSphere::create(uint32_t unID /* = E_NID_AUTOMATIC */)
    {
        SGSpherePtr node = new SGSphere(unID);
        node->release();
        return node;
    }

    SGSphere::SGSphere(uint32_t uID /* = E_NID_AUTOMATIC */)
        : SGRenderable(uID)
    {

    }

    SGSphere::~SGSphere()
    {

    }

    SGNode::Type SGSphere::getNodeType() const
    {
        return E_NT_SPHERE;
    }

    SGNodePtr SGSphere::clone() const
    {
        SGSpherePtr node = new SGSphere();
        node->release();
        cloneProperties(node);
        return node;
    }

    void SGSphere::cloneProperties(const SGNodePtr &node) const
    {
        SGRenderable::cloneProperties(node);
    }
}
