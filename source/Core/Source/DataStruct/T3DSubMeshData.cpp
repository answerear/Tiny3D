

#include "T3DSubMeshData.h"
#include "Render/T3DHardwareIndexBuffer.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    SubMeshDataPtr SubMeshData::create(Renderer::PrimitiveType primitiveType, const String &materialName, const Indices &indices, bool is16bits)
    {
        SubMeshDataPtr submesh = new SubMeshData();

        if (submesh != nullptr && submesh->init(primitiveType, materialName, indices, is16bits))
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

    bool SubMeshData::init(Renderer::PrimitiveType primitiveType, const String &materialName, const Indices &indices, bool is16bits)
    {
        bool ret = false;

        mPrimitiveType = primitiveType;
        mMaterialName = materialName;

        HardwareIndexBuffer::Type indexType = HardwareIndexBuffer::E_IT_32BITS;
        size_t indexCount = indices.size() / sizeof(uint32_t);

        if (is16bits)
        {
            indexType = HardwareIndexBuffer::E_IT_16BITS;
            indexCount = indices.size() / sizeof(uint16_t);
        }

        HardwareIndexBufferPtr indexBuffer = T3D_HARDWARE_BUFFER_MGR.createIndexBuffer(indexType, indexCount, HardwareBuffer::E_HBU_STATIC_WRITE_ONLY, false);

        if (indexBuffer != nullptr)
        {
            size_t indexSize = indices.size();
            ret = indexBuffer->writeData(0, indexSize, &indices[0]);
            if (ret)
            {
                mIndexData = IndexData::create(indexBuffer);
            }
        }

        return ret;
    }
}
