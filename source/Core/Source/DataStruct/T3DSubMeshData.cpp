

#include "T3DSubMeshData.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    SubMeshDataPtr SubMeshData::create(const String &materialName, const Indices &indices, bool is16Bits)
    {
        SubMeshDataPtr submesh = new SubMeshData();

        if (submesh != nullptr && submesh->init(materialName, indices, is16Bits))
        {
            submesh->release();
        }
        else
        {
            T3D_SAFE_RELEASE(submesh);
        }

        return submesh;
    }

    SubMeshData::SubMeshData()
        : mIndexBuffer(nullptr)
    {

    }

    SubMeshData::~SubMeshData()
    {
        mIndexBuffer = nullptr;
    }

    bool SubMeshData::init(const String &materialName, const Indices &indices, bool is16Bits)
    {
        bool result = false;

        mMaterialName = materialName;

        HardwareIndexBuffer::Type indexType = HardwareIndexBuffer::E_IT_32BITS;
        
        if (is16Bits)
            indexType = HardwareIndexBuffer::E_IT_16BITS;

        mIndexBuffer = T3D_HARDWARE_BUFFER_MGR.createIndexBuffer(indexType, indices.size(), HardwareBuffer::E_HBU_DYNAMIC, false);

        if (mIndexBuffer != nullptr)
        {
            result = true;
        }

        return result;
    }
}
