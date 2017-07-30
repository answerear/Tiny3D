/*******************************************************************************
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
 ******************************************************************************/

#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    HardwareBufferManagerBase::HardwareBufferManagerBase()
    {

    }

    HardwareBufferManagerBase::~HardwareBufferManagerBase()
    {

    }

    VertexDeclarationPtr HardwareBufferManagerBase::createVertexDeclaration()
    {
        VertexDeclaration *decl = new VertexDeclaration();
        VertexDeclarationPtr ptr(decl);
        decl->release();
        return decl;
    }

    T3D_INIT_SINGLETON(HardwareBufferManager);

    HardwareBufferManager::HardwareBufferManager(HardwareBufferManagerBase *impl)
        : mImpl(impl)
    {

    }

    HardwareBufferManager::~HardwareBufferManager()
    {
        mImpl = nullptr;
    }

    HardwareVertexBufferPtr HardwareBufferManager::createVertexBuffer(size_t vertexSize, size_t vertexCount, HardwareBuffer::Usage usage, bool useShadowBuffer)
    {
        return mImpl->createVertexBuffer(vertexSize, vertexCount, usage, useShadowBuffer);
    }

    HardwareIndexBufferPtr HardwareBufferManager::createIndexBuffer(HardwareIndexBuffer::Type indexType, size_t indexCount, HardwareBuffer::Usage usage, bool useShadowBuffer)
    {
        return mImpl->createIndexBuffer(indexType, indexCount, usage, useShadowBuffer);
    }

    HardwarePixelBufferPtr HardwareBufferManager::createPixelBuffer(uint32_t width, uint32_t height, PixelFormat format, HardwareBuffer::Usage usage, bool useShadowBuffer)
    {
        return mImpl->createPixelBuffer(width, height, format, usage, useShadowBuffer);
    }

    VertexDeclarationPtr HardwareBufferManager::createVertexDeclaration()
    {
        return mImpl->createVertexDeclaration();
    }
}
