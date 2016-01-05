

#include "SG/Renderable/T3DSGCube.h"


namespace Tiny3D
{
    SGNodePtr SGCube::create(uint32_t unID /* = E_NID_AUTOMATIC */)
    {
        SGCube *cube = new SGCube(unID);
        SGNodePtr ptr(cube);
        cube->release();
        return ptr;
    }

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



    SGNodePtr SGCube::clone() const
    {
        SGCube *node = new SGCube();
        SGNodePtr ptr(node);
        cloneProperties(node);
        node->release();
        return ptr;
    }

    void SGCube::cloneProperties(SGNode *node) const
    {

    }
}
