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

#ifndef __T3D_BLOCK_COMPRESSOR_H__
#define __T3D_BLOCK_COMPRESSOR_H__


#include "T3DImageConverterPrerequisites.h"


namespace Tiny3D
{
    /**
     * \brief 离线块压缩编码器，支持 BC1 / BC3 / BC4 / BC5 / BC7
     * \remarks
     *  - 追求实现简单与结果稳定，不追求最高画质：BC1 / BC3 颜色块用主成分方向取端点再做最小二乘细化，
     *    BC7 只用单分区的 mode 6（RGBA 7.7.7.7 + 每端点 1 位 p-bit，4 位索引）；
     *  - BC6H 需要 HDR 输入，导入链路目前只有 8 位，未实现；
     *  - sRGB 变体与对应 UNORM 格式的块数据完全一致，由调用方选择 PixelFormat。
     */
    class BlockCompressor
    {
    public:
        /**
         * \brief 判断格式是否能被本编码器生成
         */
        static bool isSupported(PixelFormat format);

        /**
         * \brief 把一张紧密排列的 B8G8R8A8 图像压缩成指定块格式
         * \param [in] bgra : 像素数据，每像素 [B, G, R, A]
         * \param [in] width : 宽度，可以不是 4 的倍数（不足一块时复制边缘像素）
         * \param [in] height : 高度
         * \param [in] format : 目标块格式，BC4 取 R 通道，BC5 取 R / G 通道
         * \param [out] out : 输出缓冲，至少 PixelFormatUtil::getSliceSize(format, width, height) 字节
         * \return 格式不支持时返回 false
         */
        static bool compress(const uint8_t *bgra, uint32_t width, uint32_t height, PixelFormat format, uint8_t *out);
    };
}


#endif  /*__T3D_BLOCK_COMPRESSOR_H__*/
