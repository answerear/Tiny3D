

#include "T3DSubMeshData.h"
#include "Render/T3DHardwareIndexBuffer.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    SubMeshDataPtr SubMeshData::create(Renderer::PrimitiveType primitiveType, const String &materialName, const Indices16 &indices)
    {
        SubMeshDataPtr submesh = new SubMeshData();

        if (submesh != nullptr && submesh->init(primitiveType, materialName, indices))
        {
            submesh->release();
        }
        else
        {
            T3D_SAFE_RELEASE(submesh);
        }

        return submesh;
    }

    SubMeshDataPtr SubMeshData::create(Renderer::PrimitiveType primitiveType, const String &materialName, const Indices32 &indices)
    {
        SubMeshDataPtr submesh = new SubMeshData();

        if (submesh != nullptr && submesh->init(primitiveType, materialName, indices))
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

    bool SubMeshData::init(Renderer::PrimitiveType primitiveType, const String &materialName, const Indices16 &indices)
    {
        bool ret = false;

        mPrimitiveType = primitiveType;
        mMaterialName = materialName;

        HardwareIndexBufferPtr indexBuffer = T3D_HARDWARE_BUFFER_MGR.createIndexBuffer(HardwareIndexBuffer::E_IT_16BITS, indices.size(), HardwareBuffer::E_HBU_STATIC_WRITE_ONLY, false);

        if (indexBuffer != nullptr)
        {
            size_t indexSize = sizeof(uint16_t) * indices.size();
            ret = indexBuffer->writeData(0, indexSize, &indices[0]);
            if (ret)
            {
                mIndexData = IndexData::create(indexBuffer);
            }
        }

        return ret;
    }

    bool SubMeshData::init(Renderer::PrimitiveType primitiveType, const String &materialName, const Indices32 &indices)
    {
        bool ret = false;

        mPrimitiveType = primitiveType;
        mMaterialName = materialName;

        HardwareIndexBufferPtr indexBuffer = T3D_HARDWARE_BUFFER_MGR.createIndexBuffer(HardwareIndexBuffer::E_IT_32BITS, indices.size(), HardwareBuffer::E_HBU_STATIC_WRITE_ONLY, false);

        if (indexBuffer != nullptr)
        {
            size_t indexSize = sizeof(uint32_t) * indices.size();
            ret = indexBuffer->writeData(0, indexSize, &indices[0]);
            if (ret)
            {
                mIndexData = IndexData::create(indexBuffer);
            }
        }

        return ret;
    }
}
