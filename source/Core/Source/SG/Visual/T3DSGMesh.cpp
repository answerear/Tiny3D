
#include "SG/Visual/T3DSGMesh.h"
#include "SG/Renderable/T3DSGBox.h"
#include "SG/Renderable/T3DSGSphere.h"
#include "Bound/T3DSphereBound.h"
#include "Bound/T3DAabbBound.h"


namespace Tiny3D
{
    SGMeshPtr SGMesh::create(uint32_t unID /* = E_NID_AUTOMATIC */)
    {
        SGMeshPtr mesh = new SGMesh(unID);
        mesh->release();
        return mesh;
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
        box->loadBox();
        addChild(box);

        AabbBoundPtr bound = AabbBound::create(getNodeID(), this);
        mBound = bound;
        bound->setParam(box->getMinX(), box->getMaxX(), box->getMinY(), box->getMaxY(), box->getMinZ(), box->getMaxZ());

        mMeshType = E_TYPE_BOX;

        return true;
    }

    bool SGMesh::loadSphere()
    {
        removeAllChildren();

        SGSpherePtr sphere = SGSphere::create();
        sphere->loadSphere();
        addChild(sphere);

        SphereBoundPtr bound = SphereBound::create(getNodeID(), this);
        mBound = bound;
        bound->setParams(Vector3::ZERO, sphere->getRadius());

        mMeshType = E_TYPE_SPHERE;

        return true;
    }

    void SGMesh::frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue)
    {
        SGVisual::frustumCulling(bound, queue);
    }

    SGNodePtr SGMesh::clone() const
    {
        SGMeshPtr mesh = new SGMesh();
        mesh->release();
        cloneProperties(mesh);
        return mesh;
    }

    void SGMesh::cloneProperties(const SGNodePtr &node) const
    {
        SGVisual::cloneProperties(node);

        const SGMeshPtr &newNode = (const SGMeshPtr &)node;
        newNode->mMeshType = mMeshType;
    }
}