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

#include "Render/T3DHardwarePixelBuffer.h"


namespace Tiny3D
{
    HardwarePixelBuffer::HardwarePixelBuffer(uint32_t width, uint32_t height, 
        PixelFormat format, Usage usage, bool useSystemMemory, bool useShadowBuffer)
        : HardwareBuffer(usage, useSystemMemory, useShadowBuffer)
    {

    }

    HardwarePixelBuffer::~HardwarePixelBuffer()
    {

    }

    void *HardwarePixelBuffer::lock(const Rect &rect, LockOptions options, int32_t &lockedPitch)
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
                // 不是只读，那就可能会被改变，等会解锁时用影子buffer更新硬件buffer
                mIsShadowBufferDirty = true;
            }

            buffer = smart_pointer_cast<HardwarePixelBuffer>(mShadowBuffer)->lock(rect, options, lockedPitch);
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
        return lock(Rect(0, 0, mWidth-1, mHeight-1), options, lockedPitch);
    }

    bool HardwarePixelBuffer::copyTo(HardwarePixelBufferPtr dst, Rect *dstRect/* = nullptr*/, Rect *srcRect/* = nullptr*/)
    {
        bool ret = false;

        uint8_t *dstData = nullptr;
        uint8_t *srcData = nullptr;

        do 
        {
            Rect rtSrc, rtDst;

            if (srcRect == nullptr)
            {
                rtSrc = Rect(0, 0, mWidth-1, mHeight-1);
            }
            else
            {
                rtSrc = *srcRect;
            }

            if (dstRect == nullptr)
            {
                rtDst = Rect(0, 0, mWidth-1, mHeight-1);
            }
            else
            {
                rtDst = *dstRect;
            }

            int32_t dstPitch = 0;
            dstData = (uint8_t *)dst->lock(rtDst, HardwareBuffer::E_HBL_WRITE_ONLY, dstPitch);
            if (dstData == nullptr)
            {
                T3D_LOG_ERROR("Lock destination pixel buffer failed !");
                break;
            }

            int32_t srcPitch = 0;
            srcData = (uint8_t *)lock(rtSrc, HardwareBuffer::E_HBL_READ_ONLY, srcPitch);
            if (srcData == nullptr)
            {
                T3D_LOG_ERROR("Lock source pixel buffer failed !");
                break;
            }

        } while (0);

        if (!ret)
        {
            if (srcData != nullptr)
            {
                unlock();
                srcData = nullptr;
            }

            if (dstData != nullptr)
            {
                dst->unlock();
                dstData = nullptr;
            }
        }

        return ret;
    }

    bool HardwarePixelBuffer::readData(size_t offset, size_t size, void *dst)
    {
        T3D_LOG_ERROR("Reading a byte range is not implemented. Use readImage.");
        return false;
    }

    bool HardwarePixelBuffer::writeData(size_t offset, size_t size, const void *src, bool discardWholeBuffer)
    {
        T3D_LOG_ERROR("Writing a byte range is not implemented. Use writeImage.");
        return false;
    }

    void *HardwarePixelBuffer::lockImpl(size_t offset, size_t size, LockOptions options)
    {
        T3D_LOG_ERROR("Lock implementation is not implemented. Use lockImpl(const Rect &, LockOptions).");
        return nullptr;
    }

    int32_t HardwarePixelBuffer::getBPP() const
    {
        int32_t bpp = 0;

        switch (mFormat)
        {
        case E_PF_PALETTE8:
            bpp = 8;
            break;
        case E_PF_R5G6B5:
        case E_PF_A1R5G5B5:
        case E_PF_A4R4G4B4:
            bpp = 16;
            break;
        case E_PF_R8G8B8:
        case E_PF_B8G8R8:
            bpp = 24;
            break;
        case E_PF_A8R8G8B8:
        case E_PF_B8G8R8A8:
        case E_PF_X8R8G8B8:
        case E_PF_B8G8R8X8:
            bpp = 32;
            break;
        }

        return bpp;
    }
}
