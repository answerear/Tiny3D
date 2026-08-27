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
#include "Kernel/T3DAgent.h"
#include "RHI/T3DRHIContext.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    uint32_t RenderBuffer::validateGPUAccess(uint32_t gpuAccess, Usage usage)
    {
        uint32_t validated = gpuAccess;

        if ((validated & kGPUUnorderedAccess) != 0
            && (usage == Usage::kImmutable || usage == Usage::kDynamic || usage == Usage::kCopy))
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER, "kGPUUnorderedAccess is incompatible with usage [%u]. "
                "Only Usage::kStatic buffers can be bound as unordered access views. "
                "The flag is dropped !", (uint32_t)usage);
            validated &= ~(uint32_t)kGPUUnorderedAccess;
        }

        return validated;
    }

    //--------------------------------------------------------------------------

    RenderBuffer::RenderBuffer(const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode,
        uint32_t gpuAccess /* = kGPUNone */)
        : mMemoryType(memType)
        , mUsage(usage)
        , mAccessMode(accMode)
        , mGPUAccess(validateGPUAccess(gpuAccess, usage))
    {
        if (memType == MemoryType::kBoth)
        {
            mBuffer.DataSize = buffer.DataSize;
            mBuffer.Data = T3D_POD_NEW_ARRAY(uint8_t, mBuffer.DataSize);
            memcpy(mBuffer.Data, buffer.Data, buffer.DataSize);
        }
        else
        {
            mBuffer.Data = buffer.Data;
            mBuffer.DataSize = buffer.DataSize;
        }
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
            
            // TODO: 回调式无栅栏异步读回还没实现，需要 RHI 侧先提供 fence 查询。
            //       当前请改用 beginRead / endRead。
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    ReadbackHandle RenderBuffer::beginRead(size_t offset, size_t size)
    {
        RHIContext *ctx = T3D_AGENT.getActiveRHIContext();
        if (ctx == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_ENGINE, "RenderBuffer::beginRead : no active RHI context !");
            return ReadbackHandle::invalid();
        }

        return ctx->beginReadBuffer(this, offset, size);
    }

    //--------------------------------------------------------------------------

    TResult RenderBuffer::endRead(ReadbackHandle handle, Buffer &dst)
    {
        RHIContext *ctx = T3D_AGENT.getActiveRHIContext();
        if (ctx == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_ENGINE, "RenderBuffer::endRead : no active RHI context !");
            return T3D_ERR_INVALID_POINTER;
        }

        return ctx->endReadBuffer(handle, dst);
    }

    //--------------------------------------------------------------------------

    size_t RenderBuffer::writeData(size_t offset, const Buffer &buffer, bool discardWholeBuffer)
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

            if (offset + buffer.DataSize > mBuffer.DataSize)
            {
                // 偏移和长度会超过缓冲区大小
                T3D_LOG_ERROR(LOG_TAG_ENGINE, "Out of bound of the buffer !");
                break;
            }
            
            // 通过 RHIContext 写数据
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();
            TResult ret = ctx->writeBuffer(this, buffer, discardWholeBuffer);
            if (T3D_FAILED(ret))
            {
                break;
            }

            mBuffer.setData(buffer.Data, buffer.DataSize);
            bytesOfWritten = buffer.DataSize;
        } while (false);
        
        return bytesOfWritten;
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
