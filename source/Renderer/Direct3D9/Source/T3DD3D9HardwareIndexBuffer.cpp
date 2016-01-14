

#include "T3DD3D9HardwareIndexBuffer.h"
#include "T3DD3D9Renderer.h"
#include "T3DD3D9Mappings.h"


namespace Tiny3D
{
    D3D9HardwareIndexBuffer::D3D9HardwareIndexBuffer(Type indexType, size_t indexCount, HardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer)
        : HardwareIndexBuffer(indexType, indexCount, usage, useSystemMemory, useShadowBuffer)
        , mD3DIndexBuffer(nullptr)
    {
        LPDIRECT3DDEVICE9 pD3DDevice = D3D9_RENDERER.getD3DDevice();

        D3DPOOL d3dpool = (useSystemMemory ? D3DPOOL_SYSTEMMEM : D3DPOOL_DEFAULT);
        HRESULT hr = pD3DDevice->CreateIndexBuffer(
            mBufferSize, 
            D3D9Mappings::get(usage), 
            D3D9Mappings::get(indexType), 
            d3dpool, 
            &mD3DIndexBuffer, 
            nullptr);
        if (FAILED(hr))
        {

        }
        else
        {
            hr = mD3DIndexBuffer->GetDesc(&mBufferDesc);
            if (FAILED(hr))
            {

            }
        }
    }

    D3D9HardwareIndexBuffer::~D3D9HardwareIndexBuffer()
    {
        D3D_SAFE_RELEASE(mD3DIndexBuffer);
    }

    void *D3D9HardwareIndexBuffer::lockImpl(size_t offset, size_t size, LockOptions options)
    {
        if (mD3DIndexBuffer == nullptr)
            return nullptr;

        char *pLockedData = nullptr;
        HRESULT hr = mD3DIndexBuffer->Lock(offset, size, (void **)&pLockedData, D3D9Mappings::get(mUsage, options));
        if (FAILED(hr))
        {
            return nullptr;
        }

        return pLockedData;
    }

    void D3D9HardwareIndexBuffer::unlockImpl()
    {
        if (mD3DIndexBuffer == nullptr)
            return;

        HRESULT hr = mD3DIndexBuffer->Unlock();
        if (FAILED(hr))
        {

        }
    }

    bool D3D9HardwareIndexBuffer::readData(size_t offset, size_t size, void *dst)
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

    bool D3D9HardwareIndexBuffer::writeData(size_t offset, size_t size, const void *src, bool discardWholeBuffer /* = false */)
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
