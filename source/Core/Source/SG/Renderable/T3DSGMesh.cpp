

#include "SG/Renderable/T3DSGMesh.h"
#include "Resource/T3DModel.h"
#include "Resource/T3DModelManager.h"
#include "DataStruct/T3DMeshData.h"
#include "DataStruct/T3DSubMeshData.h"


namespace Tiny3D
{
    SGMeshPtr SGMesh::create(const ModelPtr &model, int32_t index, uint32_t uID /* = E_NID_AUTOMATIC */)
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
        , mModel(nullptr)
        , mIndex(0)
    {

    }

    SGMesh::~SGMesh()
    {

    }

    bool SGMesh::init(const ModelPtr &model, int32_t index)
    {
        mModel = model;
        mIndex = index;
        return true;
    }

    Node::Type SGMesh::getNodeType() const
    {
        return E_NT_MESH;
    }

    NodePtr SGMesh::clone() const
    {
        SGMeshPtr mesh = create(mModel, mIndex);
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
        const Model::SubMeshDataList &submeshes = mModel->getSubMeshDataList();
        SubMeshDataPtr submeshdata = smart_pointer_cast<SubMeshData>(submeshes[mIndex]);
        return submeshdata->getPrimitiveType();
    }

    VertexDataPtr SGMesh::getVertexData() const
    {
        const MeshDataPtr &meshdata = smart_pointer_cast<MeshData>(mModel->getMeshData());
        return meshdata->getVertexData();
    }

    IndexDataPtr SGMesh::getIndexData() const
    {
        const Model::SubMeshDataList &submeshes = mModel->getSubMeshDataList();
        SubMeshDataPtr submeshdata = smart_pointer_cast<SubMeshData>(submeshes[mIndex]);
        return submeshdata->getIndexData();
    }

    bool SGMesh::isIndicesUsed() const
    {
        return true;
    }
}
