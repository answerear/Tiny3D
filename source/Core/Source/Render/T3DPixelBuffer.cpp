/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
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


#include "Render/T3DPixelBuffer.h"
#include "ImageCodec/T3DImage.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    PixelBufferPtr PixelBuffer::create(const Buffer &buffer, size_t width, size_t height, PixelFormat format, size_t mipmaps, MemoryType memType, Usage usage, uint32_t accMode)
    {
        return new PixelBuffer(buffer, width, height, format, mipmaps, memType, usage, accMode);
    }

    //--------------------------------------------------------------------------

    PixelBuffer::PixelBuffer(const Buffer &buffer, size_t width, size_t height, PixelFormat format, size_t mipmaps, MemoryType memType, Usage usage, uint32_t accMode)
        : RenderBuffer(buffer, memType, usage, accMode)
        , mWidth(width)
        , mHeight(height)
        , mMipmaps(mipmaps)
        , mFormat(format)
    {
        size_t bpp = Image::getBPP(mFormat);
        mPitch = Image::calcPitch(mWidth, bpp);
    }

    //--------------------------------------------------------------------------

    RenderResource::Type PixelBuffer::getType() const
    {
        return Type::kPixelBuffer;
    }

    //--------------------------------------------------------------------------

    bool PixelBuffer::onLoad()
    {
        return true;
    }

    //--------------------------------------------------------------------------

    bool PixelBuffer::onUnload()
    {
        return true;
    }

    //--------------------------------------------------------------------------
}
