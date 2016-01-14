

#ifndef __T3D_D3D9_INDEX_BUFFER_H__
#define __T3D_D3D9_INDEX_BUFFER_H__


#include "T3DD3D9Prerequisites.h"
#include "Render/T3DHardwareIndexBuffer.h"


namespace Tiny3D
{
    class D3D9HardwareIndexBuffer : public HardwareIndexBuffer
    {
    public:
        D3D9HardwareIndexBuffer(Type indexType, size_t indexCount, HardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer);
        virtual ~D3D9HardwareIndexBuffer();

        virtual void *lockImpl(size_t offset, size_t size, LockOptions options) override;
        virtual void unlockImpl() override;

        virtual bool readData(size_t offset, size_t size, void *dst) override;
        virtual bool writeData(size_t offset, size_t size, const void *src, bool discardWholeBuffer /* = false */) override;

        LPDIRECT3DINDEXBUFFER9 getD3DIndexBuffer()  { return mD3DIndexBuffer; }

    protected:
        LPDIRECT3DINDEXBUFFER9  mD3DIndexBuffer;
        D3DINDEXBUFFER_DESC     mBufferDesc;
    };
}


#endif  /*__T3D_D3D9_INDEX_BUFFER_H__*/
