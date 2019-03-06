/*******************************************************************************
* This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
* Copyright (C) 2015-2019  Answer Wong
* For latest info, see https://github.com/asnwerear/Tiny3D
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/


#ifndef __T3D_IMAGE_CODEC_BASE_H__
#define __T3D_IMAGE_CODEC_BASE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Kernel/T3DPixelFormat.h"
#include "ImageCodec/T3DImage.h"


namespace Tiny3D
{
    /**
    * @brief 图像编解码基类
    * @remarks 具体对应的图像编解码类需要继承本类并实现具体的逻辑
    */
    class T3D_ENGINE_API ImageCodecBase : public Object
    {
    public:
        /**
        * @brief 支持的图像类型
        */
        enum FileType
        {
            E_FT_UNKNOWN = -1,      /**< 未知图像类型 */
            E_FT_BMP = 0,           /**< Bitmap */
            E_FT_ICO = 1,           /**< Windows 图标  */
            E_FT_JPEG = 2,          /**< JPEG */
            E_FT_JNG = 3,           /**< JNG */
            E_FT_KOALA = 4,
            E_FT_LBM = 5,
            E_FT_IFF = E_FT_LBM,
            E_FT_MNG = 6,
            E_FT_PBM = 7,
            E_FT_PBMRAW = 8,
            E_FT_PCD = 9,
            E_FT_PCX = 10,
            E_FT_PGM = 11,
            E_FT_PGMRAW = 12,
            E_FT_PNG = 13,
            E_FT_PPM = 14,
            E_FT_PPMRAW = 15,
            E_FT_RAS = 16,
            E_FT_TARGA = 17,
            E_FT_TIFF = 18,
            E_FT_WBMP = 19,
            E_FT_PSD = 20,
            E_FT_CUT = 21,
            E_FT_XBM = 22,
            E_FT_XPM = 23,
            E_FT_DDS = 24,
            E_FT_GIF = 25,
            E_FT_HDR = 26,
            E_FT_FAXG3 = 27,
            E_FT_SGI = 28,
            E_FT_EXR = 29,
            E_FT_J2K = 30,
            E_FT_JP2 = 31,
            E_FT_PFM = 32,
            E_FT_PICT = 33,
            E_FT_RAW = 34,
            E_FT_WEBP = 35,
            E_FT_JXR = 36,
            E_FT_IMG = 37,
            E_FT_PVRTC = 38,
            E_FT_ASTC = 39,
            E_FT_ETC1 = 40,
            E_FT_ETC2 = 41,
            MAX_TYPE_SUPPORTED,
        };

        /**
         * @brief 析构函数
         */
        virtual ~ImageCodecBase();

        /**
         * @brief 是否支持的类型
         * @param [in] data : 图像数据
         * @param [in] size : 图像数据大小
         * @param [in] type : 图像文件类型，默认可以自动识别
         * @return 支持的返回true，否则返回false
         * @remarks 具体图像类型编解码器实现本接口
         */
        virtual bool isSupportedType(uint8_t *data, size_t size,
            FileType &type) const = 0;

        /**
         * @brief 获取文件类型
         * @return 返回文件类型
         * @remarks 具体图像类型编解码器实现本接口
         */
        virtual FileType getFileType() const = 0;

        /**
         * @brief 把图像对象编码到数据缓冲中
         * @param [in][out] data : 编码后的数据
         * @param [in][out] size : 编码后的数据大小
         * @param [in] image : 图像对象，数据源
         * @param [in] type : 文件类型
         * @return 调用成功返回 T3D_OK
         * @remarks 具体图像类型编解码器实现本接口
         */
        virtual TResult encode(uint8_t *&data, size_t &size, const Image &image,
            FileType type) = 0;

        /**
         * @brief 把缓冲数据解码到图像对象中
         * @param [in] data : 要解码的数据
         * @param [in] size : 要解码的数据大小
         * @param [in] image : 图像对象，解码后的数据保存在此对象中
         * @param [in] type : 图像类型
         * @return 调用成功返回 T3D_OK
         * @remarks 具体图像类型编解码器实现本接口
         */
        virtual TResult decode(uint8_t *data, size_t size, Image &image,
            FileType type) = 0;

        /**
         * @brief 颠倒图像
         * @param [in][out] image : 需要颠倒的图像对象
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult flip(Image &image) = 0;

        /**
         * @brief 镜像图像
         * @param [in][out] image : 需要镜像的图像对象
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult mirror(Image &image) = 0;

        /**
         * @brief 用指定颜色填充图像
         * @param [in][out] image : 需要填充的图像对象
         * @param [in] color : 需要填充的颜色
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult fill(Image &image, const Color4 &color) = 0;

        /**
         * @brief 复制源图像指定区域数据到目标图像指定区域
         * @param [in] srcImage : 源图像对象
         * @param [in] srcRect : 源图像区域
         * @param [in][out] dstImage : 目标图像对象
         * @param [in] dstRect : 目标图像区域
         * @param [in] filter : 缩放时候使用的算法
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult copy(const Image &srcImage, const Rect *srcRect,
            Image &dstImage, const Rect *dstRect, uint32_t filter) = 0;

        /**
         * @brief 转换到目标像素格式
         * @param [in][out] image : 需要转换像素格式图像对象
         * @param [in] format : 目标图像像素格式
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult convert(Image &image, PixelFormat format) = 0;

        /**
         * @brief 把源图像转换成目标像素格式并生成一个新的图像对象
         * @param [in] srcImage : 源图像对象
         * @param [in][out] dstImage : 目标图像对象
         * @param [in] format : 目标像素格式
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult convert(const Image &srcImage, Image &dstImage,
            PixelFormat format) = 0;

    protected:
        /**
         * @brief 设置图像数据
         * @param [in] image : 需要设置数据的图像对象
         * @param [in] data : 解码后的ARGB数据
         * @param [in] size : 解码后的ARGB数据大小
         */
        void setImageData(Image &image, uint8_t *data, size_t size);

        /**
         * @brief 设置图像尺寸
         * @param [in] image : 需要设置尺寸的图像对象
         * @param [in] width : 图像宽度
         * @param [in] height : 图像高度
         * @param [in] pitch : 图像跨度
         */
        void setImageDimension(Image &image, int32_t width, int32_t height,
            int32_t pitch);

        /**
         * @brief 设置图像信息
         * @param [in] image : 需要设置信息的图像对象
         * @param [in] sourceType : 数据来源的文件类型
         * @param [in] bpp : 图像色深
         * @param [in] hasAlpha : 是否有透明通道
         * @param [in] isPreMulti : 是否预乘
         * @param [in] format : 像素格式
         */
        void setImageInfo(Image &image, uint32_t sourceType, int32_t bpp,
            bool hasAlpha, bool isPreMulti, PixelFormat format);
    };
}


#include "T3DImageCodecBase.inl"


#endif  /*__T3D_IMAGE_CODEC_BASE_H__*/
