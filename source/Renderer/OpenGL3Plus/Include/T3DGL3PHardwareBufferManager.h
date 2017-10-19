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

#ifndef __T3D_GL3P_HARDWARE_BUFFER_MANAGER_H__
#define __T3D_GL3P_HARDWARE_BUFFER_MANAGER_H__


#include "T3DGL3PPrerequisites.h"

namespace Tiny3D
{
    class GL3PHardwareBufferManager : public HardwareBufferManagerBase
    {
    public:
        GL3PHardwareBufferManager();
        virtual ~GL3PHardwareBufferManager();

        virtual HardwareVertexBufferPtr createVertexBuffer(size_t vertexSize, size_t vertexCount, HardwareBuffer::Usage usage, bool useShadowBuffer) override;
        virtual HardwareIndexBufferPtr createIndexBuffer(HardwareIndexBuffer::Type indexType, size_t indexCount, HardwareBuffer::Usage usage, bool useShadowBuffer) override;
        virtual HardwarePixelBufferPtr createPixelBuffer(uint32_t width, uint32_t height, PixelFormat format, HardwareBuffer::Usage usage, bool useShadowBuffer) override;

        virtual VertexDeclarationPtr createVertexDeclaration() override;
    };
}


#endif  /*__T3D_GL3P_HARDWARE_BUFFER_MANAGER_H__*/