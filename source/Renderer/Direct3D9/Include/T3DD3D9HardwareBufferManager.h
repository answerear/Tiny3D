

#ifndef __T3D_D3D9_HARDWARE_BUFFER_MANAGER_H__
#define __T3D_D3D9_HARDWARE_BUFFER_MANAGER_H__


#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    class D3D9HardwareBufferManager : public HardwareBufferManagerBase
    {
    public:
        D3D9HardwareBufferManager();
        virtual ~D3D9HardwareBufferManager();

        virtual HardwareVertexBufferPtr createVertexBuffer(size_t vertexSize, size_t vertexCount, HardwareBuffer::Usage usage, bool useShadowBuffer) override;
        virtual HardwareIndexBufferPtr createIndexBuffer(HardwareIndexBuffer::Type indexType, size_t indexCount, HardwareBuffer::Usage usage, bool useShadowBuffer) override;

        virtual VertexDeclarationPtr createVertexDeclaration() override;
    };
}



#endif
