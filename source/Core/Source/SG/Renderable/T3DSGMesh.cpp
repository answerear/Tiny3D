

#include "SG/Renderable/T3DSGMesh.h"
#include "Resource/T3DModel.h"
#include "Resource/T3DModelManager.h"


namespace Tiny3D
{
    SGMeshPtr SGMesh::create(const ModelPtr &model, uint32_t uID /* = E_NID_AUTOMATIC */)
    {
        SGMeshPtr mesh = new SGMesh(uID);
        if (mesh != nullptr && mesh->init(model))
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
    {

    }

    SGMesh::~SGMesh()
    {

    }

    bool SGMesh::init(const ModelPtr &model)
    {
        return true;
    }

    Node::Type SGMesh::getNodeType() const
    {
        return E_NT_MESH;
    }

    NodePtr SGMesh::clone() const
    {
        SGMeshPtr mesh = create(mModel);
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
        return Renderer::E_PT_TRIANGLE_LIST;
    }

    VertexDataPtr SGMesh::getVertexData() const
    {
        return nullptr;
    }

    IndexDataPtr SGMesh::getIndexData() const
    {
        return nullptr;
    }

    bool SGMesh::isIndicesUsed() const
    {
        return true;
    }
}
