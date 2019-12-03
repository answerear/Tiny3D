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


#include "T3DD3D11HardwareIndexBuffer.h"
#include "T3DD3D11Mappings.h"
#include "T3DD3D11Renderer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11HardwareIndexBufferPtr D3D11HardwareIndexBuffer::create(
        Type indexType, size_t indexCount, const void *indices, 
        HardwareBuffer::Usage usage, uint32_t mode)
    {
        D3D11HardwareIndexBufferPtr ibo = new D3D11HardwareIndexBuffer(
            indexType, indexCount, usage, mode);
        ibo->release();

        if (ibo->init(indices) != T3D_OK)
        {
            ibo = nullptr;
        }

        return ibo;
    }

    //--------------------------------------------------------------------------

    D3D11HardwareIndexBuffer::D3D11HardwareIndexBuffer(Type indexType,
        size_t indexCount, HardwareBuffer::Usage usage, uint32_t mode)
        : HardwareIndexBuffer(indexType, indexCount, usage, mode)
    {

    }

    //--------------------------------------------------------------------------

    D3D11HardwareIndexBuffer::~D3D11HardwareIndexBuffer()
    {
    }

    //--------------------------------------------------------------------------

    TResult D3D11HardwareIndexBuffer::init(const void *indices)
    {
        TResult ret = T3D_OK;

        do 
        {
            
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    size_t D3D11HardwareIndexBuffer::readData(size_t offset, size_t size, 
        void *dst)
    {
        size_t bytesOfRead = 0;

        do 
        {
            void *src = lock(offset, size, LockOptions::READ);
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

    size_t D3D11HardwareIndexBuffer::writeData(size_t offset, size_t size,
        const void *src, bool discardWholeBuffer /* = false */)
    {
        size_t bytesOfWritten = 0;

        do 
        {
            void *dst = lock(offset, size, discardWholeBuffer 
                ? LockOptions::WRITE_DISCARD : LockOptions::WRITE);
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

    void *D3D11HardwareIndexBuffer::lockImpl(size_t offset, size_t size,
        LockOptions options)
    {
        char *pLockedData = nullptr;

        do 
        {
            
        } while (0);

        return pLockedData;
    }

    //--------------------------------------------------------------------------

    TResult D3D11HardwareIndexBuffer::unlockImpl()
    {
        TResult ret = T3D_OK;

        do 
        {
            
        } while (0);

        return ret;
    }
}
