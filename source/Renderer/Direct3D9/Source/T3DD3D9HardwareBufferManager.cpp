/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Aaron Wong
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

#include "T3DD3D9HardwareBufferManager.h"
#include "T3DD3D9HardwareVertexBuffer.h"
#include "T3DD3D9HardwareIndexBuffer.h"
#include "T3DD3D9VertexDeclaration.h"


namespace Tiny3D
{
    D3D9HardwareBufferManager::D3D9HardwareBufferManager()
        : HardwareBufferManagerBase()
    {

    }

    D3D9HardwareBufferManager::~D3D9HardwareBufferManager()
    {

    }

    HardwareVertexBufferPtr D3D9HardwareBufferManager::createVertexBuffer(size_t vertexSize, size_t vertexCount, HardwareBuffer::Usage usage, bool useShadowBuffer)
    {
        D3D9HardwareVertexBuffer *vertexBuffer = new D3D9HardwareVertexBuffer(vertexSize,
            vertexCount, usage, false, useShadowBuffer);

        HardwareVertexBufferPtr ptr(vertexBuffer);
        mVertexBuffers.insert(ptr);
        vertexBuffer->release();

        return ptr;
    }

    HardwareIndexBufferPtr D3D9HardwareBufferManager::createIndexBuffer(HardwareIndexBuffer::Type indexType, size_t indexCount, HardwareBuffer::Usage usage, bool useShadowBuffer)
    {
        D3D9HardwareIndexBuffer *indexBuffer = new D3D9HardwareIndexBuffer(indexType,
            indexCount, usage, false, useShadowBuffer);

        HardwareIndexBufferPtr ptr(indexBuffer);
        mIndexBuffers.insert(ptr);
        indexBuffer->release();

        return ptr;
    }

    VertexDeclarationPtr D3D9HardwareBufferManager::createVertexDeclaration()
    {
        D3D9VertexDeclaration *decl = new D3D9VertexDeclaration();
        VertexDeclarationPtr ptr(decl);
        decl->release();

        return ptr;
    }
}