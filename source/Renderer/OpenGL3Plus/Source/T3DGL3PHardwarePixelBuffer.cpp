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

#include "T3DGL3PHardwarePixelBuffer.h"

namespace Tiny3D
{
    GL3PHardwarePixelBuffer::GL3PHardwarePixelBuffer(
        uint32_t width, uint32_t height, PixelFormat format,
        HardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer)
        : HardwarePixelBuffer(width, height, format, usage, useSystemMemory, useShadowBuffer)
    {
        createTexture();
    }

    GL3PHardwarePixelBuffer::~GL3PHardwarePixelBuffer()
    {
    }

    bool GL3PHardwarePixelBuffer::createTexture()
    {
        bool ret = false;

        do
        {
            ret = true;
        } while (0);

        return ret;
    }

    void *GL3PHardwarePixelBuffer::lockImpl(const Rect &rect, LockOptions options, int32_t &lockedPitch)
    {
        return nullptr;
    }

    void GL3PHardwarePixelBuffer::unlockImpl()
    {
    }

    bool GL3PHardwarePixelBuffer::readImage(const Image &image, Rect *srcRect/* = nullptr*/, Rect *dstRect/* = nullptr*/)
    {
        bool ret = false;

        uint8_t *dst = nullptr;

        do
        {
            int32_t bpp = Image::getBPP(mFormat);
            Image temp;
            int32_t dstPitch = 0;
            Rect rtDst;

            if (dstRect == nullptr)
            {
                dst = (uint8_t *)lock(E_HBL_DISCARD);
                dstPitch = mPitch;
                rtDst = Rect(0, 0, mWidth - 1, mHeight - 1);
            }
            else
            {
                rtDst = *dstRect;
                dst = (uint8_t *)lock(rtDst, E_HBL_WRITE_ONLY, dstPitch);
            }

            // 临时构造一个图像对象，用于复制数据
            if (!temp.load(dst, rtDst.width(), rtDst.height(), bpp, dstPitch, mFormat))
            {
                break;
            }

            // 复制图像数据到纹理
            if (!temp.copy(image, srcRect))
            {
                break;
            }

            unlock();
            dst = nullptr;

            ret = true;
        } while (0);

        if (dst != nullptr)
        {
            unlock();
            dst = nullptr;
        }

        return ret;
    }

    bool GL3PHardwarePixelBuffer::writeImage(Image &image, Rect *dstRect/* = nullptr*/, Rect *srcRect/* = nullptr*/)
    {
        bool ret = false;

        uint8_t *src = nullptr;

        do
        {
            int32_t bpp = Image::getBPP(mFormat);
            Image temp;
            int32_t srcPitch = 0;
            Rect rtSrc;

            if (srcRect == nullptr)
            {
                src = (uint8_t *)lock(E_HBL_READ_ONLY);
                srcPitch = mPitch;
                rtSrc = Rect(0, 0, mWidth - 1, mHeight - 1);
            }

            // 临时构造一个图像对象，用于复制数据
            if (!temp.load(src, mWidth, mHeight, bpp, mPitch, mFormat))
            {
                break;
            }

            // 复制图像数据到纹理
            if (!image.copy(temp, dstRect))
            {
                break;
            }

            unlock();
            src = nullptr;

            ret = true;
        } while (0);

        if (src != nullptr)
        {
            unlock();
            src = nullptr;
        }

        return ret;
    }
}