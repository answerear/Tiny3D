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
