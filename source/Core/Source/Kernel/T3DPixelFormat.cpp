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
        { PixelFormat::_fmt_, #_fmt_, _bpp_, _channels_, PixelFormatType::_type_, _stencil_ }

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

            T3D_PF_INFO(E_PF_D24_UNORM_S8_UINT,     4,  0, kDepthStencil,   true),
            T3D_PF_INFO(E_PF_D32_FLOAT_S8X24_UINT,  8,  0, kDepthStencil,   true),
            T3D_PF_INFO(E_PF_D32_FLOAT,             4,  0, kDepthStencil,   false),
            T3D_PF_INFO(E_PF_D16_UNORM,             2,  0, kDepthStencil,   false),
        };

#undef T3D_PF_INFO

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
}
