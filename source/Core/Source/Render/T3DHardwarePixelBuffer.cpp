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

    void *HardwarePixelBuffer::lock(const Rect &rect, LockOptions options)
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

            buffer = smart_pointer_cast<HardwarePixelBuffer>(mShadowBuffer)->lock(rect, options);
        }
        else
        {
            // 没有影子buffer，调用实际锁接口
            buffer = lockImpl(rect, options);
            if (buffer != nullptr)
            {
                mIsLocked = true;
            }
        }

        if (buffer != nullptr)
        {
        }

        return buffer;
    }

    void *HardwarePixelBuffer::lock(LockOptions options)
    {
        return lock(Rect(0, 0, mWidth-1, mHeight-1), options);
    }

    bool HardwarePixelBuffer::copyTo(HardwarePixelBufferPtr dst, Rect *dstRect/* = nullptr*/, Rect *srcRect/* = nullptr*/) const
    {
        bool ret = false;

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

            if (rtSrc == rtDst)
            {
                // 两个区域相同

                PixelFormat dstFormat = dst->getFormat();
                PixelFormat srcFormat = getFormat();

                if (dstFormat == srcFormat)
                {
                    // 目标和源格式相同，直接复制
                    dst->lock(HardwareBuffer::E_HBL_WRITE_ONLY);
                }
            }
            else
            {
                // 两个区域不相同
            }

        } while (0);

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
}
