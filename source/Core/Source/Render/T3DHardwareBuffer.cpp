/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Aaron Wong
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

#include "Render/T3DHardwareBuffer.h"


namespace Tiny3D
{
    HardwareBuffer::HardwareBuffer(Usage usage, bool systemMemory, bool useShadowBuffer)
        : mBufferSize(0)
        , mLockSize(0)
        , mLockOffset(0)
        , mUsage(usage)
        , mShadowBuffer(nullptr)
        , mSystemMemory(systemMemory)
        , mUseShadowBuffer(useShadowBuffer)
        , mIsLocked(false)
        , mIsShadowBufferDirty(false)
    {
        if (useShadowBuffer && usage == E_HBU_DYNAMIC)
        {
            mUsage = E_HBU_DYNAMIC_WRITE_ONLY;
        }
        else if (useShadowBuffer && usage == E_HBU_STATIC)
        {
            mUsage = E_HBU_STATIC_WRITE_ONLY;
        }
    }

    HardwareBuffer::~HardwareBuffer()
    {
        mShadowBuffer = nullptr;
    }

    void *HardwareBuffer::lock(size_t offset, size_t size, LockOptions options)
    {
        T3D_ASSERT(!isLocked());

        void *buffer = nullptr;

        if ((offset + size) > mBufferSize)
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

            buffer = mShadowBuffer->lock(offset, size, options);
        }
        else
        {
            // 没有影子buffer，调用实际锁接口
            buffer = lockImpl(offset, size, options);
            if (buffer != nullptr)
            {
                mIsLocked = true;
            }
        }

        if (buffer != nullptr)
        {
            mLockOffset = offset;
            mLockSize = size;
        }

        return buffer;
    }

    void *HardwareBuffer::lock(LockOptions options)
    {
        return lock(0, mBufferSize, options);
    }

    void HardwareBuffer::unlock()
    {
        T3D_ASSERT(isLocked());

        if (mUseShadowBuffer && mShadowBuffer->isLocked())
        {
            mShadowBuffer->unlock();
            updateFromShadow();
        }
        else
        {
            unlockImpl();
            mIsLocked = false;
        }
    }

    bool HardwareBuffer::copyData(const HardwareBufferPtr &srcBuffer, size_t srcOffset, size_t dstOffset, size_t size, bool discardWholeBuffer /* = false */)
    {
        bool ret = false;
        const void *src = srcBuffer->lock(srcOffset, size, E_HBL_READ_ONLY);
        if (src != nullptr)
        {
            ret = writeData(dstOffset, size, src, true);
            srcBuffer->unlock();
        }

        return ret;
    }

    bool HardwareBuffer::copyData(const HardwareBufferPtr &srcBuffer)
    {
        size_t size = std::min(getBufferSize(), srcBuffer->getBufferSize()); 
        return copyData(srcBuffer, 0, 0, size, true);
    }

    void HardwareBuffer::updateFromShadow()
    {
        if (mUseShadowBuffer && mIsShadowBufferDirty)
        {
            const void *src = mShadowBuffer->lock(mLockOffset, mLockSize, E_HBL_READ_ONLY);
            if (src != nullptr)
            {
                LockOptions options;
                if (mLockOffset == 0 && mLockSize == mBufferSize)
                {
                    options = E_HBL_DISCARD;
                }
                else
                {
                    options = E_HBL_NORMAL;
                }

                void *dst = lockImpl(mLockOffset, mLockSize, options);
                if (dst != nullptr)
                {
                    memcpy(dst, src, mLockSize);
                    unlockImpl();
                }

                mShadowBuffer->unlock();
                mIsShadowBufferDirty = false;
            }
        }
    }
}
