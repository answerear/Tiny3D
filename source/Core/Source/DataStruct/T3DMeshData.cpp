


#include "T3DMeshData.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    MeshDataPtr MeshData::create(const VertexAttributes &attributes, const Vertices &vertices, size_t vertexSize)
    {
        MeshDataPtr mesh = new MeshData();

        if (mesh != nullptr && mesh->init(attributes, vertices, vertexSize))
        {
            mesh->release();
        }
        else
        {
            T3D_SAFE_RELEASE(mesh);
        }

        return mesh;
    }

    MeshData::MeshData()
        : mVertexDecl(nullptr)
        , mVertexBuffer(nullptr)
    {
    }

    MeshData::~MeshData()
    {
        mVertexDecl = nullptr;
        mVertexDecl = nullptr;
    }

    bool MeshData::init(const VertexAttributes &attributes, const Vertices &vertices, size_t vertexSize)
    {
        bool result = false;

        mVertexDecl = T3D_HARDWARE_BUFFER_MGR.createVertexDeclaration();

        size_t vertexCount = vertices.size() / vertexSize;
        mVertexBuffer = T3D_HARDWARE_BUFFER_MGR.createVertexBuffer(vertexSize, vertexCount, HardwareBuffer::E_HBU_DYNAMIC, false);

        if (mVertexDecl != nullptr && mVertexBuffer != nullptr)
        {
            result = true;
        }

        return result;
    }
}
