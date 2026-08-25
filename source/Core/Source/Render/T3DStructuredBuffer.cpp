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


#include "Render/T3DStructuredBuffer.h"
#include "Kernel/T3DAgent.h"
#include "RHI/T3DRHIContext.h"
#include "RHI/T3DRHIStructuredBuffer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    bool StructuredBuffer::validateDescriptor(StructuredBufferDesc &desc, uint32_t gpuAccess)
    {
        if (desc.elementCount == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER, "Structured buffer element count must not be zero !");
            return false;
        }

        switch (desc.kind)
        {
        case StructuredBufferKind::kStructured:
            {
                // 结构化缓冲的 StructureByteStride 在各后端都要求 4 字节对齐
                if (desc.elementSize == 0 || (desc.elementSize % 4) != 0)
                {
                    T3D_LOG_ERROR(LOG_TAG_RENDER, "Structured buffer element size [%u] must be a "
                        "non-zero multiple of 4 !", desc.elementSize);
                    return false;
                }
            }
            break;
        case StructuredBufferKind::kByteAddress:
            {
                // Raw 视图按 4 字节寻址，elementSize 由形态本身决定，忽略调用方填的值
                desc.elementSize = 4;
            }
            break;
        case StructuredBufferKind::kTyped:
            {
                if (desc.format == PixelFormat::E_PF_UNKNOWN)
                {
                    T3D_LOG_ERROR(LOG_TAG_RENDER, "Typed structured buffer requires an explicit "
                        "pixel format !");
                    return false;
                }

                if (desc.elementSize == 0)
                {
                    T3D_LOG_ERROR(LOG_TAG_RENDER, "Typed structured buffer element size must not "
                        "be zero !");
                    return false;
                }
            }
            break;
        }

        if (desc.hasCounter && desc.isAppendConsume)
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER, "Structured buffer cannot be both a counter buffer and "
                "an append/consume buffer !");
            return false;
        }

        if ((desc.hasCounter || desc.isAppendConsume) && (gpuAccess & kGPUUnorderedAccess) == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER, "Counter / append-consume structured buffer requires "
                "kGPUUnorderedAccess !");
            return false;
        }

        if ((gpuAccess & kGPUIndirectArgs) != 0
            && (((size_t)desc.elementSize * desc.elementCount) % 4) != 0)
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER, "Indirect args buffer size must be a multiple of 4 bytes !");
            return false;
        }

        return true;
    }

    //--------------------------------------------------------------------------

    StructuredBufferPtr StructuredBuffer::create(const StructuredBufferDesc &desc, MemoryType memType,
        Usage usage, uint32_t accMode, uint32_t gpuAccess)
    {
        StructuredBufferDesc validated = desc;
        if (!validateDescriptor(validated, gpuAccess))
        {
            return nullptr;
        }

        return T3D_NEW StructuredBuffer(validated, memType, usage, accMode, gpuAccess);
    }

    //--------------------------------------------------------------------------

    StructuredBuffer::StructuredBuffer(const StructuredBufferDesc &desc, MemoryType memType,
        Usage usage, uint32_t accMode, uint32_t gpuAccess)
        : RenderBuffer(desc.buffer, memType, usage, accMode, gpuAccess)
        , mDesc(desc)
    {
        // 初始数据的所有权已经交给基类的 mBuffer，这里必须断开别名，
        // 否则 mDesc.buffer 会在基类析构后变成悬垂指针
        mDesc.buffer.Data = nullptr;
        mDesc.buffer.DataSize = 0;
    }

    //--------------------------------------------------------------------------

    RenderResource::Type StructuredBuffer::getType() const
    {
        return Type::kStructuredBuffer;
    }

    //--------------------------------------------------------------------------

    bool StructuredBuffer::onLoad()
    {
        mRHIResource = T3D_AGENT.getActiveRHIContext()->createStructuredBuffer(this);
        return true;
    }

    //--------------------------------------------------------------------------

    bool StructuredBuffer::onUnload()
    {
        mRHIResource = nullptr;
        return true;
    }

    //--------------------------------------------------------------------------
}
