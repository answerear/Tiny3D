

#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    HardwareBufferManagerBase::HardwareBufferManagerBase()
    {

    }

    HardwareBufferManagerBase::~HardwareBufferManagerBase()
    {

    }

    VertexDeclarationPtr HardwareBufferManagerBase::createVertexDeclaration()
    {
        VertexDeclaration *decl = new VertexDeclaration();
        VertexDeclarationPtr ptr(decl);
        decl->release();
        return decl;
    }

    T3D_INIT_SINGLETON(HardwareBufferManager);

    HardwareBufferManager::HardwareBufferManager(HardwareBufferManagerBase *impl)
        : mImpl(impl)
    {

    }

    HardwareBufferManager::~HardwareBufferManager()
    {
        mImpl = nullptr;
    }

    HardwareVertexBufferPtr HardwareBufferManager::createVertexBuffer(size_t vertexSize, size_t vertexCount, HardwareBuffer::Usage usage, bool useShadowBuffer)
    {
        return mImpl->createVertexBuffer(vertexSize, vertexCount, usage, useShadowBuffer);
    }

    HardwareIndexBufferPtr HardwareBufferManager::createIndexBuffer(HardwareIndexBuffer::Type indexType, size_t indexCount, HardwareBuffer::Usage usage, bool useShadowBuffer)
    {
        return mImpl->createIndexBuffer(indexType, indexCount, usage, useShadowBuffer);
    }

    VertexDeclarationPtr HardwareBufferManager::createVertexDeclaration()
    {
        return mImpl->createVertexDeclaration();
    }
}
