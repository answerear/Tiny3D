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


#include "Render/T3DHardwarePixelBuffer.h"
#include "Kernel/T3DImage.h"


namespace Tiny3D
{
    HardwarePixelBuffer::HardwarePixelBuffer(uint32_t width, uint32_t height,
        PixelFormat format, Usage usage, bool useSystemMemory, 
        bool useShadowBuffer)
        : HardwareBuffer(usage, useSystemMemory, useShadowBuffer)
        , mWidth(width)
        , mHeight(height)
        , mPitch(0)
        , mFormat(format)
    {
        int32_t bpp = Image::getBPP(mFormat);
        mPitch = Image::calcPitch(mWidth, bpp);
    }

    HardwarePixelBuffer::~HardwarePixelBuffer()
    {

    }

    void *HardwarePixelBuffer::lock(const Rect &rect, LockOptions options, 
        int32_t &lockedPitch)
    {
        T3D_ASSERT(!isLocked());

        void *buffer = nullptr;

        if (rect.left < 0 || rect.right >= mWidth
            || rect.top < 0 || rect.bottom >= mHeight)
        {
            // 越界了 :(
        }
        else if (mUseShadowBuffer)
        {
            if (options != E_HBL_READ_ONLY)
            {
                // 不是只读，那就可能会被改变，
                // 等会解锁时用影子buffer更新硬件buffer
                mIsShadowBufferDirty = true;
            }

            buffer = smart_pointer_cast<HardwarePixelBuffer>
                (mShadowBuffer)->lock(rect, options, lockedPitch);
        }
        else
        {
            // 没有影子buffer，调用实际锁接口
            buffer = lockImpl(rect, options, lockedPitch);

            if (buffer != nullptr)
            {
                mIsLocked = true;
            }
        }

        if (buffer != nullptr)
        {
            mLockSize = lockedPitch * rect.height();
        }

        return buffer;
    }

    void *HardwarePixelBuffer::lock(LockOptions options)
    {
        int32_t lockedPitch = 0;
        return lock(Rect(0, 0, mWidth - 1, mHeight - 1), options, lockedPitch);
    }

    TResult HardwarePixelBuffer::copyTo(HardwarePixelBufferPtr dst, 
        Rect *dstRect/* = nullptr*/, Rect *srcRect/* = nullptr*/)
    {
        TResult ret = T3D_ERR_OK;

        uint8_t *dstData = nullptr;
        uint8_t *srcData = nullptr;

        do
        {
            Rect rtSrc, rtDst;

            if (srcRect == nullptr)
            {
                rtSrc = Rect(0, 0, mWidth - 1, mHeight - 1);
            }
            else
            {
                rtSrc = *srcRect;
            }

            if (dstRect == nullptr)
            {
                rtDst = Rect(0, 0, mWidth - 1, mHeight - 1);
            }
            else
            {
                rtDst = *dstRect;
            }

            int32_t dstPitch = dst->getPitch();
            dstData = (uint8_t *)dst->lock(HardwareBuffer::E_HBL_WRITE_ONLY);
            if (dstData == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_RENDER,
                    "Lock destination pixel buffer failed !");
                break;
            }

            int32_t srcPitch = mPitch;
            srcData = (uint8_t *)lock(HardwareBuffer::E_HBL_READ_ONLY);
            if (srcData == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_RENDER,
                    "Lock source pixel buffer failed !");
                break;
            }

            Image srcImage;
            ret = srcImage.load(srcData, mWidth, mHeight, 
                Image::getBPP(mFormat), srcPitch, mFormat);
            if (ret != T3D_ERR_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RENDER,
                    "Load source texture to image failed  !");
                break;
            }

            Image dstImage;
            ret = dstImage.load(dstData, dst->getWidth(), dst->getHeight(), 
                Image::getBPP(dst->getFormat()), dstPitch, dst->getFormat());
            if (ret != T3D_ERR_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RENDER,
                    "Load destination texture to image failed !");
                break;
            }

            ret = dstImage.copy(srcImage, &rtSrc, &rtDst);
            if (ret != T3D_ERR_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RENDER,
                    "Copy image from source image failed !");
                break;
            }
        } while (0);

        if (srcData != nullptr)
        {
            ret = unlock();
            if (ret != T3D_ERR_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RENDER,
                    "Unlock source buffer failed !");
            }

            srcData = nullptr;
        }

        if (dstData != nullptr)
        {
            ret = dst->unlock();
            if (ret != T3D_ERR_OK)
            {
                T3D_LOG_ERROR(LOG_TAG_RENDER,
                    "Unlock destination buffer failed !");
            }

            dstData = nullptr;
        }

        return ret;
    }

    size_t HardwarePixelBuffer::readData(size_t offset, size_t size, void *dst)
    {
        T3D_LOG_ERROR(LOG_TAG_RENDER, "Reading a byte range is not implemented.\
            Use readImage.");
        return false;
    }

    size_t HardwarePixelBuffer::writeData(size_t offset, size_t size, 
        const void *src, bool discardWholeBuffer)
    {
        T3D_LOG_ERROR(LOG_TAG_RENDER, "Writing a byte range is not implemented.\
            Use writeImage.");
        return false;
    }

    void *HardwarePixelBuffer::lockImpl(size_t offset, size_t size, 
        LockOptions options)
    {
        T3D_LOG_ERROR(LOG_TAG_RENDER, "Lock implementation is not implemented.\
            Use lockImpl(const Rect &, LockOptions).");
        return nullptr;
    }
}

