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


#include "T3DD3D9HardwareVertexBuffer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D9HardwareVertexBufferPtr D3D9HardwareVertexBuffer::create(
        size_t vertexSize, size_t vertexCount, Usage usage,
        bool useSystemMemory, bool useShadowBuffer)
    {
        D3D9HardwareVertexBufferPtr vb = new D3D9HardwareVertexBuffer(
            vertexSize, vertexCount, usage, useSystemMemory, useShadowBuffer);
        vb->release();
        return vb;
    }

    //--------------------------------------------------------------------------

    D3D9HardwareVertexBuffer::D3D9HardwareVertexBuffer(size_t vertexSize,
        size_t vertexCount, Usage usage, bool useSystemMemory,
        bool useShadowBuffer)
        : HardwareVertexBuffer(vertexSize, vertexCount, usage, useSystemMemory,
            useSystemMemory)
    {
    }

    //--------------------------------------------------------------------------

    D3D9HardwareVertexBuffer::~D3D9HardwareVertexBuffer()
    {
    }

    //--------------------------------------------------------------------------

    size_t D3D9HardwareVertexBuffer::readData(size_t offset, size_t size,
        void *dst)
    {
        return 0;
    }

    //--------------------------------------------------------------------------

    size_t D3D9HardwareVertexBuffer::writeData(size_t offset, size_t size,
        const void *src, bool discardWholeBuffer /* = false */)
    {
        return 0;
    }

    //--------------------------------------------------------------------------

    void *D3D9HardwareVertexBuffer::lockImpl(size_t offset, size_t size,
        LockOptions options)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult D3D9HardwareVertexBuffer::unlockImpl()
    {
        return T3D_OK;
    }
}
