
#include "T3DMeshData.h"


namespace Tiny3D
{
    VertexBufferPtr VertexBuffer::create()
    {
        VertexBufferPtr vb = new VertexBuffer();

        if (vb != nullptr)
        {
            vb->release();
        }

        return vb;
    }

    VertexBuffer::VertexBuffer()
        : mAttributes()
        , mVertices()
        , mVertexSize(0)
    {

    }

    MeshDataPtr MeshData::create()
    {
        MeshDataPtr data = new MeshData();

        if (data != nullptr)
        {
            data->release();
        }

        return data;
    }

    MeshData::MeshData()
        : mBuffers()
        , mSubMeshes()
    {

    }
}
