
#include "SG/Visual/T3DSGMesh.h"


namespace Tiny3D
{
    SGMeshPtr SGMesh::create(uint32_t unID /* = E_NID_AUTOMATIC */)
    {
        SGMesh *mesh = new SGMesh(unID);
        SGMeshPtr ptr(mesh);
        mesh->release();
        return ptr;
    }

    SGMesh::SGMesh(uint32_t unID /* = E_NID_AUTOMATIC */)
        : SGVisual(unID)
    {

    }

    SGMesh::~SGMesh()
    {

    }

    SGNode::Type SGMesh::getNodeType() const
    {
        return E_NT_MESH;
    }

    bool SGMesh::loadBox()
    {
        return true;
    }

    SGNodePtr SGMesh::clone() const
    {
        SGMesh *mesh = new SGMesh();
        SGNodePtr ptr(mesh);
        mesh->release();
        cloneProperties(mesh);
        return ptr;
    }

    void SGMesh::cloneProperties(SGNode *node) const
    {
        SGVisual::cloneProperties(node);
    }
}