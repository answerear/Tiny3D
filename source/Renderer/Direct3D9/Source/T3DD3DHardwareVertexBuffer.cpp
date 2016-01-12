
#include "T3DD3DHardwareVertexBuffer.h"


namespace Tiny3D
{
    D3D9HardwareVertexBuffer::D3D9HardwareVertexBuffer(size_t vertexSize, size_t vertexCount, Usage usage, bool useSystemMemory, bool useShadowBuffer)
        : HardwareVertexBuffer(vertexSize, vertexCount, usage, useSystemMemory, useShadowBuffer)
    {

    }

    D3D9HardwareVertexBuffer::~D3D9HardwareVertexBuffer()
    {

    }
}