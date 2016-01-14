

#include "T3DD3D9HardwareBufferManager.h"
#include "T3DD3D9HardwareVertexBuffer.h"
#include "T3DD3D9HardwareIndexBuffer.h"
#include "T3DD3D9VertexDeclaration.h"


namespace Tiny3D
{
    D3D9HardwareBufferManager::D3D9HardwareBufferManager()
        : HardwareBufferManagerBase()
    {

    }

    D3D9HardwareBufferManager::~D3D9HardwareBufferManager()
    {

    }

    HardwareVertexBufferPtr D3D9HardwareBufferManager::createVertexBuffer(size_t vertexSize, size_t vertexCount, HardwareBuffer::Usage usage, bool useShadowBuffer)
    {
        D3D9HardwareVertexBuffer *vertexBuffer = new D3D9HardwareVertexBuffer(vertexSize,
            vertexCount, usage, false, useShadowBuffer);

        HardwareVertexBufferPtr ptr(vertexBuffer);
        mVertexBuffers.insert(ptr);
        vertexBuffer->release();

        return ptr;
    }

    HardwareIndexBufferPtr D3D9HardwareBufferManager::createIndexBuffer(HardwareIndexBuffer::Type indexType, size_t indexCount, HardwareBuffer::Usage usage, bool useShadowBuffer)
    {
        D3D9HardwareIndexBuffer *indexBuffer = new D3D9HardwareIndexBuffer(indexType,
            indexCount, usage, false, useShadowBuffer);

        HardwareIndexBufferPtr ptr(indexBuffer);
        mIndexBuffers.insert(ptr);
        indexBuffer->release();

        return ptr;
    }

    VertexDeclarationPtr D3D9HardwareBufferManager::createVertexDeclaration()
    {
        D3D9VertexDeclaration *decl = new D3D9VertexDeclaration();
        VertexDeclarationPtr ptr(decl);
        decl->release();

        return ptr;
    }
}