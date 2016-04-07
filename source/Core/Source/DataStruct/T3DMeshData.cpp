


#include "T3DMeshData.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    MeshDataPtr MeshData::create(Real *vertices, size_t vertexSize, size_t vertexCount)
    {
        MeshDataPtr mesh = new MeshData(vertices, vertexSize, vertexCount);
        mesh->release();
        return mesh;
    }

    MeshData::MeshData(Real *vertices, size_t vertexSize, size_t vertexCount)
    {
        mVertexBuffer = T3D_HARDWARE_BUFFER_MGR.createVertexBuffer(vertexSize, vertexCount, HardwareBuffer::E_HBU_WRITE_ONLY, false);

    }

    MeshData::~MeshData()
    {

    }

}
