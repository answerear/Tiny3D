


#include "T3DMeshData.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    MeshDataPtr MeshData::create()
    {
        MeshDataPtr mesh = new MeshData();
        mesh->release();
        return mesh;
    }

    MeshData::MeshData()
        : mVertices(nullptr)
        , mVertexSize(0)
        , mVertexCount(0)
    {

    }

    MeshData::~MeshData()
    {
        delete []mVertices;
        mVertices = nullptr;
    }
}
