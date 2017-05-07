

#include "SG/Renderable/T3DSGMesh.h"
#include "Resource/T3DModel.h"
#include "Resource/T3DModelManager.h"
#include "Resource/T3DMaterial.h"
#include "Resource/T3DMaterialManager.h"
#include "DataStruct/T3DGeometryData.h"


namespace Tiny3D
{
    SGMeshPtr SGMesh::create(ModelPtr model, int32_t index, uint32_t uID /* = E_NID_AUTOMATIC */)
    {
        SGMeshPtr mesh = new SGMesh(uID);
        if (mesh != nullptr && mesh->init(model, index))
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
        , mMeshIndex(-1)
        , mModel(nullptr)
        , mMaterial(nullptr)
        , mGeometryData(nullptr)
    {

    }

    SGMesh::~SGMesh()
    {
        mModel = nullptr;
        mMaterial = nullptr;
    }

    bool SGMesh::init(ModelPtr model, int32_t index)
    {
        mModel = model;
        mMeshIndex = index;

        const Model::GeometryDataList &geometries = mModel->getGeometryDataList();
        if (mMeshIndex < 0 || mMeshIndex >= geometries.size())
            return false;

        GeometryDataPtr geometry = smart_pointer_cast<GeometryData>(geometries[mMeshIndex]);
        mMaterial = T3D_MATERIAL_MGR.loadMaterial(geometry->getMaterialName(), Material::E_MT_DEFAULT);
        mGeometryData = geometry;

        return true;
    }

    Node::Type SGMesh::getNodeType() const
    {
        return E_NT_MESH;
    }

    NodePtr SGMesh::clone() const
    {
        SGMeshPtr mesh = create(mModel, mMeshIndex);
        cloneProperties(mesh);
        return mesh;
    }

    void SGMesh::cloneProperties(const NodePtr &node) const
    {

    }

    MaterialPtr SGMesh::getMaterial() const
    {
        return mMaterial;
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

    void SGMesh::updateTransform()
    {
        updateSkin();
    }

    void SGMesh::updateSkin()
    {
//         GeometryDataPtr geometry = smart_pointer_cast<GeometryData>(mGeometryData);
//         VertexDataPtr vertexData = geometry->getVertexData();
//         mModel->getSkinData();
//         HardwareVertexBufferPtr vb = vertexData->getVertexBuffer(0);
//         vb->writeData(0, sizeof(BoneVertex) * vertices.size(), &vertices[0]);
    }
}
