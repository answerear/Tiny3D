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


#include "T3DD3D11HardwareConstantBuffer.h"
#include "T3DD3D11Mappings.h"
#include "T3DD3D11Renderer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11HardwareConstantBufferPtr D3D11HardwareConstantBuffer::create(
        size_t bufSize, const void *buffer, Usage usage, bool useSystemMemory, 
        bool useShadowBuffer)
    {
        D3D11HardwareConstantBufferPtr cbo = new D3D11HardwareConstantBuffer(
            bufSize, usage, useSystemMemory, useShadowBuffer);
        cbo->release();

        if (cbo->init(buffer) != T3D_OK)
        {
            cbo = nullptr;
        }

        return cbo;
    }

    //--------------------------------------------------------------------------

    D3D11HardwareConstantBuffer::D3D11HardwareConstantBuffer(
        size_t bufSize, HardwareBuffer::Usage usage, bool useSystemMemory,
        bool useShadowBuffer)
        : HardwareConstantBuffer(bufSize, usage, useSystemMemory,
            useShadowBuffer)
    {

    }

    //--------------------------------------------------------------------------

    D3D11HardwareConstantBuffer::~D3D11HardwareConstantBuffer()
    {
    }

    //--------------------------------------------------------------------------

    TResult D3D11HardwareConstantBuffer::init(const void *indices)
    {
        TResult ret = T3D_OK;

        do 
        {
            
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    size_t D3D11HardwareConstantBuffer::readData(size_t offset, size_t size, 
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

    size_t D3D11HardwareConstantBuffer::writeData(size_t offset, size_t size,
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

    void *D3D11HardwareConstantBuffer::lockImpl(size_t offset, size_t size,
        LockOptions options)
    {
        char *pLockedData = nullptr;

        do 
        {
            
        } while (0);

        return pLockedData;
    }

    //--------------------------------------------------------------------------

    TResult D3D11HardwareConstantBuffer::unlockImpl()
    {
        TResult ret = T3D_OK;

        do 
        {
            
        } while (0);

        return ret;
    }
}
