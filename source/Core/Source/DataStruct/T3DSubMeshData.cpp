

#include "T3DSubMeshData.h"
#include "Render/T3DHardwareIndexBuffer.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    SubMeshDataPtr SubMeshData::create(Renderer::PrimitiveType primitiveType, const String &materialName, const Indices &indices, bool is16Bits)
    {
        SubMeshDataPtr submesh = new SubMeshData();

        if (submesh != nullptr && submesh->init(primitiveType, materialName, indices, is16Bits))
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
        : mIndexData(nullptr)
    {

    }

    SubMeshData::~SubMeshData()
    {
        mIndexData = nullptr;
    }

    bool SubMeshData::init(Renderer::PrimitiveType primitiveType, const String &materialName, const Indices &indices, bool is16Bits)
    {
        bool ret = false;

        mPrimitiveType = primitiveType;
        mMaterialName = materialName;

        HardwareIndexBuffer::Type indexType = HardwareIndexBuffer::E_IT_32BITS;
        
        if (is16Bits)
            indexType = HardwareIndexBuffer::E_IT_16BITS;

        HardwareIndexBufferPtr indexBuffer = T3D_HARDWARE_BUFFER_MGR.createIndexBuffer(indexType, indices.size(), HardwareBuffer::E_HBU_STATIC_WRITE_ONLY, false);

        if (indexBuffer != nullptr)
        {
            size_t indexSize = sizeof(uint32_t) * indices.size();
            if (is16Bits)
                indexSize = sizeof(uint16_t) * indices.size();
            ret = indexBuffer->writeData(0, indexSize, &indices[0]);
            if (ret)
            {
                mIndexData = IndexData::create(indexBuffer);
            }
        }

        return ret;
    }
}
