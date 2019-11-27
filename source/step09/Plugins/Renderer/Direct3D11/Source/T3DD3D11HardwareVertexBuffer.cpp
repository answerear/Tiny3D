/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#include "T3DD3D11HardwareVertexBuffer.h"
#include "T3DD3D11Renderer.h"
#include "T3DD3D11Mappings.h"
#include "T3DD3D11Error.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11HardwareVertexBufferPtr D3D11HardwareVertexBuffer::create(
        size_t vertexSize, size_t vertexCount, const void *vertices, 
        Usage usage, bool useSystemMemory, bool useShadowBuffer)
    {
        D3D11HardwareVertexBufferPtr vb = new D3D11HardwareVertexBuffer(
            vertexSize, vertexCount, usage, useSystemMemory, useShadowBuffer);
        vb->release();
        if (vb->init(vertices) != T3D_OK)
        {
            vb = nullptr;
        }
        return vb;
    }

    //--------------------------------------------------------------------------

    D3D11HardwareVertexBuffer::D3D11HardwareVertexBuffer(size_t vertexSize,
        size_t vertexCount, Usage usage, bool useSystemMemory,
        bool useShadowBuffer)
        : HardwareVertexBuffer(vertexSize, vertexCount, usage, useSystemMemory,
            useSystemMemory)
        , mD3DBuffer(nullptr)
    {
    }

    //--------------------------------------------------------------------------

    D3D11HardwareVertexBuffer::~D3D11HardwareVertexBuffer()
    {
        D3D_SAFE_RELEASE(mD3DBuffer);
    }

    //--------------------------------------------------------------------------

    TResult D3D11HardwareVertexBuffer::init(const void *vertices)
    {
        TResult ret = T3D_ERR_FAIL;

        do 
        {
            // 初始化数据
            D3D11_SUBRESOURCE_DATA data;
            memset(&data, 0, sizeof(data));

            if (vertices != nullptr)
            {
                data.pSysMem = vertices;
            }            

            D3D11_BUFFER_DESC desc;
            memset(&desc, 0, sizeof(desc));
            desc.Usage;
            desc.ByteWidth = (uint32_t)mVertexSize;
            desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            desc.CPUAccessFlags = 0;

            ID3D11Device *pD3DDevice = D3D11_RENDERER.getD3DDevice();
            HRESULT hr = S_OK;
            hr = pD3DDevice->CreateBuffer(&desc, &data, &mD3DBuffer);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_CREATE_BUFFER;
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create vertex buffer \
                    failed ! DX ERROR [%d]", hr);
                break;
            }

            ret = T3D_OK;
        } while (0);
        return ret;
    }

    //--------------------------------------------------------------------------

    size_t D3D11HardwareVertexBuffer::readData(size_t offset, size_t size,
        void *dst)
    {
        size_t bytesOfRead = 0;

        do 
        {
            void *src = lock(offset, size, LockOptions::READ_ONLY);

            if (src == nullptr)
            {
                break;
            }

            memcpy(dst, src, size);
            bytesOfRead = size;

            unlock();
        } while (0);

        return bytesOfRead;
    }

    //--------------------------------------------------------------------------

    size_t D3D11HardwareVertexBuffer::writeData(size_t offset, size_t size,
        const void *src, bool discardWholeBuffer /* = false */)
    {
        size_t bytesOfWritten = 0;

        do 
        {
            void *dst = lock(offset, size, discardWholeBuffer 
                ? LockOptions::DISCARD : LockOptions::NORMAL);

            if (dst == nullptr)
            {
                break;
            }

            memcpy(dst, src, size);
            bytesOfWritten = size;

            unlock();
        } while (0);

        return bytesOfWritten;
    }

    //--------------------------------------------------------------------------

    void *D3D11HardwareVertexBuffer::lockImpl(size_t offset, size_t size,
        LockOptions options)
    {
        char *pLockedData = nullptr;

        do 
        {
            
        } while (0);

        return pLockedData;
    }

    //--------------------------------------------------------------------------

    TResult D3D11HardwareVertexBuffer::unlockImpl()
    {
        TResult ret = T3D_OK;

        do 
        {
            
        } while (0);

        return ret;
    }
}
