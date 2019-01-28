/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Agent)
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


#include "Render/T3DHardwareBuffer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    HardwareBuffer::HardwareBuffer(Usage usage, bool useSystemMemory, 
        bool useShadowBuffer)
        : mBufferSize(0)
        , mLockSize(0)
        , mLockOffset(0)
        , mUsage(usage)
        , mUseSystemMemory(useSystemMemory)
        , mUseShadowBuffer(useShadowBuffer)
        , mIsLocked(false)
        , mIsShadowBufferDirty(false)
    {
        if (useShadowBuffer && usage == E_HBU_DYNAMIC)
        {
            // 使用了影子缓存，则直接动态只读的硬件缓存，影子缓存用于提升其性能
            mUsage = E_HBU_DYNAMIC_WRITE_ONLY;
        }
        else if (useShadowBuffer && usage == E_HBU_STATIC)
        {
            mUsage = E_HBU_STATIC_WRITE_ONLY;
        }
    }

    HardwareBuffer::~HardwareBuffer()
    {
        mUseShadowBuffer = nullptr;
    }

    //--------------------------------------------------------------------------

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
                // 不是只读，那就可能会被改变，
                // 等会解锁时用影子buffer更新硬件buffer
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

    //--------------------------------------------------------------------------

    void *HardwareBuffer::lock(LockOptions options)
    {
        return lock(0, mBufferSize, options);
    }

    //--------------------------------------------------------------------------

    TResult HardwareBuffer::unlock()
    {
        T3D_ASSERT(isLocked());

        TResult ret = T3D_OK;

        do 
        {
            if (mUseShadowBuffer && mShadowBuffer->isLocked())
            {
                ret = mShadowBuffer->unlock();
                if (ret != T3D_OK)
                {
                    break;
                }

                updateFromShadow();
            }
            else
            {
                ret = unlockImpl();
                if (ret != T3D_OK)
                {
                    break;
                }

                mIsLocked = false;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    size_t HardwareBuffer::copyData(HardwareBufferPtr srcBuffer, 
        size_t srcOffset, size_t dstOffset, size_t size, 
        bool discardWholeBuffer /* = false */)
    {
        if (srcBuffer == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER, 
                "Copy data failed ! Source buffer is nullptr !");
            return 0;
        }

        size_t s = 0;

        const void *src = srcBuffer->lock(srcOffset, size, E_HBL_READ_ONLY);
        if (src != nullptr)
        {
            s = writeData(dstOffset, size, src, discardWholeBuffer);
            srcBuffer->unlock();
        }

        return s;
    }

    size_t HardwareBuffer::copyData(HardwareBufferPtr srcBuffer)
    {
        size_t size = std::min(getBufferSize(), srcBuffer->getBufferSize());
        return copyData(srcBuffer, 0, 0, size, true);
    }

    //--------------------------------------------------------------------------

    TResult HardwareBuffer::updateFromShadow()
    {
        TResult ret = T3D_OK;

        if (mUseShadowBuffer && mIsShadowBufferDirty)
        {
            const void *src = mShadowBuffer->lock(mLockOffset, mLockSize, 
                E_HBL_READ_ONLY);

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
                    ret = unlockImpl();
                }

                ret = mShadowBuffer->unlock();
                mIsShadowBufferDirty = false;
            }
        }

        return ret;
    }
}

