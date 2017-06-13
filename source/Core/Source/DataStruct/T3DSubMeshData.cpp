
#include "T3DSubMeshData.h"

namespace Tiny3D
{
    SubMeshDataPtr SubMeshData::create(const String &name, const String &materialName, Renderer::PrimitiveType priType, bool is16Bits, size_t indexCount)
    {
        SubMeshDataPtr data = new SubMeshData(name, materialName, priType, is16Bits, indexCount);

        if (data != nullptr)
        {
            data->release();
        }

        return data;
    }

    SubMeshData::SubMeshData(const String &name, const String &materialName, Renderer::PrimitiveType priType, bool is16Bits, size_t indexCount)
        : mName(name)
        , mMaterialName(materialName)
        , mPrimitiveType(priType)
        , mIs16Bits(is16Bits)
        , mIndices(is16Bits ? indexCount * sizeof(uint16_t) : indexCount * sizeof(uint32_t))
    {

    }
}
