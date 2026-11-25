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

#include "Kernel/T3DPixelFormat.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    namespace
    {
#define T3D_PF_INFO(_fmt_, _bpp_, _channels_, _type_, _stencil_) \
        { PixelFormat::_fmt_, #_fmt_, _bpp_, _channels_, PixelFormatType::_type_, _stencil_, false, 1, _bpp_ }

#define T3D_PF_SRGB(_fmt_, _bpp_, _channels_) \
        { PixelFormat::_fmt_, #_fmt_, _bpp_, _channels_, PixelFormatType::kUNorm, false, true, 1, _bpp_ }

        // 目前只有 4x4 的 BC 格式；bytesPerPixel 填 0，按像素算字节数的旧路径会直接拒绝它们
#define T3D_PF_BLOCK(_fmt_, _bytesPerBlock_, _channels_, _type_, _srgb_) \
        { PixelFormat::_fmt_, #_fmt_, 0, _channels_, PixelFormatType::_type_, false, _srgb_, 4, _bytesPerBlock_ }

        // 按枚举值顺序排列，下标即枚举值，getInfo 里有断言核对
        const PixelFormatInfo kPixelFormatInfos[] =
        {
            T3D_PF_INFO(E_PF_UNKNOWN,               0,  0, kUnknown,        false),

            T3D_PF_INFO(E_PF_PALETTE8,              1,  1, kUNorm,          false),
            T3D_PF_INFO(E_PF_B5G6R5,                2,  3, kUNorm,          false),
            T3D_PF_INFO(E_PF_B5G5R5A1,              2,  4, kUNorm,          false),
            T3D_PF_INFO(E_PF_B4R4G4A4,              2,  4, kUNorm,          false),

            T3D_PF_INFO(E_PF_R8_UNORM,              1,  1, kUNorm,          false),
            T3D_PF_INFO(E_PF_R8G8_UNORM,            2,  2, kUNorm,          false),
            T3D_PF_INFO(E_PF_R8G8B8,                3,  3, kUNorm,          false),
            T3D_PF_INFO(E_PF_B8G8R8,                3,  3, kUNorm,          false),
            T3D_PF_INFO(E_PF_R8G8B8A8,              4,  4, kUNorm,          false),
            T3D_PF_INFO(E_PF_B8G8R8A8,              4,  4, kUNorm,          false),
            T3D_PF_INFO(E_PF_R8G8B8X8,              4,  3, kUNorm,          false),
            T3D_PF_INFO(E_PF_B8G8R8X8,              4,  3, kUNorm,          false),

            T3D_PF_SRGB(E_PF_R8G8B8A8_SRGB,         4,  4),
            T3D_PF_SRGB(E_PF_B8G8R8A8_SRGB,         4,  4),

            T3D_PF_INFO(E_PF_R16_UNORM,             2,  1, kUNorm,          false),
            T3D_PF_INFO(E_PF_R16G16_UNORM,          4,  2, kUNorm,          false),

            T3D_PF_INFO(E_PF_R10G10B10A2_UNORM,     4,  4, kUNorm,          false),

            T3D_PF_INFO(E_PF_R16_FLOAT,             2,  1, kFloat,          false),
            T3D_PF_INFO(E_PF_R16G16_FLOAT,          4,  2, kFloat,          false),
            T3D_PF_INFO(E_PF_R16G16B16A16_FLOAT,    8,  4, kFloat,          false),
            T3D_PF_INFO(E_PF_R32_FLOAT,             4,  1, kFloat,          false),
            T3D_PF_INFO(E_PF_R32G32_FLOAT,          8,  2, kFloat,          false),
            T3D_PF_INFO(E_PF_R32G32B32A32_FLOAT,    16, 4, kFloat,          false),
            T3D_PF_INFO(E_PF_R11G11B10_FLOAT,       4,  3, kFloat,          false),

            T3D_PF_INFO(E_PF_R8G8B8A8_UINT,         4,  4, kUInt,           false),
            T3D_PF_INFO(E_PF_R16G16_UINT,           4,  2, kUInt,           false),
            T3D_PF_INFO(E_PF_R32_UINT,              4,  1, kUInt,           false),

            T3D_PF_BLOCK(E_PF_BC1_UNORM,            8,  4, kUNorm,          false),
            T3D_PF_BLOCK(E_PF_BC1_SRGB,             8,  4, kUNorm,          true),
            T3D_PF_BLOCK(E_PF_BC2_UNORM,            16, 4, kUNorm,          false),
            T3D_PF_BLOCK(E_PF_BC2_SRGB,             16, 4, kUNorm,          true),
            T3D_PF_BLOCK(E_PF_BC3_UNORM,            16, 4, kUNorm,          false),
            T3D_PF_BLOCK(E_PF_BC3_SRGB,             16, 4, kUNorm,          true),
            T3D_PF_BLOCK(E_PF_BC4_UNORM,            8,  1, kUNorm,          false),
            T3D_PF_BLOCK(E_PF_BC4_SNORM,            8,  1, kSNorm,          false),
            T3D_PF_BLOCK(E_PF_BC5_UNORM,            16, 2, kUNorm,          false),
            T3D_PF_BLOCK(E_PF_BC5_SNORM,            16, 2, kSNorm,          false),
            T3D_PF_BLOCK(E_PF_BC6H_UF16,            16, 3, kFloat,          false),
            T3D_PF_BLOCK(E_PF_BC6H_SF16,            16, 3, kFloat,          false),
            T3D_PF_BLOCK(E_PF_BC7_UNORM,            16, 4, kUNorm,          false),
            T3D_PF_BLOCK(E_PF_BC7_SRGB,             16, 4, kUNorm,          true),

            T3D_PF_INFO(E_PF_D24_UNORM_S8_UINT,     4,  0, kDepthStencil,   true),
            T3D_PF_INFO(E_PF_D32_FLOAT_S8X24_UINT,  8,  0, kDepthStencil,   true),
            T3D_PF_INFO(E_PF_D32_FLOAT,             4,  0, kDepthStencil,   false),
            T3D_PF_INFO(E_PF_D16_UNORM,             2,  0, kDepthStencil,   false),
        };

#undef T3D_PF_INFO
#undef T3D_PF_SRGB
#undef T3D_PF_BLOCK

        struct SRGBPair
        {
            PixelFormat linear;
            PixelFormat srgb;
        };

        const SRGBPair kSRGBPairs[] =
        {
            { PixelFormat::E_PF_R8G8B8A8,   PixelFormat::E_PF_R8G8B8A8_SRGB },
            { PixelFormat::E_PF_B8G8R8A8,   PixelFormat::E_PF_B8G8R8A8_SRGB },
            { PixelFormat::E_PF_BC1_UNORM,  PixelFormat::E_PF_BC1_SRGB },
            { PixelFormat::E_PF_BC2_UNORM,  PixelFormat::E_PF_BC2_SRGB },
            { PixelFormat::E_PF_BC3_UNORM,  PixelFormat::E_PF_BC3_SRGB },
            { PixelFormat::E_PF_BC7_UNORM,  PixelFormat::E_PF_BC7_SRGB },
        };

        static_assert(sizeof(kPixelFormatInfos) / sizeof(kPixelFormatInfos[0]) == kPixelFormatCount,
            "kPixelFormatInfos must have one entry per PixelFormat");
    }

    //--------------------------------------------------------------------------

    const PixelFormatInfo &PixelFormatUtil::getInfo(PixelFormat format)
    {
        const uint32_t index = static_cast<uint32_t>(format);
        if (index >= kPixelFormatCount)
        {
            return kPixelFormatInfos[0];
        }

        T3D_ASSERT(kPixelFormatInfos[index].format == format);
        return kPixelFormatInfos[index];
    }

    //--------------------------------------------------------------------------

    PixelFormat PixelFormatUtil::toLinear(PixelFormat format)
    {
        for (const auto &pair : kSRGBPairs)
        {
            if (pair.srgb == format)
            {
                return pair.linear;
            }
        }
        return format;
    }

    //--------------------------------------------------------------------------

    PixelFormat PixelFormatUtil::toSRGB(PixelFormat format)
    {
        for (const auto &pair : kSRGBPairs)
        {
            if (pair.linear == format)
            {
                return pair.srgb;
            }
        }
        return format;
    }

    //--------------------------------------------------------------------------

    size_t PixelFormatUtil::getRowPitch(PixelFormat format, uint32_t width)
    {
        const PixelFormatInfo &info = getInfo(format);
        const uint32_t units = (width + info.blockSize - 1) / info.blockSize;
        return static_cast<size_t>(units) * info.bytesPerBlock;
    }

    //--------------------------------------------------------------------------

    uint32_t PixelFormatUtil::getRowCount(PixelFormat format, uint32_t height)
    {
        const uint32_t blockSize = getInfo(format).blockSize;
        return (height + blockSize - 1) / blockSize;
    }

    //--------------------------------------------------------------------------

    size_t PixelFormatUtil::getSliceSize(PixelFormat format, uint32_t width, uint32_t height)
    {
        return getRowPitch(format, width) * getRowCount(format, height);
    }

    //--------------------------------------------------------------------------

    size_t PixelFormatUtil::getMipChainSize(PixelFormat format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels)
    {
        size_t total = 0;
        for (uint32_t level = 0; level < std::max<uint32_t>(1, mipLevels); ++level)
        {
            const uint32_t mipW = std::max<uint32_t>(1, width >> level);
            const uint32_t mipH = std::max<uint32_t>(1, height >> level);
            const uint32_t mipD = std::max<uint32_t>(1, depth >> level);
            total += getSliceSize(format, mipW, mipH) * mipD;
        }
        return total;
    }

    //--------------------------------------------------------------------------
}
