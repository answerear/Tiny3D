
#include "T3DD3D9HardwareVertexBuffer.h"
#include "T3DD3D9Renderer.h"
#include "T3DD3D9Mappings.h"


namespace Tiny3D
{
    D3D9HardwareVertexBuffer::D3D9HardwareVertexBuffer(size_t vertexSize, size_t vertexCount, Usage usage, bool useSystemMemory, bool useShadowBuffer)
        : HardwareVertexBuffer(vertexSize, vertexCount, usage, useSystemMemory, useShadowBuffer)
        , mD3DVertexBuffer(nullptr)
    {
        LPDIRECT3DDEVICE9 pD3DDevice = D3D9_RENDERER.getD3DDevice();

        D3DPOOL d3dpool = (useSystemMemory ? D3DPOOL_SYSTEMMEM : D3DPOOL_DEFAULT);
        HRESULT hr = pD3DDevice->CreateVertexBuffer(
            mBufferSize, 
            D3D9Mappings::get(usage), 
            0, 
            d3dpool, 
            &mD3DVertexBuffer, 
            nullptr);
        if (FAILED(hr))
        {

        }
        else
        {
            hr = mD3DVertexBuffer->GetDesc(&mBufferDesc);
            if (FAILED(hr))
            {

            }
        }
    }

    D3D9HardwareVertexBuffer::~D3D9HardwareVertexBuffer()
    {
        D3D_SAFE_RELEASE(mD3DVertexBuffer);
    }

    void *D3D9HardwareVertexBuffer::lockImpl(size_t offset, size_t size, LockOptions options)
    {
        if (mD3DVertexBuffer == nullptr)
            return nullptr;

        char *pLockedData = nullptr;
        HRESULT hr = mD3DVertexBuffer->Lock(offset, size, (void **)&pLockedData, D3D9Mappings::get(mUsage, options));
        if (FAILED(hr))
        {
            return nullptr;
        }

        return pLockedData;
    }

    void D3D9HardwareVertexBuffer::unlockImpl()
    {
        if (mD3DVertexBuffer == nullptr)
            return;

        HRESULT hr = mD3DVertexBuffer->Unlock();
        if (FAILED(hr))
        {

        }
    }

    bool D3D9HardwareVertexBuffer::readData(size_t offset, size_t size, void *dst)
    {
        bool ret = false;
        void *src = lock(offset, size, HardwareBuffer::E_HBL_READ_ONLY);
        if (src != nullptr)
        {
            memcpy(dst, src, size);
            ret = true;
        }
        unlock();
        return ret;
    }

    bool D3D9HardwareVertexBuffer::writeData(size_t offset, size_t size, const void *src, bool discardWholeBuffer /* = false */)
    {
        bool ret = false;
        void *dst = lock(offset, size, discardWholeBuffer ? HardwareBuffer::E_HBL_DISCARD : HardwareBuffer::E_HBL_NORMAL);
        if (dst != nullptr)
        {
            memcpy(dst, src, size);
            ret = true;
        }
        unlock();
        return ret;
    }
}