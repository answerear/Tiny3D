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


#include "T3DR3DHardwareBufferManager.h"
#include "T3DR3DHardwareVertexBuffer.h"
#include "T3DR3DHardwareIndexBuffer.h"
#include "T3DR3DHardwarePixelBuffer.h"
#include "T3DR3DVertexDeclaration.h"
#include "T3DR3DVertexArrayObject.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    R3DHardwareBufferManagerPtr R3DHardwareBufferManager::create()
    {
        R3DHardwareBufferManagerPtr mgr = new R3DHardwareBufferManager();
        mgr->release();
        return mgr;
    }

    //--------------------------------------------------------------------------

    R3DHardwareBufferManager::R3DHardwareBufferManager()
    {

    }

    //--------------------------------------------------------------------------

    R3DHardwareBufferManager::~R3DHardwareBufferManager()
    {

    }

    //--------------------------------------------------------------------------

    HardwareVertexBufferPtr R3DHardwareBufferManager::createVertexBuffer(
        size_t vertexSize, size_t vertexCount, HardwareBuffer::Usage usage, 
        bool useSystemMemory, bool useShadowBuffer)
    {
        return R3DHardwareVertexBuffer::create(vertexSize, vertexCount, usage,
            useSystemMemory, useShadowBuffer);
    }

    //--------------------------------------------------------------------------

    HardwareIndexBufferPtr R3DHardwareBufferManager::createIndexBuffer(
        HardwareIndexBuffer::Type indexType, size_t indexCount, 
        HardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer)
    {
        return R3DHardwareIndexBuffer::create(indexType, indexCount, usage, 
            useSystemMemory, useShadowBuffer);
    }

    //--------------------------------------------------------------------------

    HardwarePixelBufferPtr R3DHardwareBufferManager::createPixelBuffer(
        size_t width, size_t height, PixelFormat format,
        HardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer)
    {
        return R3DHardwarePixelBuffer::create(width, height, format, usage, 
            useSystemMemory, useShadowBuffer);
    }

    //--------------------------------------------------------------------------

    VertexArrayObjectPtr R3DHardwareBufferManager::createVertexArrayObject(
        bool useIndices)
    {
        return R3DVertexArrayObject::create(useIndices);
    }

    //--------------------------------------------------------------------------

    VertexDeclarationPtr R3DHardwareBufferManager::createVertexDeclaration()
    {
        return R3DVertexDeclaration::create();
    }
}
