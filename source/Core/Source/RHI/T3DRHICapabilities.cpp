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

#include "RHI/T3DRHICapabilities.h"
#include "Render/T3DBlendState.h"
#include "Render/T3DPixelBuffer.h"
#include "Render/T3DRenderTarget.h"
#include "Render/T3DRenderTexture.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    bool RHICapabilities::checkRenderTextureFormat(const char *tag, const char *func, PixelFormat format,
        uint32_t sampleCount, bool wantsUAV) const
    {
        const uint32_t support = getFormatSupport(format);
        const char *name = PixelFormatUtil::getName(format);

        if (PixelFormatUtil::isDepthFormat(format))
        {
            if ((support & kFormatSupportDepthStencil) == 0)
            {
                T3D_LOG_ERROR(tag, "%s : format [%s] cannot be used as depth stencil target !", func, name);
                return false;
            }
            return true;
        }

        if ((support & kFormatSupportRenderTarget) == 0)
        {
            T3D_LOG_ERROR(tag, "%s : format [%s] cannot be used as render target !", func, name);
            return false;
        }

        if (sampleCount > 1 && (support & kFormatSupportMultisample) == 0)
        {
            T3D_LOG_ERROR(tag, "%s : format [%s] does not support multisample render target or resolve !", func, name);
            return false;
        }

        if (wantsUAV && sampleCount <= 1 && (support & kFormatSupportUnorderedAccess) == 0)
        {
            T3D_LOG_ERROR(tag, "%s : format [%s] does not support unordered access !", func, name);
            return false;
        }

        return true;
    }

    //--------------------------------------------------------------------------

    bool RHICapabilities::checkCompressedTextureFormat(const char *tag, const char *func, PixelFormat format,
        uint32_t dimension, uint32_t width, uint32_t height, uint32_t sampleCount, bool wantsUAV) const
    {
        if (!PixelFormatUtil::isCompressed(format))
        {
            return true;
        }

        const char *name = PixelFormatUtil::getName(format);

        if (dimension != 2)
        {
            T3D_LOG_ERROR(tag, "%s : compressed format [%s] only supports 2D, 2D array and cube textures !", func, name);
            return false;
        }

        if ((getFormatSupport(format) & kFormatSupportTexture) == 0)
        {
            T3D_LOG_ERROR(tag, "%s : compressed format [%s] is not supported by this device !", func, name);
            return false;
        }

        const uint32_t blockSize = PixelFormatUtil::getBlockSize(format);
        if (width % blockSize != 0 || height % blockSize != 0)
        {
            T3D_LOG_ERROR(tag, "%s : size [%u x %u] of compressed format [%s] must be a multiple of %u !",
                func, width, height, name, blockSize);
            return false;
        }

        if (sampleCount > 1)
        {
            T3D_LOG_ERROR(tag, "%s : compressed format [%s] cannot be multisampled !", func, name);
            return false;
        }

        if (wantsUAV)
        {
            T3D_LOG_ERROR(tag, "%s : compressed format [%s] cannot have unordered access !", func, name);
            return false;
        }

        return true;
    }

    //--------------------------------------------------------------------------

    bool RHICapabilities::checkRenderTextureLayout(const char *tag, const char *func, const PixelBuffer2DDesc &desc,
        uint32_t gpuAccess) const
    {
        const char *name = PixelFormatUtil::getName(desc.format);
        const uint32_t arraySize = desc.arraySize == 0 ? 1 : desc.arraySize;
        const uint32_t mipmaps = desc.mipmaps == 0 ? 1 : desc.mipmaps;
        const uint32_t sampleCount = desc.sampleDesc.Count;

        if (desc.cubemap)
        {
            if (arraySize % 6 != 0)
            {
                T3D_LOG_ERROR(tag, "%s : cube render texture needs an array size of 6 * N, got %u !", func, arraySize);
                return false;
            }
            if (desc.width != desc.height)
            {
                T3D_LOG_ERROR(tag, "%s : cube render texture must be square, got [%u x %u] !", func, desc.width, desc.height);
                return false;
            }
        }

        if (sampleCount > 1 && (arraySize > 1 || desc.cubemap || mipmaps > 1))
        {
            T3D_LOG_ERROR(tag, "%s : MSAA render texture must be a single 2D layer with one mip "
                "(array size %u, cube %d, mipmaps %u) !", func, arraySize, desc.cubemap ? 1 : 0, mipmaps);
            return false;
        }

        const uint32_t fullMips = TextureSubresource::getFullMipCount(desc.width, desc.height);
        if (mipmaps > fullMips)
        {
            T3D_LOG_ERROR(tag, "%s : mipmaps %u exceeds the full mip chain %u of [%u x %u] !",
                func, mipmaps, fullMips, desc.width, desc.height);
            return false;
        }

        if ((gpuAccess & kGPUGenerateMips) != 0)
        {
            if (PixelFormatUtil::isDepthFormat(desc.format))
            {
                T3D_LOG_ERROR(tag, "%s : depth format [%s] cannot generate mips !", func, name);
                return false;
            }
            if (!supportsFormat(desc.format, kFormatSupportRenderTarget | kFormatSupportLinearFilter))
            {
                T3D_LOG_ERROR(tag, "%s : format [%s] must be a filterable render target to generate mips !", func, name);
                return false;
            }
        }

        return true;
    }

    //--------------------------------------------------------------------------

    bool RHICapabilities::checkGenerateMips(const char *tag, const char *func, PixelBuffer *buffer) const
    {
        if (buffer == nullptr || buffer->getRHIResource() == nullptr)
        {
            T3D_LOG_ERROR(tag, "%s : pixel buffer is null or has no RHI resource !", func);
            return false;
        }

        if (buffer->getType() != RenderResource::Type::kPixelBuffer2D
            || !static_cast<PixelBuffer2D *>(buffer)->isRenderTexture())
        {
            T3D_LOG_ERROR(tag, "%s : only render textures can generate mips, got resource type %u !",
                func, static_cast<uint32_t>(buffer->getType()));
            return false;
        }

        if ((buffer->getGPUAccess() & kGPUGenerateMips) == 0)
        {
            T3D_LOG_ERROR(tag, "%s : render texture was not created with kGPUGenerateMips !", func);
            return false;
        }

        const PixelBuffer2DDesc &desc = static_cast<PixelBuffer2D *>(buffer)->getDescriptor();
        if (desc.mipmaps <= 1)
        {
            T3D_LOG_WARNING(tag, "%s : render texture has only one mip, nothing to generate !", func);
            return false;
        }

        return true;
    }

    //--------------------------------------------------------------------------

    bool RHICapabilities::checkReadOnlyDepthStencil(const char *tag, const char *func, const RenderTarget *renderTarget) const
    {
        if (renderTarget == nullptr || renderTarget->getDepthStencilReadOnly() == kDepthStencilWritable)
        {
            return true;
        }

        if (!supportsReadOnlyDepthStencil)
        {
            T3D_LOG_ERROR(tag, "%s : read-only depth stencil is not supported by this RHI backend !", func);
            return false;
        }

        // 窗口自带的深度不能被采样，只读没有意义，所以只接受外挂的深度纹理
        RenderTexturePtr depthStencil = renderTarget->getDepthStencil();
        if (depthStencil == nullptr || depthStencil->getPixelBuffer() == nullptr)
        {
            T3D_LOG_ERROR(tag, "%s : read-only depth stencil needs a depth stencil render texture attached !", func);
            return false;
        }

        const PixelFormat format = depthStencil->getPixelFormat();
        if (!PixelFormatUtil::isDepthFormat(format))
        {
            T3D_LOG_ERROR(tag, "%s : read-only depth stencil attachment has non-depth format [%s] !",
                func, PixelFormatUtil::getName(format));
            return false;
        }

        return true;
    }

    //--------------------------------------------------------------------------

    uint32_t RHICapabilities::filterDepthStencilClear(const char *tag, const char *func, const RenderTarget *renderTarget,
        uint32_t parts)
    {
        if (renderTarget == nullptr)
        {
            return parts;
        }

        const uint32_t skipped = parts & renderTarget->getDepthStencilReadOnly();
        if (skipped != 0)
        {
            T3D_LOG_WARNING(tag, "%s : skip clearing the read-only %s%s of the render target !", func,
                (skipped & kReadOnlyDepth) != 0 ? "depth " : "", (skipped & kReadOnlyStencil) != 0 ? "stencil" : "");
        }
        return parts & ~skipped;
    }

    //--------------------------------------------------------------------------

    bool RHICapabilities::checkBlendDesc(const char *tag, const char *func, const BlendDesc &desc) const
    {
        auto isDualSource = [](BlendFactor factor)
        {
            return factor == BlendFactor::kSrc1Color || factor == BlendFactor::kOneMinusSrc1Color
                || factor == BlendFactor::kSrc1Alpha || factor == BlendFactor::kOneMinusSrc1Alpha;
        };

        // 不开独立混合时后端只读 RT 0，关掉混合的 RT 不读因子
        const uint32_t count = desc.IndependentBlendEnable ? BlendDesc::kMaxRenderTarget : 1;
        for (uint32_t i = 0; i < count; ++i)
        {
            const BlendDesc::RTBlendDesc &rt = desc.RenderTargetStates[i];
            if (!rt.BlendEnable)
            {
                continue;
            }

            if (!isDualSource(rt.SrcBlend) && !isDualSource(rt.DestBlend)
                && !isDualSource(rt.SrcBlendAlpha) && !isDualSource(rt.DstBlendAlpha))
            {
                continue;
            }

            if (!supportsDualSourceBlend)
            {
                T3D_LOG_ERROR(tag, "%s : dual source blending is not supported by this RHI backend !", func);
                return false;
            }

            if (i != 0)
            {
                T3D_LOG_ERROR(tag, "%s : dual source blend factors can only be used on render target 0, got render target %u !",
                    func, i);
                return false;
            }
        }

        return true;
    }

    //--------------------------------------------------------------------------

    bool RHICapabilities::supportsQuery(QueryType type) const
    {
        switch (type)
        {
        case QueryType::kOcclusion:             return supportsOcclusionQuery;
        case QueryType::kBinaryOcclusion:       return supportsBinaryOcclusionQuery;
        case QueryType::kTimestamp:             return supportsTimestampQuery;
        case QueryType::kPipelineStatistics:    return supportsPipelineStatisticsQuery;
        default:                                return false;
        }
    }

    //--------------------------------------------------------------------------

    bool RHICapabilities::checkQueryType(const char *tag, QueryType type) const
    {
        if (!supportsQuery(type))
        {
            T3D_LOG_WARNING(tag, "createQuery : %s query is not supported by this RHI backend !", getQueryTypeName(type));
            return false;
        }
        return true;
    }

    //--------------------------------------------------------------------------

    void RHICapabilities::logFormatSupport(const char *tag) const
    {
        // 一个字母一位，不支持的位打 '-'，列宽固定方便在日志里对齐比较
        static const struct
        {
            uint32_t flag;
            char     letter;
        } kColumns[] =
        {
            { kFormatSupportTexture,         'T' },
            { kFormatSupportLinearFilter,    'F' },
            { kFormatSupportRenderTarget,    'R' },
            { kFormatSupportBlend,           'B' },
            { kFormatSupportMultisample,     'M' },
            { kFormatSupportDepthStencil,    'D' },
            { kFormatSupportUnorderedAccess, 'U' },
            { kFormatSupportTypedBuffer,     'Y' },
        };
        constexpr size_t kColumnCount = sizeof(kColumns) / sizeof(kColumns[0]);

        T3D_LOG_INFO(tag, "Pixel format support (T=texture F=linear filter R=render target B=blend "
            "M=multisample D=depth stencil U=UAV Y=typed buffer):");

        for (uint32_t i = 1; i < kPixelFormatCount; ++i)
        {
            char bits[kColumnCount + 1];
            for (size_t c = 0; c < kColumnCount; ++c)
            {
                bits[c] = (formatSupport[i] & kColumns[c].flag) != 0 ? kColumns[c].letter : '-';
            }
            bits[kColumnCount] = '\0';

            T3D_LOG_INFO(tag, "  %-26s %s", PixelFormatUtil::getName(static_cast<PixelFormat>(i)), bits);
        }
    }

    //--------------------------------------------------------------------------
}
