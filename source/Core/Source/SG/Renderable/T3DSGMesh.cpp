

#include "SG/Renderable/T3DSGMesh.h"
#include "Resource/T3DModel.h"
#include "Resource/T3DModelManager.h"
#include "DataStruct/T3DGeometryData.h"


namespace Tiny3D
{
    SGMeshPtr SGMesh::create(const ObjectPtr &geometryData, bool isVertexShared, uint32_t uID /* = E_NID_AUTOMATIC */)
    {
        SGMeshPtr mesh = new SGMesh(uID);
        if (mesh != nullptr && mesh->init(geometryData, isVertexShared))
        {
            mesh->release();
        }
        else
        {
            T3D_SAFE_RELEASE(mesh);
        }
        return mesh;
    }

    SGMesh::SGMesh(uint32_t uID /* = E_NID_AUTOMATIC */)
        : SGGeometry(uID)
        , mGeometryData(nullptr)
        , mIsVertexShared(false)
    {

    }

    SGMesh::~SGMesh()
    {

    }

    bool SGMesh::init(const ObjectPtr &geometryData, bool isVertexShared)
    {
        mGeometryData = geometryData;
        mIsVertexShared = isVertexShared;
        return true;
    }

    Node::Type SGMesh::getNodeType() const
    {
        return E_NT_MESH;
    }

    NodePtr SGMesh::clone() const
    {
        SGMeshPtr mesh = create(mGeometryData, mIsVertexShared);
        cloneProperties(mesh);
        return mesh;
    }

    void SGMesh::cloneProperties(const NodePtr &node) const
    {

    }

    MaterialPtr SGMesh::getMaterial() const
    {
        return nullptr;
    }

    Renderer::PrimitiveType SGMesh::getPrimitiveType() const
    {
        GeometryDataPtr geometry = smart_pointer_cast<GeometryData>(mGeometryData);
        return geometry->getPrimitiveType();
    }

    VertexDataPtr SGMesh::getVertexData() const
    {
        GeometryDataPtr geometry = smart_pointer_cast<GeometryData>(mGeometryData);
        return geometry->getVertexData();
    }

    IndexDataPtr SGMesh::getIndexData() const
    {
        GeometryDataPtr geometry = smart_pointer_cast<GeometryData>(mGeometryData);
        return geometry->getIndexData();
    }

    bool SGMesh::isIndicesUsed() const
    {
        return true;
    }
}
