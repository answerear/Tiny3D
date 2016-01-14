

#ifndef __T3D_D3D9_HARDWARE_VERTEX_BUFFER_H__
#define __T3D_D3D9_HARDWARE_VERTEX_BUFFER_H__


#include "T3DD3D9Prerequisites.h"
#include "Render/T3DHardwareVertexBuffer.h"


namespace Tiny3D
{
    class D3D9HardwareVertexBuffer : public HardwareVertexBuffer
    {
    public:
        D3D9HardwareVertexBuffer(size_t vertexSize, size_t vertexCount, Usage usage, bool useSystemMemory, bool useShadowBuffer);
        virtual ~D3D9HardwareVertexBuffer();

        virtual void *lockImpl(size_t offset, size_t size, LockOptions options) override;
        virtual void unlockImpl() override;

        virtual bool readData(size_t offset, size_t size, void *dst) override;
        virtual bool writeData(size_t offset, size_t size, const void *src, bool discardWholeBuffer /* = false */) override;

        LPDIRECT3DVERTEXBUFFER9 getD3DVertexBuffer()    { return mD3DVertexBuffer; }

    protected:
        LPDIRECT3DVERTEXBUFFER9     mD3DVertexBuffer;
        D3DVERTEXBUFFER_DESC        mBufferDesc;
    };
}


#endif  /*__T3D_D3D9_HARDWARE_VERTEX_BUFFER_H__*/
