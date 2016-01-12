
#include "Render/T3DHardwareIndexBuffer.h"


namespace Tiny3D
{
    HardwareIndexBuffer::HardwareIndexBuffer(Type indexType, size_t indexCount, HardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer)
        : HardwareBuffer(usage, useSystemMemory, useShadowBuffer)
        , mType(indexType)
        , mIndexCount(indexCount)
    {

    }

    HardwareIndexBuffer::~HardwareIndexBuffer()
    {

    }
}