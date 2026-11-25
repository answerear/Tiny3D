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


#include "T3DBlockCompressor.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>


namespace Tiny3D
{
    namespace
    {
        /// 4x4 块的 RGBA 像素，行优先
        struct Block
        {
            uint8_t px[16][4];
        };

        /// 端点细化迭代次数：每次按当前索引做最小二乘求端点，保留误差最小的一次
        const uint32_t kRefineIterations = 3;

        /// BC7 4 位索引的插值权重（满值 64）
        const int kBC7Weights4[16] = { 0, 4, 9, 13, 17, 21, 26, 30, 34, 38, 43, 47, 51, 55, 60, 64 };

        template <typename T>
        T clampValue(T v, T lo, T hi)
        {
            return v < lo ? lo : (v > hi ? hi : v);
        }

        /// 取出一个块，图像边缘不足 4 像素时复制最后一行 / 列
        void fetchBlock(const uint8_t *bgra, uint32_t width, uint32_t height, uint32_t bx, uint32_t by, Block &block)
        {
            for (uint32_t y = 0; y < 4; ++y)
            {
                const uint32_t sy = std::min(by * 4 + y, height - 1);
                for (uint32_t x = 0; x < 4; ++x)
                {
                    const uint32_t sx = std::min(bx * 4 + x, width - 1);
                    const uint8_t *src = bgra + (static_cast<size_t>(sy) * width + sx) * 4;
                    uint8_t *dst = block.px[y * 4 + x];
                    dst[0] = src[2];
                    dst[1] = src[1];
                    dst[2] = src[0];
                    dst[3] = src[3];
                }
            }
        }

        /// 求点集的均值与主成分方向（幂迭代）。点完全重合时 axis 为 0
        void principalAxis(const float pts[][4], uint32_t count, uint32_t dims, float mean[4], float axis[4])
        {
            for (uint32_t c = 0; c < 4; ++c)
            {
                mean[c] = 0.0f;
                axis[c] = 0.0f;
            }
            for (uint32_t i = 0; i < count; ++i)
            {
                for (uint32_t c = 0; c < dims; ++c)
                {
                    mean[c] += pts[i][c];
                }
            }
            for (uint32_t c = 0; c < dims; ++c)
            {
                mean[c] /= static_cast<float>(count);
            }

            float cov[4][4] = {};
            for (uint32_t i = 0; i < count; ++i)
            {
                float d[4] = {};
                for (uint32_t c = 0; c < dims; ++c)
                {
                    d[c] = pts[i][c] - mean[c];
                }
                for (uint32_t a = 0; a < dims; ++a)
                {
                    for (uint32_t b = 0; b < dims; ++b)
                    {
                        cov[a][b] += d[a] * d[b];
                    }
                }
            }

            // 从方差最大的那一列起步，避免初值恰好与主方向正交
            uint32_t start = 0;
            for (uint32_t c = 1; c < dims; ++c)
            {
                if (cov[c][c] > cov[start][start])
                {
                    start = c;
                }
            }
            if (cov[start][start] <= 0.0f)
            {
                return;
            }

            float v[4] = {};
            for (uint32_t c = 0; c < dims; ++c)
            {
                v[c] = cov[c][start];
            }

            for (uint32_t iter = 0; iter < 8; ++iter)
            {
                float next[4] = {};
                float maxAbs = 0.0f;
                for (uint32_t a = 0; a < dims; ++a)
                {
                    for (uint32_t b = 0; b < dims; ++b)
                    {
                        next[a] += cov[a][b] * v[b];
                    }
                    maxAbs = std::max(maxAbs, std::fabs(next[a]));
                }
                if (maxAbs <= 0.0f)
                {
                    return;
                }
                for (uint32_t c = 0; c < dims; ++c)
                {
                    v[c] = next[c] / maxAbs;
                }
            }

            float len = 0.0f;
            for (uint32_t c = 0; c < dims; ++c)
            {
                len += v[c] * v[c];
            }
            len = std::sqrt(len);
            for (uint32_t c = 0; c < dims; ++c)
            {
                axis[c] = v[c] / len;
            }
        }

        /// 沿主方向取投影最远的两点作为初始端点
        void initialEndpoints(const float pts[][4], uint32_t count, uint32_t dims, float e0[4], float e1[4])
        {
            float mean[4];
            float axis[4];
            principalAxis(pts, count, dims, mean, axis);

            float tMin = 0.0f;
            float tMax = 0.0f;
            for (uint32_t i = 0; i < count; ++i)
            {
                float t = 0.0f;
                for (uint32_t c = 0; c < dims; ++c)
                {
                    t += (pts[i][c] - mean[c]) * axis[c];
                }
                tMin = std::min(tMin, t);
                tMax = std::max(tMax, t);
            }

            for (uint32_t c = 0; c < 4; ++c)
            {
                e0[c] = clampValue(mean[c] + axis[c] * tMax, 0.0f, 255.0f);
                e1[c] = clampValue(mean[c] + axis[c] * tMin, 0.0f, 255.0f);
            }
        }

        /// 已知每点在 e0 上的权重 w（e1 上为 1 - w），最小二乘求两个端点。矩阵奇异时返回 false
        bool solveEndpoints(const float pts[][4], const float *weights, uint32_t count, uint32_t dims,
            float e0[4], float e1[4])
        {
            float aa = 0.0f, bb = 0.0f, ab = 0.0f;
            float ax[4] = {};
            float bx[4] = {};
            for (uint32_t i = 0; i < count; ++i)
            {
                const float w = weights[i];
                const float iw = 1.0f - w;
                aa += w * w;
                bb += iw * iw;
                ab += w * iw;
                for (uint32_t c = 0; c < dims; ++c)
                {
                    ax[c] += w * pts[i][c];
                    bx[c] += iw * pts[i][c];
                }
            }

            const float det = aa * bb - ab * ab;
            if (std::fabs(det) < 1e-6f)
            {
                return false;
            }

            for (uint32_t c = 0; c < dims; ++c)
            {
                e0[c] = clampValue((ax[c] * bb - bx[c] * ab) / det, 0.0f, 255.0f);
                e1[c] = clampValue((bx[c] * aa - ax[c] * ab) / det, 0.0f, 255.0f);
            }
            return true;
        }

        uint16_t packRGB565(const float c[3])
        {
            const int r = clampValue(static_cast<int>(c[0] * 31.0f / 255.0f + 0.5f), 0, 31);
            const int g = clampValue(static_cast<int>(c[1] * 63.0f / 255.0f + 0.5f), 0, 63);
            const int b = clampValue(static_cast<int>(c[2] * 31.0f / 255.0f + 0.5f), 0, 31);
            return static_cast<uint16_t>((r << 11) | (g << 5) | b);
        }

        void unpackRGB565(uint16_t v, int out[3])
        {
            const int r = (v >> 11) & 31;
            const int g = (v >> 5) & 63;
            const int b = v & 31;
            out[0] = (r << 3) | (r >> 2);
            out[1] = (g << 2) | (g >> 4);
            out[2] = (b << 3) | (b >> 2);
        }

        void writeColorBlock(uint16_t c0, uint16_t c1, uint32_t indices, uint8_t out[8])
        {
            out[0] = static_cast<uint8_t>(c0 & 0xFF);
            out[1] = static_cast<uint8_t>(c0 >> 8);
            out[2] = static_cast<uint8_t>(c1 & 0xFF);
            out[3] = static_cast<uint8_t>(c1 >> 8);
            out[4] = static_cast<uint8_t>(indices & 0xFF);
            out[5] = static_cast<uint8_t>((indices >> 8) & 0xFF);
            out[6] = static_cast<uint8_t>((indices >> 16) & 0xFF);
            out[7] = static_cast<uint8_t>(indices >> 24);
        }

        /**
         * \brief 编码 BC1 / BC3 的颜色块
         * \param [in] allowTransparent : BC1 中 alpha < 128 的像素走三色模式的透明索引 3；
         *  BC2 / BC3 的颜色块总是四色模式，必须传 false
         */
        void encodeColorBlock(const Block &block, bool allowTransparent, uint8_t out[8])
        {
            bool transparent[16] = {};
            float pts[16][4] = {};
            uint32_t opaqueIndex[16] = {};
            uint32_t count = 0;
            for (uint32_t i = 0; i < 16; ++i)
            {
                transparent[i] = allowTransparent && block.px[i][3] < 128;
                if (!transparent[i])
                {
                    pts[count][0] = block.px[i][0];
                    pts[count][1] = block.px[i][1];
                    pts[count][2] = block.px[i][2];
                    opaqueIndex[count] = i;
                    ++count;
                }
            }

            if (count == 0)
            {
                // 全透明：c0 <= c1 进入三色模式，全部取索引 3
                writeColorBlock(0, 0, 0xFFFFFFFFu, out);
                return;
            }

            const bool threeColor = (count < 16);

            float e0[4];
            float e1[4];
            initialEndpoints(pts, count, 3, e0, e1);

            uint64_t bestErr = std::numeric_limits<uint64_t>::max();
            uint16_t bestC0 = 0;
            uint16_t bestC1 = 0;
            uint32_t bestIndices = 0;

            for (uint32_t iter = 0; iter < kRefineIterations; ++iter)
            {
                uint16_t c0 = packRGB565(e0);
                uint16_t c1 = packRGB565(e1);
                // 四色模式要求 c0 > c1，三色模式要求 c0 <= c1
                if (threeColor ? (c0 > c1) : (c0 < c1))
                {
                    std::swap(c0, c1);
                }
                // c0 == c1 时解码器按三色模式处理，索引 3 会变成黑色 / 透明，不能用于不透明像素
                const bool paletteHas3 = threeColor || c0 == c1;

                int pal[4][3];
                unpackRGB565(c0, pal[0]);
                unpackRGB565(c1, pal[1]);
                for (uint32_t c = 0; c < 3; ++c)
                {
                    if (paletteHas3)
                    {
                        pal[2][c] = (pal[0][c] + pal[1][c]) / 2;
                        pal[3][c] = 0;
                    }
                    else
                    {
                        pal[2][c] = (2 * pal[0][c] + pal[1][c]) / 3;
                        pal[3][c] = (pal[0][c] + 2 * pal[1][c]) / 3;
                    }
                }
                const uint32_t usable = paletteHas3 ? 3u : 4u;

                uint32_t indices = 0;
                uint64_t err = 0;
                uint32_t pointIndex[16] = {};
                for (uint32_t i = 0; i < 16; ++i)
                {
                    if (transparent[i])
                    {
                        indices |= 3u << (2 * i);
                        continue;
                    }
                    uint32_t best = 0;
                    int bestDist = std::numeric_limits<int>::max();
                    for (uint32_t p = 0; p < usable; ++p)
                    {
                        int dist = 0;
                        for (uint32_t c = 0; c < 3; ++c)
                        {
                            const int d = static_cast<int>(block.px[i][c]) - pal[p][c];
                            dist += d * d;
                        }
                        if (dist < bestDist)
                        {
                            bestDist = dist;
                            best = p;
                        }
                    }
                    indices |= best << (2 * i);
                    err += static_cast<uint64_t>(bestDist);
                    pointIndex[i] = best;
                }

                if (err < bestErr)
                {
                    bestErr = err;
                    bestC0 = c0;
                    bestC1 = c1;
                    bestIndices = indices;
                }
                if (err == 0 || iter + 1 == kRefineIterations)
                {
                    break;
                }

                // 按本轮索引求新的端点，e0 对应 c0 槽位
                float weights[16];
                for (uint32_t k = 0; k < count; ++k)
                {
                    const uint32_t idx = pointIndex[opaqueIndex[k]];
                    if (paletteHas3)
                    {
                        weights[k] = (idx == 0) ? 1.0f : (idx == 1 ? 0.0f : 0.5f);
                    }
                    else
                    {
                        static const float kW4[4] = { 1.0f, 0.0f, 2.0f / 3.0f, 1.0f / 3.0f };
                        weights[k] = kW4[idx];
                    }
                }
                if (!solveEndpoints(pts, weights, count, 3, e0, e1))
                {
                    break;
                }
            }

            writeColorBlock(bestC0, bestC1, bestIndices, out);
        }

        /// 编码 BC4 单通道块（BC3 的 alpha、BC5 的两个通道也用它），使用 a0 > a1 的 8 值模式
        void encodeSingleChannelBlock(const uint8_t values[16], uint8_t out[8])
        {
            uint8_t minV = 255;
            uint8_t maxV = 0;
            for (uint32_t i = 0; i < 16; ++i)
            {
                minV = std::min(minV, values[i]);
                maxV = std::max(maxV, values[i]);
            }

            std::memset(out, 0, 8);
            out[0] = maxV;
            out[1] = minV;
            if (maxV == minV)
            {
                return;
            }

            int pal[8];
            pal[0] = maxV;
            pal[1] = minV;
            for (int i = 1; i <= 6; ++i)
            {
                pal[i + 1] = ((7 - i) * maxV + i * minV + 3) / 7;
            }

            uint64_t bits = 0;
            for (uint32_t i = 0; i < 16; ++i)
            {
                uint32_t best = 0;
                int bestDist = std::numeric_limits<int>::max();
                for (uint32_t p = 0; p < 8; ++p)
                {
                    const int d = std::abs(static_cast<int>(values[i]) - pal[p]);
                    if (d < bestDist)
                    {
                        bestDist = d;
                        best = p;
                    }
                }
                bits |= static_cast<uint64_t>(best) << (3 * i);
            }

            for (uint32_t b = 0; b < 6; ++b)
            {
                out[2 + b] = static_cast<uint8_t>((bits >> (8 * b)) & 0xFF);
            }
        }

        void extractChannel(const Block &block, uint32_t channel, uint8_t values[16])
        {
            for (uint32_t i = 0; i < 16; ++i)
            {
                values[i] = block.px[i][channel];
            }
        }

        /// 128 位小端位流写入器
        struct BitWriter
        {
            uint8_t *data;
            uint32_t pos;

            void write(uint32_t value, uint32_t bits)
            {
                for (uint32_t b = 0; b < bits; ++b)
                {
                    if ((value >> b) & 1u)
                    {
                        data[pos >> 3] |= static_cast<uint8_t>(1u << (pos & 7));
                    }
                    ++pos;
                }
            }
        };

        /// BC7 mode 6 端点量化为 7 位 + 共享 p-bit，取误差最小的 p-bit
        void quantizeEndpointMode6(const float e[4], int q[4], int &pbit)
        {
            float bestErr = std::numeric_limits<float>::max();
            for (int p = 0; p < 2; ++p)
            {
                int tmp[4];
                float err = 0.0f;
                for (uint32_t c = 0; c < 4; ++c)
                {
                    tmp[c] = clampValue(static_cast<int>(std::floor((e[c] - p) * 0.5f + 0.5f)), 0, 127);
                    const float d = static_cast<float>(tmp[c] * 2 + p) - e[c];
                    err += d * d;
                }
                if (err < bestErr)
                {
                    bestErr = err;
                    pbit = p;
                    std::memcpy(q, tmp, sizeof(tmp));
                }
            }
        }

        /// 编码 BC7 块，只用 mode 6：单分区，RGBA 端点 7 位 + p-bit，4 位索引
        void encodeBC7Block(const Block &block, uint8_t out[16])
        {
            float pts[16][4];
            for (uint32_t i = 0; i < 16; ++i)
            {
                for (uint32_t c = 0; c < 4; ++c)
                {
                    pts[i][c] = block.px[i][c];
                }
            }

            float e0[4];
            float e1[4];
            initialEndpoints(pts, 16, 4, e0, e1);

            uint64_t bestErr = std::numeric_limits<uint64_t>::max();
            int bestQ0[4] = {};
            int bestQ1[4] = {};
            int bestP0 = 0;
            int bestP1 = 0;
            uint32_t bestIdx[16] = {};

            for (uint32_t iter = 0; iter < kRefineIterations; ++iter)
            {
                int q0[4];
                int q1[4];
                int p0 = 0;
                int p1 = 0;
                quantizeEndpointMode6(e0, q0, p0);
                quantizeEndpointMode6(e1, q1, p1);

                int v0[4];
                int v1[4];
                for (uint32_t c = 0; c < 4; ++c)
                {
                    v0[c] = q0[c] * 2 + p0;
                    v1[c] = q1[c] * 2 + p1;
                }

                int pal[16][4];
                for (uint32_t k = 0; k < 16; ++k)
                {
                    const int w = kBC7Weights4[k];
                    for (uint32_t c = 0; c < 4; ++c)
                    {
                        pal[k][c] = ((64 - w) * v0[c] + w * v1[c] + 32) >> 6;
                    }
                }

                uint32_t idx[16];
                uint64_t err = 0;
                for (uint32_t i = 0; i < 16; ++i)
                {
                    uint32_t best = 0;
                    int bestDist = std::numeric_limits<int>::max();
                    for (uint32_t k = 0; k < 16; ++k)
                    {
                        int dist = 0;
                        for (uint32_t c = 0; c < 4; ++c)
                        {
                            const int d = static_cast<int>(block.px[i][c]) - pal[k][c];
                            dist += d * d;
                        }
                        if (dist < bestDist)
                        {
                            bestDist = dist;
                            best = k;
                        }
                    }
                    idx[i] = best;
                    err += static_cast<uint64_t>(bestDist);
                }

                if (err < bestErr)
                {
                    bestErr = err;
                    std::memcpy(bestQ0, q0, sizeof(q0));
                    std::memcpy(bestQ1, q1, sizeof(q1));
                    bestP0 = p0;
                    bestP1 = p1;
                    std::memcpy(bestIdx, idx, sizeof(idx));
                }
                if (err == 0 || iter + 1 == kRefineIterations)
                {
                    break;
                }

                // weights 是 e0 上的权重
                float weights[16];
                for (uint32_t i = 0; i < 16; ++i)
                {
                    weights[i] = 1.0f - static_cast<float>(kBC7Weights4[idx[i]]) / 64.0f;
                }
                if (!solveEndpoints(pts, weights, 16, 4, e0, e1))
                {
                    break;
                }
            }

            // 锚点（像素 0）的索引只存 3 位，最高位必须为 0，否则交换端点并反转索引
            if ((bestIdx[0] & 8u) != 0)
            {
                for (uint32_t c = 0; c < 4; ++c)
                {
                    std::swap(bestQ0[c], bestQ1[c]);
                }
                std::swap(bestP0, bestP1);
                for (uint32_t i = 0; i < 16; ++i)
                {
                    bestIdx[i] = 15u - bestIdx[i];
                }
            }

            std::memset(out, 0, 16);
            BitWriter writer { out, 0 };
            // mode 6：6 个 0 后跟 1
            writer.write(1u << 6, 7);
            for (uint32_t c = 0; c < 4; ++c)
            {
                writer.write(static_cast<uint32_t>(bestQ0[c]), 7);
                writer.write(static_cast<uint32_t>(bestQ1[c]), 7);
            }
            writer.write(static_cast<uint32_t>(bestP0), 1);
            writer.write(static_cast<uint32_t>(bestP1), 1);
            writer.write(bestIdx[0], 3);
            for (uint32_t i = 1; i < 16; ++i)
            {
                writer.write(bestIdx[i], 4);
            }
        }
    }

    //--------------------------------------------------------------------------

    bool BlockCompressor::isSupported(PixelFormat format)
    {
        switch (format)
        {
        case PixelFormat::E_PF_BC1_UNORM:
        case PixelFormat::E_PF_BC1_SRGB:
        case PixelFormat::E_PF_BC3_UNORM:
        case PixelFormat::E_PF_BC3_SRGB:
        case PixelFormat::E_PF_BC4_UNORM:
        case PixelFormat::E_PF_BC5_UNORM:
        case PixelFormat::E_PF_BC7_UNORM:
        case PixelFormat::E_PF_BC7_SRGB:
            return true;
        default:
            return false;
        }
    }

    //--------------------------------------------------------------------------

    bool BlockCompressor::compress(const uint8_t *bgra, uint32_t width, uint32_t height, PixelFormat format, uint8_t *out)
    {
        if (!isSupported(format) || bgra == nullptr || out == nullptr || width == 0 || height == 0)
        {
            return false;
        }

        const uint32_t blocksX = (width + 3) / 4;
        const uint32_t blocksY = (height + 3) / 4;
        const uint32_t bytesPerBlock = PixelFormatUtil::getBytesPerBlock(format);

        Block block;
        uint8_t values[16];
        for (uint32_t by = 0; by < blocksY; ++by)
        {
            for (uint32_t bx = 0; bx < blocksX; ++bx)
            {
                fetchBlock(bgra, width, height, bx, by, block);
                uint8_t *dst = out + (static_cast<size_t>(by) * blocksX + bx) * bytesPerBlock;

                switch (format)
                {
                case PixelFormat::E_PF_BC1_UNORM:
                case PixelFormat::E_PF_BC1_SRGB:
                    encodeColorBlock(block, true, dst);
                    break;
                case PixelFormat::E_PF_BC3_UNORM:
                case PixelFormat::E_PF_BC3_SRGB:
                    extractChannel(block, 3, values);
                    encodeSingleChannelBlock(values, dst);
                    encodeColorBlock(block, false, dst + 8);
                    break;
                case PixelFormat::E_PF_BC4_UNORM:
                    extractChannel(block, 0, values);
                    encodeSingleChannelBlock(values, dst);
                    break;
                case PixelFormat::E_PF_BC5_UNORM:
                    extractChannel(block, 0, values);
                    encodeSingleChannelBlock(values, dst);
                    extractChannel(block, 1, values);
                    encodeSingleChannelBlock(values, dst + 8);
                    break;
                case PixelFormat::E_PF_BC7_UNORM:
                case PixelFormat::E_PF_BC7_SRGB:
                    encodeBC7Block(block, dst);
                    break;
                default:
                    return false;
                }
            }
        }

        return true;
    }
}
