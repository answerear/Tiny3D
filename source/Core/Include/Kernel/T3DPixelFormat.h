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

#ifndef __T3D_PIXEL_FORMAT_H__
#define __T3D_PIXEL_FORMAT_H__


#include "T3DPrerequisites.h"
#include "Kernel/T3DConstant.h"


namespace Tiny3D
{
    /// PixelFormat 的取值个数，按格式下标的表用它定长
    constexpr uint32_t kPixelFormatCount = static_cast<uint32_t>(PixelFormat::E_PF_MAX);

    /**
     * \brief 像素格式各通道的数值类型
     */
    enum class PixelFormatType : uint32_t
    {
        kUnknown = 0,       ///< E_PF_UNKNOWN 与越界值
        kUNorm,             ///< 无符号归一化（含调色板索引与 16 位打包格式）
        kFloat,             ///< 浮点
        kUInt,              ///< 无符号整数
        kDepthStencil,      ///< 深度 / 深度模板
    };

    /**
     * \brief 像素格式的静态属性
     */
    struct PixelFormatInfo
    {
        PixelFormat     format;         ///< 本条目对应的格式
        const char      *name;          ///< 与枚举名一致，用于日志
        uint32_t        bytesPerPixel;  ///< CPU 侧每像素字节数，0 表示未知格式
        uint32_t        channels;       ///< 颜色通道数，深度模板格式为 0
        PixelFormatType type;           ///< 通道数值类型
        bool            hasStencil;     ///< 是否带模板
    };

    /**
     * \brief 与后端无关的像素格式属性查询
     * \remarks 各后端的映射表只负责「引擎格式 → 原生格式」，每像素字节数、
     *          是否深度 / 模板 / 整数这类属性统一从这里取，避免各后端各写一份。
     */
    class T3D_ENGINE_API PixelFormatUtil
    {
    public:
        /// 越界值返回 E_PF_UNKNOWN 的条目
        static const PixelFormatInfo &getInfo(PixelFormat format);

        static uint32_t getBytesPerPixel(PixelFormat format) { return getInfo(format).bytesPerPixel; }

        static uint32_t getChannelCount(PixelFormat format) { return getInfo(format).channels; }

        static const char *getName(PixelFormat format) { return getInfo(format).name; }

        static bool isDepthFormat(PixelFormat format) { return getInfo(format).type == PixelFormatType::kDepthStencil; }

        static bool hasStencil(PixelFormat format) { return getInfo(format).hasStencil; }

        static bool isFloatFormat(PixelFormat format) { return getInfo(format).type == PixelFormatType::kFloat; }

        static bool isIntegerFormat(PixelFormat format) { return getInfo(format).type == PixelFormatType::kUInt; }
    };
}


#endif  /*__T3D_PIXEL_FORMAT_H__*/
