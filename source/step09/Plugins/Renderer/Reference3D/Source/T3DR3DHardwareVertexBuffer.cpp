/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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


#include "T3DR3DHardwareVertexBuffer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    R3DHardwareVertexBufferPtr R3DHardwareVertexBuffer::create(
        size_t vertexSize, size_t vertexCount, Usage usage, 
        bool useSystemMemory, bool useShadowBuffer)
    {
        R3DHardwareVertexBufferPtr vb = new R3DHardwareVertexBuffer(vertexSize,
            vertexCount, usage, useSystemMemory, useShadowBuffer);
        vb->release();
        return vb;
    }

    //--------------------------------------------------------------------------

    R3DHardwareVertexBuffer::R3DHardwareVertexBuffer(size_t vertexSize, 
        size_t vertexCount, Usage usage, bool useSystemMemory, 
        bool useShadowBuffer)
        : HardwareVertexBuffer(vertexSize, vertexCount, usage, useSystemMemory,
            useSystemMemory)
    {

    }

    //--------------------------------------------------------------------------

    R3DHardwareVertexBuffer::~R3DHardwareVertexBuffer()
    {

    }

    //--------------------------------------------------------------------------

    size_t R3DHardwareVertexBuffer::readData(size_t offset, size_t size,
        void *dst)
    {
        return 0;
    }

    //--------------------------------------------------------------------------

    size_t R3DHardwareVertexBuffer::writeData(size_t offset, size_t size,
        const void *src, bool discardWholeBuffer /* = false */)
    {
        return 0;
    }

    //--------------------------------------------------------------------------

    void *R3DHardwareVertexBuffer::lockImpl(size_t offset, size_t size,
        LockOptions options)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult R3DHardwareVertexBuffer::unlockImpl()
    {
        return T3D_OK;
    }
}
