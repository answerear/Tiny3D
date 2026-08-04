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


#ifndef __T3D_IMAGE_CODEC_BASE_H__
#define __T3D_IMAGE_CODEC_BASE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Kernel/T3DConstant.h"
#include "Resource/T3DImage.h"


namespace Tiny3D
{
    /**
     * \brief 图像编解码抽象基类，具体格式由派生类实现
     * \remarks 派生类须实现 isSupportedType、getFileType、encode、decode 及图像处理虚函数
     */
    class T3D_ENGINE_API ImageCodecBase : public Object
    {
    public:
        /**
         * \brief 支持的图像文件格式枚举，与 FreeImage 格式 ID 对齐
         */
        enum class FileType : uint32_t
        {
            UNKNOWN = 0xFFFFFFFF,   ///< 未知或未指定
            BMP = 0,                ///< Bitmap
            ICO = 1,                ///< Windows 图标
            JPEG = 2,               ///< JPEG
            JNG = 3,                ///< JPEG Network Graphics
            KOALA = 4,              ///< Koala 格式
            LBM = 5,                ///< Deluxe Paint LBM
            IFF = LBM,              ///< IFF（同 LBM）
            MNG = 6,                ///< Multiple-image Network Graphics
            PBM = 7,                ///< Portable Bitmap
            PBMRAW = 8,             ///< PBM 原始格式
            PCD = 9,                ///< Kodak Photo CD
            PCX = 10,               ///< PC Paintbrush
            PGM = 11,               ///< Portable Graymap
            PGMRAW = 12,            ///< PGM 原始格式
            PNG = 13,               ///< PNG
            PPM = 14,               ///< Portable Pixmap
            PPMRAW = 15,            ///< PPM 原始格式
            RAS = 16,               ///< Sun Raster
            TARGA = 17,             ///< Targa
            TIFF = 18,              ///< TIFF
            WBMP = 19,              ///< Wireless Bitmap
            PSD = 20,               ///< Photoshop
            CUT = 21,               ///< Dr. Halo CUT
            XBM = 22,               ///< X Bitmap
            XPM = 23,               ///< X Pixmap
            DDS = 24,               ///< DirectDraw Surface
            GIF = 25,               ///< GIF
            HDR = 26,               ///< Radiance HDR
            FAXG3 = 27,             ///< CCITT Group 3 Fax
            SGI = 28,               ///< SGI Image
            EXR = 29,               ///< OpenEXR
            J2K = 30,               ///< JPEG-2000
            JP2 = 31,               ///< JPEG-2000 JP2
            PFM = 32,               ///< Portable Float Map
            PICT = 33,              ///< Mac PICT
            RAW = 34,               ///< 相机 RAW
            WEBP = 35,              ///< WebP
            JXR = 36,               ///< JPEG XR
            IMG = 37,               ///< GEM Image
            PVRTC = 38,             ///< PowerVR 压缩纹理
            ASTC = 39,              ///< ASTC 压缩纹理
            ETC1 = 40,              ///< ETC1 压缩纹理
            ETC2 = 41,              ///< ETC2 压缩纹理
            MAX_TYPE_SUPPORTED,     ///< 枚举上界（非有效格式）
        };

        /// 虚析构
        virtual ~ImageCodecBase();

        /**
         * \brief 检测缓冲区是否为本编解码器支持的格式
         * \param [in] data : 图像数据缓冲区
         * \param [in] size : 数据字节数
         * \param [in,out] type : 输入可为 UNKNOWN 以自动识别；识别成功时写回具体 FileType
         * \return 支持返回 true，否则 false
         */
        virtual bool isSupportedType(uint8_t *data, size_t size, FileType &type) const = 0;

        /**
         * \brief 获取本编解码器对应的文件类型
         * \return FileType 枚举值
         */
        virtual FileType getFileType() const = 0;

        /**
         * \brief 将 Image 编码到内存缓冲区
         * \param [out] data : 编码输出缓冲区，由派生类分配
         * \param [out] size : 编码输出字节数
         * \param [in] image : 源图像
         * \param [in] type : 目标文件格式
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult encode(uint8_t *&data, size_t &size, const Image &image, FileType type) = 0;

        /**
         * \brief 将内存缓冲区解码到 Image
         * \param [in] data : 待解码数据
         * \param [in] size : 数据字节数
         * \param [out] image : 解码结果写入此对象
         * \param [in] type : 文件格式；UNKNOWN 时由派生类自动识别
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult decode(uint8_t *data, size_t size, Image &image, FileType type) = 0;

        /**
         * \brief 垂直翻转图像
         * \param [in,out] image : 待翻转的图像，结果写回同一对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult flip(Image &image) = 0;

        /**
         * \brief 水平镜像图像
         * \param [in,out] image : 待镜像的图像，结果写回同一对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult mirror(Image &image) = 0;

        /**
         * \brief 用指定颜色填充图像
         * \param [in,out] image : 待填充的图像
         * \param [in] color : 填充颜色
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult fill(Image &image, const Color4 &color) = 0;

        /**
         * \brief 将源图像指定区域复制到目标图像指定区域
         * \param [in] srcImage : 源图像
         * \param [in] srcRect : 源区域，可为 nullptr 表示整图
         * \param [in,out] dstImage : 目标图像
         * \param [in] dstRect : 目标区域，可为 nullptr 表示整图
         * \param [in] filter : 缩放时使用的滤波算法
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult copy(const Image &srcImage, const Rect *srcRect, Image &dstImage, const Rect *dstRect, uint32_t filter) = 0;

        /**
         * \brief 就地转换 Image 像素格式
         * \param [in,out] image : 待转换图像
         * \param [in] format : 目标 PixelFormat
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult convert(Image &image, PixelFormat format) = 0;

        /**
         * \brief 将源图像转换像素格式并写入目标图像
         * \param [in] srcImage : 源图像
         * \param [out] dstImage : 转换结果
         * \param [in] format : 目标 PixelFormat
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult convert(const Image &srcImage, Image &dstImage, PixelFormat format) = 0;

    protected:
        /**
         * \brief 设置 Image 原始像素数据指针与大小
         * \param [in,out] image : 目标 Image
         * \param [in] data : 像素数据缓冲区
         * \param [in] size : 数据字节数
         */
        void setImageData(Image &image, uint8_t *data, size_t size);

        /**
         * \brief 设置 Image 宽高与 pitch
         * \param [in,out] image : 目标 Image
         * \param [in] width : 宽度（像素）
         * \param [in] height : 高度（像素）
         * \param [in] pitch : 行跨度（字节）
         */
        void setImageDimension(Image &image, int32_t width, int32_t height, int32_t pitch);

        /**
         * \brief 设置 Image 格式元信息
         * \param [in,out] image : 目标 Image
         * \param [in] fileFormat : 源文件格式（Image::FileFormat）
         * \param [in] bpp : 色深（bits per pixel）
         * \param [in] hasAlpha : 是否含 alpha 通道
         * \param [in] isPreMulti : 是否预乘 alpha
         * \param [in] pixelFormat : 像素格式
         */
        void setImageInfo(Image &image, Image::FileFormat fileFormat, int32_t bpp, bool hasAlpha, bool isPreMulti, PixelFormat pixelFormat);
    };
}


#include "T3DImageCodecBase.inl"


#endif  /*__T3D_IMAGE_CODEC_BASE_H__*/
