
#include "Render/T3DHardwareIndexBuffer.h"


namespace Tiny3D
{
    HardwareIndexBuffer::HardwareIndexBuffer(Type indexType, size_t indexCount, HardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer)
        : HardwareBuffer(usage, useSystemMemory, useShadowBuffer)
        , mType(indexType)
        , mIndexCount(indexCount)
    {
        mIndexSize = (indexType == E_IT_16BITS ? sizeof(uint16_t) : sizeof(uint32_t));
        mBufferSize = mIndexSize * mIndexCount;
    }

    HardwareIndexBuffer::~HardwareIndexBuffer()
    {

    }
}