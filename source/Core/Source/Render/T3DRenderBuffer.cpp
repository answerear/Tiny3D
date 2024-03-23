/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
            if (mUsage == Usage::kImmutable)
            {
                // 不可写缓冲区，无法写
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
