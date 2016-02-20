

#include "SG/Visual/T3DSGVirtualMesh.h"


namespace Tiny3D
{
    SGVirtualMeshPtr SGVirtualMesh::create(uint32_t uID /* = E_NID_AUTOMATIC */)
    {
        SGVirtualMeshPtr mesh = new SGVirtualMesh(uID);
        mesh->release();
        return mesh;
    }

    SGVirtualMesh::SGVirtualMesh(uint32_t uID /* = E_NID_AUTOMATIC */)
        : SGVisual(uID)
    {

    }

    SGVirtualMesh::~SGVirtualMesh()
    {

    }

    SGNode::Type SGVirtualMesh::getNodeType() const
    {
        return E_NT_VIRTUAL;
    }

    SGNodePtr SGVirtualMesh::clone() const
    {
        SGVirtualMeshPtr mesh = create();
        cloneProperties(mesh);
        return mesh;
    }

    void SGVirtualMesh::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {
        if (isVisible())
        {
            SGNode::frustumCulling(bound, queue);
        }
    }
}
