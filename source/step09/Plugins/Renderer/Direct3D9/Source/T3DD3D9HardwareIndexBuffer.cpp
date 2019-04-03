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


#include "T3DD3D9HardwareIndexBuffer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D9HardwareIndexBufferPtr D3D9HardwareIndexBuffer::create(
        Type indexType, size_t indexCount, HardwareBuffer::Usage usage,
        bool useSystemMemory, bool useShadowBuffer)
    {
        D3D9HardwareIndexBufferPtr ibo = new D3D9HardwareIndexBuffer(indexType, 
            indexCount, usage, useSystemMemory, useShadowBuffer);
        ibo->release();
        return ibo;
    }

    //--------------------------------------------------------------------------

    D3D9HardwareIndexBuffer::D3D9HardwareIndexBuffer(Type indexType,
        size_t indexCount, HardwareBuffer::Usage usage, bool useSystemMemory,
        bool useShadowBuffer)
        : HardwareIndexBuffer(indexType, indexCount, usage, useSystemMemory, 
            useShadowBuffer)
    {

    }

    //--------------------------------------------------------------------------

    D3D9HardwareIndexBuffer::~D3D9HardwareIndexBuffer()
    {

    }

    //--------------------------------------------------------------------------

    TResult D3D9HardwareIndexBuffer::init()
    {
        TResult ret = T3D_OK;

        do 
        {

        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    size_t D3D9HardwareIndexBuffer::readData(size_t offset, size_t size, 
        void *dst)
    {
        return 0;
    }

    //--------------------------------------------------------------------------

    size_t D3D9HardwareIndexBuffer::writeData(size_t offset, size_t size,
        const void *src, bool discardWholeBuffer /* = false */)
    {
        return 0;
    }

    //--------------------------------------------------------------------------

    void *D3D9HardwareIndexBuffer::lockImpl(size_t offset, size_t size,
        LockOptions options)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult D3D9HardwareIndexBuffer::unlockImpl()
    {
        return T3D_OK;
    }
}
