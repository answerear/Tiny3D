


#include "T3DMeshData.h"
#include "Render/T3DHardwareVertexBuffer.h"
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
        : mVertexData(nullptr)
    {
    }

    MeshData::~MeshData()
    {
        mVertexData = nullptr;
    }

    bool MeshData::init(const VertexAttributes &attributes, const Vertices &vertices, size_t vertexSize)
    {
        bool ret = false;

        VertexDeclarationPtr vertexDecl = T3D_HARDWARE_BUFFER_MGR.createVertexDeclaration();
        auto itr = attributes.begin();
        while (itr != attributes.end())
        {
            auto vertexElement = *itr;
            vertexDecl->addElement(vertexElement);
            ++itr;
        }

        size_t vertexCount = vertices.size() / vertexSize;
        HardwareVertexBufferPtr vertexBuffer = T3D_HARDWARE_BUFFER_MGR.createVertexBuffer(vertexSize, vertexCount, HardwareBuffer::E_HBU_STATIC_WRITE_ONLY, false);

        if (vertexDecl != nullptr && vertexBuffer != nullptr)
        {
            ret = vertexBuffer->writeData(0, vertices.size(), &vertices[0]);

            if (ret)
            {
                mVertexData = VertexData::create(vertexDecl, vertexBuffer);
            }
        }

        return ret;
    }
}
