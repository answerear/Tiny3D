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


#include "T3DR3DHardwarePixelBuffer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    R3DHardwarePixelBufferPtr R3DHardwarePixelBuffer::create(size_t width,
        size_t height, PixelFormat format, Usage usage, bool useSystemMemory,
        bool useShadowBuffer)
    {
        R3DHardwarePixelBufferPtr pb = new R3DHardwarePixelBuffer(width, height,
            format, usage, useSystemMemory, useShadowBuffer);
        pb->release();
        return pb;
    }

    //--------------------------------------------------------------------------

    R3DHardwarePixelBuffer::R3DHardwarePixelBuffer(size_t width,
        size_t height, PixelFormat format, Usage usage, bool useSystemMemory,
        bool useShadowBuffer)
        : HardwarePixelBuffer(width, height, format, usage, useSystemMemory, 
            useShadowBuffer)
    {

    }

    //--------------------------------------------------------------------------

    R3DHardwarePixelBuffer::~R3DHardwarePixelBuffer()
    {

    }

    //--------------------------------------------------------------------------

    TResult R3DHardwarePixelBuffer::readImage(const Image &image, 
        Rect *srcRect /* = nullptr */, Rect *dstRect /* = nullptr */)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult R3DHardwarePixelBuffer::writeImage(Image &image,
        Rect *dstRect /* = nullptr */, Rect *srcRect /* = nullptr */)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void *R3DHardwarePixelBuffer::lockImpl(const Rect &rect,
        LockOptions options, int32_t &lockedPitch)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult R3DHardwarePixelBuffer::unlockImpl()
    {
        return T3D_OK;
    }
}
