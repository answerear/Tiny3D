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


#include "Render/T3DRenderBuffer.h"
#include "T3DErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    RenderBuffer::RenderBuffer(const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode)
        : mBuffer(buffer)
        , mMemoryType(memType)
        , mUsage(usage)
        , mAccessMode(accMode)
    {
        
    }

    //--------------------------------------------------------------------------

    RenderBuffer::~RenderBuffer()
    {
        mBuffer.release();
    }
    
    //--------------------------------------------------------------------------

    size_t RenderBuffer::readData(size_t offset, size_t size, void *dst)
    {
        size_t bytesOfRead = 0;
        
        if (mUsage == Usage::kStatic && mMemoryType != MemoryType::kVRAM)
        {
            // 不动态更新的数据，并且在系统内存也有一份的，才能读取，以提高性能
            if (offset + size <= mBuffer.DataSize)
            {
                bytesOfRead = size;
            }
            else
            {
                bytesOfRead = mBuffer.DataSize - offset;
            }
            
            memcpy(dst, mBuffer.Data+offset, bytesOfRead);
        }
        
        return bytesOfRead;
    }

    //--------------------------------------------------------------------------

    TResult RenderBuffer::readData(size_t offset, size_t size, RenderBuffeReadCallback callback)
    {
        TResult ret = T3D_OK;

        do
        {
            if (mUsage == Usage::kStatic)
            {
                // 静态数据，直接用同步接口读取
                T3D_LOG_WARNING(LOG_TAG_ENGINE, "Static buffer should use sync api - readData(ize_t offset, size_t size, void *dst) !");
                ret = T3D_ERR_RENDER_BUFFER_READ_SYNC;
                break;
            }

            if (mMemoryType == MemoryType::kRAM)
            {
                // 内存中数据，直接用同步接口读取
                T3D_LOG_WARNING(LOG_TAG_ENGINE, "Data is in RAM, it should use sync api - readData(ize_t offset, size_t size, void *dst)");
                ret = T3D_ERR_RENDER_BUFFER_READ_SYNC;
                break;
            }

            if (mAccessMode == kCPUNone || (mAccessMode & kCPURead) != kCPURead)
            {
                // CPU 无法读取
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "CPU could not read data ! You should create this buffer with flag - kCPURead !");
                ret = T3D_ERR_FAIL;
                break;
            }
            
            // TODO: 通过 RHIContext 读数据
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    size_t RenderBuffer::writeData(size_t offset, size_t size, const void *src, bool discardWholeBuffer)
    {
        size_t bytesOfWritten = 0;

        do
        {
            if (mUsage == Usage::kStatic)
            {
                // 静态缓冲区，无法写
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Static buffer could not be written !");
                break;
            }

            if (!(mAccessMode & kCPUWrite))
            {
                // 没有设置 CPU 写权限，无法写
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "CPU could not write data !");
                break;
            }

            if (offset + size > mBuffer.DataSize)
            {
                // 偏移和长度会超过缓冲区大小
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Out of bound of the buffer !");
                break;
            }
            
            mBuffer.setData(src, size);
            
            // TODO: 通过 RHIContext 写数据
        } while (false);
        
        return 0;
    }

    //--------------------------------------------------------------------------

    size_t RenderBuffer::copyData(RenderBufferPtr srcBuffer, size_t srcOffset, size_t dstOffset, size_t size, bool discardWholeBuffer)
    {
        // TODO: 暂不支持
        return 0;
    }

    //--------------------------------------------------------------------------

    size_t RenderBuffer::copyData(RenderBufferPtr srcBuffer)
    {
        // TODO: 暂不支持
        return 0;
    }

    //--------------------------------------------------------------------------
}
