
#include "SG/Visual/T3DSGMesh.h"
#include "SG/Renderable/T3DSGBox.h"
#include "Bound/T3DAabbBound.h"


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
        removeAllChildren();

        SGBoxPtr box = SGBox::create();
        addChild((SGNodePtr)box);

        mBound = new AabbBound(10, this);

        return true;
    }

    void SGMesh::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {
        SGVisual::frustumCulling(bound, queue);
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