/***************************************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
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
 **************************************************************************************************/

#include "T3DGL3PHardwareVertexBuffer.h"

namespace Tiny3D
{
    GL3PHardwareVertexBuffer::GL3PHardwareVertexBuffer(size_t vertexSize, size_t vertexCount, Usage usage, bool useSystemMemory, bool useShadowBuffer)
        : HardwareVertexBuffer(vertexSize, vertexCount, usage, useSystemMemory, useShadowBuffer)
    {
    }

    GL3PHardwareVertexBuffer::~GL3PHardwareVertexBuffer()
    {
    }

    void *GL3PHardwareVertexBuffer::lockImpl(size_t offset, size_t size, LockOptions options)
    {
        char *pLockedData = nullptr;
        return pLockedData;
    }

    void GL3PHardwareVertexBuffer::unlockImpl()
    {
    }

    bool GL3PHardwareVertexBuffer::readData(size_t offset, size_t size, void *dst)
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

    bool GL3PHardwareVertexBuffer::writeData(size_t offset, size_t size, const void *src, bool discardWholeBuffer /* = false */)
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