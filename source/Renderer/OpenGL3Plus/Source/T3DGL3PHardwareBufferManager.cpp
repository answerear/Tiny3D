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

#include "T3DGL3PHardwareBufferManager.h"
#include "T3DGL3PHardwareVertexBuffer.h"
#include "T3DGL3PHardwareIndexBuffer.h"
#include "T3DGL3PHardwarePixelBuffer.h"
#include "T3DGL3pVertexDeclaration.h"

namespace Tiny3D
{
    GL3PHardwareBufferManager::GL3PHardwareBufferManager()
        : HardwareBufferManagerBase()
    {

    }

    GL3PHardwareBufferManager::~GL3PHardwareBufferManager()
    {

    }

    HardwareVertexBufferPtr GL3PHardwareBufferManager::createVertexBuffer(
        size_t vertexSize, size_t vertexCount, HardwareBuffer::Usage usage,
        bool useShadowBuffer)
    {
        GL3PHardwareVertexBuffer *vertexBuffer = new GL3PHardwareVertexBuffer(
            vertexSize, vertexCount, usage, false, useShadowBuffer);

        HardwareVertexBufferPtr ptr(vertexBuffer);
        mVertexBuffers.insert(ptr);
        vertexBuffer->release();

        return ptr;
    }

    HardwareIndexBufferPtr GL3PHardwareBufferManager::createIndexBuffer(
        HardwareIndexBuffer::Type indexType, size_t indexCount,
        HardwareBuffer::Usage usage, bool useShadowBuffer)
    {
        GL3PHardwareIndexBuffer *indexBuffer = new GL3PHardwareIndexBuffer(
            indexType, indexCount, usage, false, useShadowBuffer);

        HardwareIndexBufferPtr ptr(indexBuffer);
        mIndexBuffers.insert(ptr);
        indexBuffer->release();

        return ptr;
    }

    HardwarePixelBufferPtr GL3PHardwareBufferManager::createPixelBuffer(
        uint32_t width, uint32_t height, PixelFormat format,
        HardwareBuffer::Usage usage, bool useShadowBuffer)
    {
        GL3PHardwarePixelBuffer *pixelBuffer = new GL3PHardwarePixelBuffer(
            width, height, format, usage, false, useShadowBuffer);

        HardwarePixelBufferPtr ptr(pixelBuffer);
        mPixelBuffers.insert(ptr);
        pixelBuffer->release();

        return ptr;
    }

    VertexDeclarationPtr GL3PHardwareBufferManager::createVertexDeclaration()
    {
        GL3PVertexDeclaration *decl = new GL3PVertexDeclaration();
        VertexDeclarationPtr ptr(decl);
        decl->release();

        return ptr;
    }
}