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


#include "T3DImageImporter.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>


namespace Tiny3D
{
    namespace
    {
        void reportErr(String *outError, const String &msg)
        {
            if (outError != nullptr)
            {
                *outError = msg;
            }
            IMGCONV_LOG_ERROR("%s", msg.c_str());
        }

        /**
         * \brief 将 Image 任意像素格式统一转换为紧密排列的 B8G8R8A8 (每像素 4 字节: [B, G, R, A])
         */
        bool convertToBGRA8(const ImagePtr &image, std::vector<uint8_t> &outBGRA, String *outError)
        {
            const uint32_t width  = static_cast<uint32_t>(image->getWidth());
            const uint32_t height = static_cast<uint32_t>(image->getHeight());
            const size_t   pitch  = image->getPitch();
            const uint8_t *src    = image->getData();
            const PixelFormat fmt = image->getFormat();

            if (width == 0 || height == 0 || src == nullptr)
            {
                reportErr(outError, "Decoded image has invalid dimensions or null pixel buffer.");
                return false;
            }

            outBGRA.resize(static_cast<size_t>(width) * height * 4);

            for (uint32_t y = 0; y < height; ++y)
            {
                const uint8_t *srcRow = src + static_cast<size_t>(y) * pitch;
                uint8_t *dstRow = outBGRA.data() + static_cast<size_t>(y) * width * 4;

                switch (fmt)
                {
                case PixelFormat::E_PF_B8G8R8A8:
                case PixelFormat::E_PF_B8G8R8X8:
                    std::memcpy(dstRow, srcRow, static_cast<size_t>(width) * 4);
                    if (fmt == PixelFormat::E_PF_B8G8R8X8)
                    {
                        for (uint32_t x = 0; x < width; ++x)
                        {
                            dstRow[x * 4 + 3] = 255;
                        }
                    }
                    break;

                case PixelFormat::E_PF_R8G8B8A8:
                case PixelFormat::E_PF_R8G8B8X8:
                    for (uint32_t x = 0; x < width; ++x)
                    {
                        dstRow[x * 4 + 0] = srcRow[x * 4 + 2]; // B
                        dstRow[x * 4 + 1] = srcRow[x * 4 + 1]; // G
                        dstRow[x * 4 + 2] = srcRow[x * 4 + 0]; // R
                        dstRow[x * 4 + 3] = (fmt == PixelFormat::E_PF_R8G8B8X8) ? 255 : srcRow[x * 4 + 3];
                    }
                    break;

                case PixelFormat::E_PF_B8G8R8:
                    for (uint32_t x = 0; x < width; ++x)
                    {
                        dstRow[x * 4 + 0] = srcRow[x * 3 + 0]; // B
                        dstRow[x * 4 + 1] = srcRow[x * 3 + 1]; // G
                        dstRow[x * 4 + 2] = srcRow[x * 3 + 2]; // R
                        dstRow[x * 4 + 3] = 255;               // A
                    }
                    break;

                case PixelFormat::E_PF_R8G8B8:
                    for (uint32_t x = 0; x < width; ++x)
                    {
                        dstRow[x * 4 + 0] = srcRow[x * 3 + 2]; // B
                        dstRow[x * 4 + 1] = srcRow[x * 3 + 1]; // G
                        dstRow[x * 4 + 2] = srcRow[x * 3 + 0]; // R
                        dstRow[x * 4 + 3] = 255;               // A
                    }
                    break;

                case PixelFormat::E_PF_PALETTE8:
                    for (uint32_t x = 0; x < width; ++x)
                    {
                        uint8_t v = srcRow[x];
                        dstRow[x * 4 + 0] = v;
                        dstRow[x * 4 + 1] = v;
                        dstRow[x * 4 + 2] = v;
                        dstRow[x * 4 + 3] = 255;
                    }
                    break;

                case PixelFormat::E_PF_B5G6R5:
                    for (uint32_t x = 0; x < width; ++x)
                    {
                        uint16_t p = *reinterpret_cast<const uint16_t *>(srcRow + x * 2);
                        uint8_t b = static_cast<uint8_t>(((p & 0x001F) * 255 + 15) / 31);
                        uint8_t g = static_cast<uint8_t>((((p >> 5) & 0x003F) * 255 + 31) / 63);
                        uint8_t r = static_cast<uint8_t>((((p >> 11) & 0x001F) * 255 + 15) / 31);
                        dstRow[x * 4 + 0] = b;
                        dstRow[x * 4 + 1] = g;
                        dstRow[x * 4 + 2] = r;
                        dstRow[x * 4 + 3] = 255;
                    }
                    break;

                case PixelFormat::E_PF_B5G5R5A1:
                    for (uint32_t x = 0; x < width; ++x)
                    {
                        uint16_t p = *reinterpret_cast<const uint16_t *>(srcRow + x * 2);
                        uint8_t b = static_cast<uint8_t>(((p & 0x001F) * 255 + 15) / 31);
                        uint8_t g = static_cast<uint8_t>((((p >> 5) & 0x001F) * 255 + 15) / 31);
                        uint8_t r = static_cast<uint8_t>((((p >> 10) & 0x001F) * 255 + 15) / 31);
                        uint8_t a = (p & 0x8000) ? 255 : 0;
                        dstRow[x * 4 + 0] = b;
                        dstRow[x * 4 + 1] = g;
                        dstRow[x * 4 + 2] = r;
                        dstRow[x * 4 + 3] = a;
                    }
                    break;

                case PixelFormat::E_PF_B4R4G4A4:
                    for (uint32_t x = 0; x < width; ++x)
                    {
                        uint16_t p = *reinterpret_cast<const uint16_t *>(srcRow + x * 2);
                        dstRow[x * 4 + 0] = static_cast<uint8_t>((p & 0x000F) * 17);
                        dstRow[x * 4 + 1] = static_cast<uint8_t>(((p >> 4) & 0x000F) * 17);
                        dstRow[x * 4 + 2] = static_cast<uint8_t>(((p >> 8) & 0x000F) * 17);
                        dstRow[x * 4 + 3] = static_cast<uint8_t>(((p >> 12) & 0x000F) * 17);
                    }
                    break;

                default:
                    reportErr(outError, "Unsupported source pixel format for conversion to B8G8R8A8.");
                    return false;
                }
            }

            return true;
        }

        /**
         * \brief 垂直翻转 B8G8R8A8 像素缓冲区
         */
        void flipVerticalBGRA8(std::vector<uint8_t> &bgra, uint32_t width, uint32_t height)
        {
            const size_t rowBytes = static_cast<size_t>(width) * 4;
            std::vector<uint8_t> tempRow(rowBytes);
            for (uint32_t y = 0; y < height / 2; ++y)
            {
                uint8_t *top = bgra.data() + static_cast<size_t>(y) * rowBytes;
                uint8_t *bot = bgra.data() + static_cast<size_t>(height - 1 - y) * rowBytes;
                std::memcpy(tempRow.data(), top, rowBytes);
                std::memcpy(top, bot, rowBytes);
                std::memcpy(bot, tempRow.data(), rowBytes);
            }
        }

        /**
         * \brief 双线性插值缩放 B8G8R8A8 缓冲区
         */
        void resizeBilinearBGRA8(
            const std::vector<uint8_t> &src,
            uint32_t srcW,
            uint32_t srcH,
            std::vector<uint8_t> &dst,
            uint32_t dstW,
            uint32_t dstH)
        {
            dst.resize(static_cast<size_t>(dstW) * dstH * 4);

            for (uint32_t y = 0; y < dstH; ++y)
            {
                float srcY = ((static_cast<float>(y) + 0.5f) * static_cast<float>(srcH) / static_cast<float>(dstH)) - 0.5f;
                int32_t y0 = static_cast<int32_t>(std::floor(srcY));
                int32_t y1 = y0 + 1;
                float fy = srcY - static_cast<float>(y0);
                y0 = std::max<int32_t>(0, std::min<int32_t>(y0, static_cast<int32_t>(srcH) - 1));
                y1 = std::max<int32_t>(0, std::min<int32_t>(y1, static_cast<int32_t>(srcH) - 1));

                for (uint32_t x = 0; x < dstW; ++x)
                {
                    float srcX = ((static_cast<float>(x) + 0.5f) * static_cast<float>(srcW) / static_cast<float>(dstW)) - 0.5f;
                    int32_t x0 = static_cast<int32_t>(std::floor(srcX));
                    int32_t x1 = x0 + 1;
                    float fx = srcX - static_cast<float>(x0);
                    x0 = std::max<int32_t>(0, std::min<int32_t>(x0, static_cast<int32_t>(srcW) - 1));
                    x1 = std::max<int32_t>(0, std::min<int32_t>(x1, static_cast<int32_t>(srcW) - 1));

                    const uint8_t *p00 = src.data() + (static_cast<size_t>(y0) * srcW + x0) * 4;
                    const uint8_t *p10 = src.data() + (static_cast<size_t>(y0) * srcW + x1) * 4;
                    const uint8_t *p01 = src.data() + (static_cast<size_t>(y1) * srcW + x0) * 4;
                    const uint8_t *p11 = src.data() + (static_cast<size_t>(y1) * srcW + x1) * 4;

                    uint8_t *outPx = dst.data() + (static_cast<size_t>(y) * dstW + x) * 4;
                    for (int c = 0; c < 4; ++c)
                    {
                        float v0 = p00[c] * (1.0f - fx) + p10[c] * fx;
                        float v1 = p01[c] * (1.0f - fx) + p11[c] * fx;
                        float v  = v0 * (1.0f - fy) + v1 * fy;
                        int32_t iv = static_cast<int32_t>(v + 0.5f);
                        outPx[c] = static_cast<uint8_t>(std::max<int32_t>(0, std::min<int32_t>(255, iv)));
                    }
                }
            }
        }

        /**
         * \brief 对 B8G8R8A8 执行 Alpha 预乘：B = (B * A + 127) / 255, G = (G * A + 127) / 255, R = (R * A + 127) / 255
         */
        void premultiplyAlphaBGRA8(std::vector<uint8_t> &bgra, uint32_t width, uint32_t height)
        {
            const size_t totalPixels = static_cast<size_t>(width) * height;
            uint8_t *px = bgra.data();
            for (size_t i = 0; i < totalPixels; ++i)
            {
                uint32_t a = px[i * 4 + 3];
                px[i * 4 + 0] = static_cast<uint8_t>((static_cast<uint32_t>(px[i * 4 + 0]) * a + 127u) / 255u);
                px[i * 4 + 1] = static_cast<uint8_t>((static_cast<uint32_t>(px[i * 4 + 1]) * a + 127u) / 255u);
                px[i * 4 + 2] = static_cast<uint8_t>((static_cast<uint32_t>(px[i * 4 + 2]) * a + 127u) / 255u);
            }
        }

        /**
         * \brief 使用 2x2 Box Filter 生成下一级 Mipmap
         */
        void downsampleMipBoxBGRA8(
            const uint8_t *prevMip,
            uint32_t prevW,
            uint32_t prevH,
            uint8_t *nextMip,
            uint32_t nextW,
            uint32_t nextH)
        {
            for (uint32_t y = 0; y < nextH; ++y)
            {
                uint32_t sy0 = std::min(y * 2, prevH - 1);
                uint32_t sy1 = std::min(y * 2 + 1, prevH - 1);

                for (uint32_t x = 0; x < nextW; ++x)
                {
                    uint32_t sx0 = std::min(x * 2, prevW - 1);
                    uint32_t sx1 = std::min(x * 2 + 1, prevW - 1);

                    const uint8_t *p00 = prevMip + (static_cast<size_t>(sy0) * prevW + sx0) * 4;
                    const uint8_t *p10 = prevMip + (static_cast<size_t>(sy0) * prevW + sx1) * 4;
                    const uint8_t *p01 = prevMip + (static_cast<size_t>(sy1) * prevW + sx0) * 4;
                    const uint8_t *p11 = prevMip + (static_cast<size_t>(sy1) * prevW + sx1) * 4;

                    uint8_t *dst = nextMip + (static_cast<size_t>(y) * nextW + x) * 4;
                    for (int c = 0; c < 4; ++c)
                    {
                        uint32_t sum = static_cast<uint32_t>(p00[c])
                                     + static_cast<uint32_t>(p10[c])
                                     + static_cast<uint32_t>(p01[c])
                                     + static_cast<uint32_t>(p11[c]);
                        dst[c] = static_cast<uint8_t>((sum + 2u) >> 2);
                    }
                }
            }
        }
    }

    //--------------------------------------------------------------------------

    ImageImporterPtr ImageImporter::create()
    {
        return T3D_NEW ImageImporter();
    }

    //--------------------------------------------------------------------------

    TResult ImageImporter::importTexture(
        const String &srcPath,
        const String &texName,
        const TextureImportSettings &settings,
        const UUID &uuid,
        Texture2DPtr &outTexture,
        String *outError)
    {
        outTexture = nullptr;

        if (!Dir::exists(srcPath))
        {
            reportErr(outError, "Source image file does not exist: " + srcPath);
            return T3D_ERR_FILE_NOT_EXIST;
        }

        String srcDir, srcTitle, srcExt;
        Dir::parsePath(srcPath, srcDir, srcTitle, srcExt);
        if (srcDir.empty())
        {
            srcDir = ".";
        }
        String srcFileName = srcExt.empty() ? srcTitle : (srcTitle + "." + srcExt);

        ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(srcDir, ARCHIVE_TYPE_FS, Archive::AccessMode::kRead);
        if (archive == nullptr)
        {
            reportErr(outError, "Failed to open source directory archive: " + srcDir);
            return T3D_ERR_FAIL;
        }

        // 1. 通过 T3D_IMAGE_MGR (FreeImageCodec) 解码源图
        ImagePtr image = T3D_IMAGE_MGR.loadImage(archive, srcFileName);
        if (image == nullptr)
        {
            reportErr(outError, "Failed to decode source image: " + srcPath);
            return T3D_ERR_FAIL;
        }

        uint32_t width  = static_cast<uint32_t>(image->getWidth());
        uint32_t height = static_cast<uint32_t>(image->getHeight());

        // 2. 统一像素格式为 B8G8R8A8
        std::vector<uint8_t> level0BGRA;
        if (!convertToBGRA8(image, level0BGRA, outError))
        {
            T3D_IMAGE_MGR.unload(image);
            return T3D_ERR_FAIL;
        }
        T3D_IMAGE_MGR.unload(image);
        image = nullptr;

        // 3. FlipY 翻转
        if (settings.FlipY)
        {
            flipVerticalBGRA8(level0BGRA, width, height);
        }

        // 4. MaxSize 等比缩放（长边超过 MaxSize 时）
        if (settings.MaxSize > 0 && (width > settings.MaxSize || height > settings.MaxSize))
        {
            uint32_t targetW = width;
            uint32_t targetH = height;
            if (width >= height)
            {
                targetW = settings.MaxSize;
                targetH = std::max<uint32_t>(1u, static_cast<uint32_t>(
                    (static_cast<uint64_t>(height) * settings.MaxSize + width / 2) / width));
            }
            else
            {
                targetH = settings.MaxSize;
                targetW = std::max<uint32_t>(1u, static_cast<uint32_t>(
                    (static_cast<uint64_t>(width) * settings.MaxSize + height / 2) / height));
            }

            std::vector<uint8_t> scaled;
            resizeBilinearBGRA8(level0BGRA, width, height, scaled, targetW, targetH);
            level0BGRA.swap(scaled);
            width  = targetW;
            height = targetH;
        }

        // 5. Premultiply Alpha（必须在 Mipmap 生成之前执行，保证下采样在预乘空间中无黑边出血）
        if (settings.PremultipliedAlpha)
        {
            premultiplyAlphaBGRA8(level0BGRA, width, height);
        }

        // 6. 计算 Mipmap 层级与总字节数，并生成完整 Mip 链紧密拼接到 Buffer
        uint32_t mipLevels = 1;
        size_t totalByteSize = static_cast<size_t>(width) * height * 4;

        if (settings.GenerateMipmaps)
        {
            uint32_t mw = width;
            uint32_t mh = height;
            while (mw > 1 || mh > 1)
            {
                mw = std::max<uint32_t>(1u, mw >> 1);
                mh = std::max<uint32_t>(1u, mh >> 1);
                totalByteSize += static_cast<size_t>(mw) * mh * 4;
                ++mipLevels;
            }
        }

        Buffer buf;
        buf.DataSize = totalByteSize;
        buf.Data = T3D_POD_NEW_ARRAY(uint8_t, totalByteSize);
        uint8_t *bufPtr = buf.Data;
        const size_t level0Bytes = static_cast<size_t>(width) * height * 4;
        std::memcpy(bufPtr, level0BGRA.data(), level0Bytes);

        if (mipLevels > 1)
        {
            size_t prevOffset = 0;
            uint32_t prevW = width;
            uint32_t prevH = height;
            size_t nextOffset = level0Bytes;

            for (uint32_t level = 1; level < mipLevels; ++level)
            {
                uint32_t nextW = std::max<uint32_t>(1u, prevW >> 1);
                uint32_t nextH = std::max<uint32_t>(1u, prevH >> 1);
                size_t nextBytes = static_cast<size_t>(nextW) * nextH * 4;

                downsampleMipBoxBGRA8(
                    bufPtr + prevOffset, prevW, prevH,
                    bufPtr + nextOffset, nextW, nextH);

                prevOffset = nextOffset;
                nextOffset += nextBytes;
                prevW = nextW;
                prevH = nextH;
            }
        }

        // 7. 构建 Texture2D 并填充属性（Texture2D 接管 buf.Data 所有权）
        Texture2DPtr tex = T3D_TEXTURE_MGR.createTexture2D(
            texName,
            width,
            height,
            PixelFormat::E_PF_B8G8R8A8,
            buf,
            mipLevels,
            1,
            0,
            uuid);

        if (tex == nullptr)
        {
            buf.release();
            reportErr(outError, "T3D_TEXTURE_MGR.createTexture2D failed for: " + srcPath);
            return T3D_ERR_FAIL;
        }

        tex->setPremultipliedAlpha(settings.PremultipliedAlpha);
        tex->setSamplerDesc(settings.Sampler);

        outTexture = tex;
        return T3D_OK;
    }
}
