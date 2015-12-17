

#include "SG/T3DSGCube.h"


namespace Tiny3D
{
    SGCube::SGCube(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGRenderable(unID)
    {

    }

    SGCube::~SGCube()
    {

    }

    SGNode::Type SGCube::getNodeType() const
    {
        return E_NT_CUBE;
    }

    SGNode *SGCube::clone() const
    {
        SGCube *node = new SGCube();
        cloneProperties(node);
        return node;
    }

    void SGCube::cloneProperties(SGNode *node) const
    {

    }
}
