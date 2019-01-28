/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Agent)
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


#ifndef __T3D_IMAGE_H__
#define __T3D_IMAGE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Kernel/T3DPixelFormat.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Image : public Object
    {
        friend class ImageCodecBase;

    public:
        static const char * const FILETYPE_PNG;
        static const char * const FILETYPE_JPG;
        static const char * const FILETYPE_BMP;
        static const char * const FILETYPE_TGA;
        static const char * const FILETYPE_DDS;
        static const char * const FILETYPE_PVRTC;
        static const char * const FILETYPE_ASTC;
        static const char * const FILETYPE_ETC1;
        static const char * const FILETYPE_ETC2;

        enum Filter
        {
            /**< Box, pulse, Fourier window, 1st order (constant) b-spline */
            E_FILTER_BOX = 0,
            /**< Mitchell & Netravali's two-param cubic filter */
            E_FILTER_BICUBIC = 1,
            /**< Bilinear filter */
            E_FILTER_BILINEAR = 2,
            /**< 4th order (cubic) b-spline */
            E_FILTER_BSPLINE = 3,
            /**< Catmull-Rom spline, Overhauser spline */
            E_FILTER_CATMULLROM = 4,
            /**< Lanczos3 filter */
            E_FILTER_LANCZOS3 = 5
        };

        /**
         * @brief 默认构造函数
         */
        Image();

        /**
         * @brief 构造一个指定宽高图像对象
         */
        Image(int32_t width, int32_t height, int32_t bpp, PixelFormat format);

        /**
         * @brief 拷贝构造函数
         */
        Image(const Image &other);

        /**
         * @brief 析构函数
         */
        virtual ~Image();

        /**
         * @brief 从别的图像对象复制过来
         */
        Image &operator =(const Image &other);

        /**
         * @brief 比较图像是否相同，主要使用图像MD5值来比对
         */
        bool operator ==(const Image &other) const;

        /**
         * @brief 比较图像是否不同，主要使用图像MD5值来比对
         */
        bool operator !=(const Image &other) const;

        /**
         * @brief 从文件加载图像数据
         * @param [in] path : 文件全路径
         * @return 调用成功返回 T3D_OK
         * @note 本接口读取文件后会自动识别文件格式来解码
         * @see TResult load(DataStream &stream)
         */
        TResult load(const String &path);

        /**
         * @brief 从内存加载图像数据
         * @param [in] stream : 数据输入流
         * @return 调用成功返回 T3D_OK
         * @note 本接口自动识别数据流的格式，并且解析数据流
         * @see TResult load(const String &path);
         */
        TResult load(DataStream &stream);

        /**
         * @brief 从内存Raw数据加载图像
         * @param [in] data : 图像原始数据
         * @param [in] width : 图像宽度
         * @param [in] height : 图像高度
         * @param [in] bpp : 图像色深
         * @param [in] pitch : 图像跨度
         * @param [in] format : 图像像素格式
         * @param [in] copySource : 是否深拷贝一份数据到内部维护
         * @return 调用成功返回 T3D_OK
         */
        TResult load(uint8_t *data, int32_t width, int32_t height, int32_t bpp, 
            int32_t pitch, PixelFormat format, bool copySource = false);

        /**
         * @brief 把图像保存到文件中
         * @param [in] path : 文件全路径
         * @param [in] fileType : 要保存的文件类型，默认是PNG文件
         * @return 调用成功返回 T3D_OK
         */
        TResult save(const String &path,
            const String &fileType = FILETYPE_PNG) const;

        /**
         * @brief 把图像保存到文件中
         * @param [in] stream : 输出的数据流对象
         * @param [in] fileType : 要保存的文件类型，默认是PNG文件
         * @return 调用成功返回 T3D_OK
         */
        TResult save(DataStream &stream,
            const String &fileType = FILETYPE_PNG) const;

        /**
         * @brief 销毁图像，使其成为空图像对象
         * @return 销毁成功返回true，否则返回false
         */
        TResult destroy();

        /**
         * @brief 上下翻转图像
         * @return 调用成功返回true，否则返回false
         */
        TResult flip();

        /**
         * @brief 左右镜像图像
         * @return 调用成功返回true，否则返回false
         */
        TResult mirror();

        /**
         * @brief 给图像填充指定颜色
         */
        TResult fill(const Color4 &rkColor);

        /**
         * @brief 从别的图像对象复制数据过来
         * @param [in] image : 源图像
         * @param [in] srcRect : 要复制的源区域，默认nullptr为整个区域
         * @param [in] dstRect : 要复制过去的目标区域，默认nullptr为整个区域
         * @return 调用成功返回 T3D_OK
         * @note 当源和目标区域不相同大小时，自动缩放来复制，
         *          源和目标图像必须要像素格式一致，否则调用失败。
         * @note 当自动缩放时，会导致性能下降
         */
        TResult copy(const Image &image, Rect *srcRect = nullptr,
            Rect *dstRect = nullptr, Filter filter = E_FILTER_BILINEAR);

        /**
         * @brief 转成目标像素格式的图像
         * @param [in] format : 目标像素格式
         * @return 调用成功返回true，否则返回false
         */
        TResult convert(PixelFormat format);

        /**
         * @brief 转成目标像素格式的图像
         * @param [in] format : 目标像素格式
         * @return 调用成功返回true，否则返回false
         */
        TResult convert(Image &image, PixelFormat format) const;

        /**
         * @brief 比较图像
         * @param [in] other : 另外一个图像对象
         * @param [in] compareAlpha : 如果存在alpha通道，是否比较alpha通道，
         *                           如果不存在alpha通道，本参数无效
         * @return 比较出相同返回true，不同返回false
         * @note 主要是比较图像MD5值
         */
        bool compare(const Image &other, bool compareAlpha = true) const;

        /**
         * @brief 返回图像数据
         */
        uint8_t *getData();

        /**
         * @brief 返回图像数据
         */
        const uint8_t * const getData() const;

        /**
         * @brief 返回图像数据大小
         */
        size_t getSize() const;

        /**
         * @brief 返回图像宽度
         */
        int32_t getWidth() const;

        /**
         * @brief 返回图像高度
         */
        int32_t getHeight() const;

        /**
         * @brief 返回图像跨度
         */
        int32_t getPitch() const;

        /**
         * @brief 返回图像色深
         */
        int32_t getBPP() const;

        /**
         * @brief 返回图像像素格式
         */
        PixelFormat getFormat() const;

        /**
         * @brief 返回图像是否带alpha通道
         */
        bool hasAlpha() const;

        /**
         * @brief 返回图像是否做了alpha预乘
         */
        bool isPremultipliedAlpha() const;

        /**
         * @brief 返回是否空图像
         */
        bool isEmpty() const;

        /**
         * @brief 获取三个颜色通道和一个透明通道的颜色mask
         */
        void getColorMask(uint32_t &redMask, uint32_t &greenMask, 
            uint32_t &blueMask, uint32_t &alphaMask) const;

        /**
         * @brief 根据色深计算图像行跨度
         */
        static int32_t calcPitch(int32_t width, int32_t bpp);

        /**
         * @brief 根据图像像素格式获取色深
         */
        static int32_t getBPP(PixelFormat format);

    protected:
        /**
         * @brief 根据文件扩展类型获取文件类型枚举
         */
        uint32_t getFileType(const String &fileExt) const;

        /**
         * @brief 计算图像行跨度
         */
        int32_t calcPitch() const;

        /**
         * @brief 获取当前像素格式下一个像素占用的字节数
         */
        int32_t getBytesPerPixel() const;

    private:
        int32_t     mWidth;             /**< 图像宽度 */
        int32_t     mHeight;            /**< 图像高度 */
        int32_t     mBPP;               /**< 图像色深 */
        int32_t     mPitch;             /**< 图像行跨度 */

        PixelFormat mFormat;            /**< 图像像素格式 */

        bool        mHasAlpha;          /**< 是否带alpha通道 */
        bool        mIsPreMulti;        /**< 是否预乘alpha */
        bool        mIsDataExternal;    /**< 数据是否外部分配的 */
        bool        mIsEmpty;           /**< 空图像（即没有任何数据的图像） */

        size_t      mDataSize;          /**< 图像数据大小 */
        uint8_t     *mData;             /**< 图像数据 */
    };
}


#include "T3DImage.inl"


#endif  /*__T3D_IMAGE_H__*/
