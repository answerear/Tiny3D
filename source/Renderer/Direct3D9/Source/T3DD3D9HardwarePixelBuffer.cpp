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

#include "T3DD3D9HardwarePixelBuffer.h"
#include "T3DD3D9Renderer.h"
#include "T3DD3D9Mappings.h"


namespace Tiny3D
{
    D3D9HardwarePixelBuffer::D3D9HardwarePixelBuffer(
        uint32_t width, uint32_t height, PixelFormat format, 
        HardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer)
        : HardwarePixelBuffer(width, height, format, usage, useSystemMemory, useShadowBuffer)
    {

    }

    D3D9HardwarePixelBuffer::~D3D9HardwarePixelBuffer()
    {

    }

    void *D3D9HardwarePixelBuffer::lockImpl(const Rect &rect, LockOptions options)
    {
        return nullptr;
    }

    void D3D9HardwarePixelBuffer::unlockImpl()
    {

    }

    bool D3D9HardwarePixelBuffer::readImage(const Image &image, Rect *srcRect/* = nullptr*/, Rect *dstRect/* = nullptr*/)
    {
        bool ret = false;

        do 
        {
        } while (0);

        return ret;
    }

    bool D3D9HardwarePixelBuffer::writeImage(Image &image, Rect *dstRect/* = nullptr*/, Rect *srcRect/* = nullptr*/) const
    {
        bool ret = false;

        do 
        {
        } while (0);

        return ret;
    }
}
