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


#include "T3DD3D11HardwarePixelBuffer.h"
#include "T3DD3D11Mappings.h"
#include "T3DD3D11Renderer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11HardwarePixelBufferPtr D3D11HardwarePixelBuffer::create(size_t width,
        size_t height, PixelFormat format, const void *pixels, Usage usage, 
        uint32_t mode)
    {
        D3D11HardwarePixelBufferPtr pb = new D3D11HardwarePixelBuffer(width, 
            height, format, usage, mode);
        pb->release();

        if (pb->init(pixels) != T3D_OK)
        {
            pb = nullptr;
        }

        return pb;
    }

    //--------------------------------------------------------------------------

    D3D11HardwarePixelBuffer::D3D11HardwarePixelBuffer(size_t width,
        size_t height, PixelFormat format, Usage usage, uint32_t mode)
        : HardwarePixelBuffer(width, height, format, usage, mode)
    {
    }

    //--------------------------------------------------------------------------

    D3D11HardwarePixelBuffer::~D3D11HardwarePixelBuffer()
    {
    }

    //--------------------------------------------------------------------------

    TResult D3D11HardwarePixelBuffer::init(const void *pixels)
    {
        TResult ret = T3D_OK;

        do 
        {
            
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11HardwarePixelBuffer::readImage(const Image &image,
        Rect *srcRect /* = nullptr */, Rect *dstRect /* = nullptr */)
    {
        TResult ret = T3D_OK;

        uint8_t *dst = nullptr;

        do
        {
            size_t bpp = Image::getBPP(mFormat);
            Image temp;
            size_t dstPitch = 0;
            Rect rtDst;

            if (dstRect == nullptr)
            {
                dst = (uint8_t *)lock(LockOptions::WRITE_DISCARD);
                dstPitch = mPitch;
                rtDst = Rect(0, 0, mWidth - 1, mHeight - 1);
            }
            else
            {
                rtDst = *dstRect;
                dst = (uint8_t *)lock(rtDst, LockOptions::WRITE, 
                    dstPitch);
            }

            // 临时构造一个图像对象，用于复制数据
            ret = temp.load(dst, rtDst.width(), rtDst.height(), bpp,
                dstPitch, mFormat);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Load temporary image \
                        for reading image failed !");
                break;
            }

            // 复制图像数据到纹理
            ret = temp.copy(image, srcRect);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Copy temporary image \
                        for reading image failed !");
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

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11HardwarePixelBuffer::writeImage(Image &image,
        Rect *dstRect /* = nullptr */, Rect *srcRect /* = nullptr */)
    {
        TResult ret = T3D_OK;

        uint8_t *src = nullptr;

        do
        {
            size_t bpp = Image::getBPP(mFormat);
            Image temp;
            size_t srcPitch = 0;
            Rect rtSrc;

            if (srcRect == nullptr)
            {
                src = (uint8_t *)lock(LockOptions::READ);
                srcPitch = mPitch;
                rtSrc = Rect(0, 0, mWidth - 1, mHeight - 1);
            }

            // 临时构造一个图像对象，用于复制数据
            ret = temp.load(src, mWidth, mHeight, bpp, mPitch, mFormat);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Load temporary image \
                        for writing image failed !");
                break;
            }

            // 复制图像数据到纹理
            ret = image.copy(temp, dstRect);
            if (ret != T3D_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Copy temporary image \
                        for writing image failed !");
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

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void *D3D11HardwarePixelBuffer::lockImpl(const Rect &rect,
        LockOptions options, size_t &lockedPitch)
    {
        if (rect.left < 0 || rect.right < 0
            || rect.right >= getWidth() || rect.bottom >= getHeight())
        {
            // 越界了
            return nullptr;
        }

        TResult ret = T3D_OK;
        uint8_t *lockedBuffer = nullptr;

        do
        {
            
        } while (0);

        return lockedBuffer;
    }

    //--------------------------------------------------------------------------

    TResult D3D11HardwarePixelBuffer::unlockImpl()
    {
        TResult ret = T3D_OK;

        do 
        {
            
        } while (0);

        return ret;
    }
}


