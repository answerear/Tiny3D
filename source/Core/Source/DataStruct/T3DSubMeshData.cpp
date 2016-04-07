

#include "T3DSubMeshData.h"


namespace Tiny3D
{
    SubMeshDataPtr SubMeshData::create()
    {
        SubMeshDataPtr submesh = new SubMeshData();
        submesh->release();
        return submesh;
    }

    SubMeshData::SubMeshData()
    {

    }

    SubMeshData::~SubMeshData()
    {

    }
}
