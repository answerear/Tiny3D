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


#ifndef __T3D_IMAGE_CODEC_H__
#define __T3D_IMAGE_CODEC_H__


#include "ImageCodec/T3DImageCodecBase.h"


namespace Tiny3D
{
    /**
     * \brief 图像编解码单例门面，按 FileType 路由到已注册的 ImageCodecBase 实现
     */
    class T3D_ENGINE_API ImageCodec
        : public Singleton<ImageCodec>
        , public Object
    {
    public:
        /**
         * \brief 创建 ImageCodec 实例
         * \return ImageCodec 智能指针
         */
        static ImageCodecPtr create();

        /// 虚析构
        virtual ~ImageCodec();

        /**
         * \brief 注册指定格式的编解码器
         * \param [in] type : 文件格式键
         * \param [in] codec : 编解码器实例指针
         * \return 调用成功返回 T3D_OK
         */
        TResult addImageCodec(ImageCodecBase::FileType type, ImageCodecBase *codec);

        /**
         * \brief 从映射表移除指定格式的编解码器
         * \param [in] type : 文件格式键
         * \return 调用成功返回 T3D_OK
         */
        TResult removeImageCodec(ImageCodecBase::FileType type);

        /**
         * \brief 编码 Image 并写入文件
         * \param [in] name : 输出文件路径
         * \param [in] image : 源图像
         * \param [in] type : 目标文件格式，默认 PNG
         * \return 调用成功返回 T3D_OK；文件打开失败返回 T3D_ERR_FILE_NOT_EXIST
         */
        TResult encode(const String &name, const Image &image, ImageCodecBase::FileType type = ImageCodecBase::FileType::PNG);

        /**
         * \brief 编码 Image 并写入数据流
         * \param [in,out] stream : 输出数据流
         * \param [in] image : 源图像
         * \param [in] type : 目标文件格式，默认 PNG
         * \return 调用成功返回 T3D_OK；写入字节数不足时仅记录错误日志
         */
        TResult encode(DataStream &stream, const Image &image, ImageCodecBase::FileType type = ImageCodecBase::FileType::PNG);

        /**
         * \brief 编码 Image 到内存缓冲区
         * \param [out] data : 编码输出缓冲区，由底层编解码器分配
         * \param [out] size : 输出字节数
         * \param [in] image : 源图像
         * \param [in] type : 目标文件格式，默认 PNG
         * \return 调用成功返回 T3D_OK；找不到编解码器时失败
         */
        TResult encode(uint8_t *&data, size_t &size, const Image &image, ImageCodecBase::FileType type = ImageCodecBase::FileType::PNG);

        /**
         * \brief 从文件路径解码 Image（当前实现为空，直接返回 T3D_OK）
         * \param [in] name : 图像文件路径
         * \param [out] image : 解码结果
         * \param [in] type : 文件格式，默认 UNKNOWN 自动识别
         * \return 当前实现始终返回 T3D_OK，不填充 image
         */
        TResult decode(const String &name, Image &image, ImageCodecBase::FileType type = ImageCodecBase::FileType::UNKNOWN);

        /**
         * \brief 从数据流解码 Image
         * \param [in,out] stream : 输入数据流
         * \param [out] image : 解码结果
         * \param [in] type : 文件格式，默认 UNKNOWN 时按内容自动识别
         * \return 调用成功返回 T3D_OK；读取失败返回 T3D_ERR_FILE_DATA_MISSING
         */
        TResult decode(DataStream &stream, Image &image, ImageCodecBase::FileType type = ImageCodecBase::FileType::UNKNOWN);

        /**
         * \brief 从内存缓冲区解码 Image
         * \param [in] data : 待解码数据
         * \param [in] size : 数据字节数
         * \param [out] image : 解码结果
         * \param [in] type : 文件格式，默认 UNKNOWN 时按内容自动识别
         * \return 调用成功返回 T3D_OK；找不到编解码器返回 T3D_ERR_IMG_NOT_FOUND
         */
        TResult decode(uint8_t *data, size_t size, Image &image, ImageCodecBase::FileType type = ImageCodecBase::FileType::UNKNOWN);

        /**
         * \brief 垂直翻转图像，按 image.getFileFormat() 选择编解码器
         * \param [in,out] image : 待翻转图像
         * \return 调用成功返回 T3D_OK；找不到编解码器返回 T3D_ERR_IMG_NOT_FOUND
         */
        TResult flip(Image &image);

        /**
         * \brief 水平镜像图像，按 image.getFileFormat() 选择编解码器
         * \param [in,out] image : 待镜像图像
         * \return 调用成功返回 T3D_OK；找不到编解码器返回 T3D_ERR_IMG_NOT_FOUND
         */
        TResult mirror(Image &image);

        /**
         * \brief 用颜色填充图像，按 image.getFileFormat() 选择编解码器
         * \param [in,out] image : 待填充图像
         * \param [in] color : 填充颜色
         * \return 调用成功返回 T3D_OK；找不到编解码器返回 T3D_ERR_IMG_NOT_FOUND
         */
        TResult fill(Image &image, const Color4 &color);

        /**
         * \brief 区域复制，按 srcImage.getFileFormat() 选择编解码器
         * \param [in] srcImage : 源图像
         * \param [in] srcRect : 源区域，可为 nullptr
         * \param [in,out] dstImage : 目标图像
         * \param [in] dstRect : 目标区域，可为 nullptr
         * \param [in] filter : 缩放滤波算法
         * \return 调用成功返回 T3D_OK；找不到编解码器返回 T3D_ERR_IMG_NOT_FOUND
         */
        TResult copy(const Image &srcImage, const Rect *srcRect, Image &dstImage, const Rect *dstRect, uint32_t filter);

        /**
         * \brief 就地转换像素格式，按 image.getFileFormat() 选择编解码器
         * \param [in,out] image : 待转换图像
         * \param [in] format : 目标 PixelFormat
         * \return 调用成功返回 T3D_OK；找不到编解码器返回 T3D_ERR_IMG_NOT_FOUND
         */
        TResult convert(Image &image, PixelFormat format);

        /**
         * \brief 转换像素格式并输出到新 Image，按 srcImage.getFileFormat() 选择编解码器
         * \param [in] srcImage : 源图像
         * \param [out] dstImage : 转换结果
         * \param [in] format : 目标 PixelFormat
         * \return 调用成功返回 T3D_OK；找不到编解码器返回 T3D_ERR_IMG_NOT_FOUND
         */
        TResult convert(const Image &srcImage, Image &dstImage, PixelFormat format);

    protected:
        /// 受保护构造，初始化空编解码器映射
        ImageCodec();

        /**
         * \brief 按缓冲区内容或指定类型查找编解码器
         * \param [in] data : 图像数据缓冲区
         * \param [in] size : 数据字节数
         * \param [in,out] type : 非 UNKNOWN 时按 type 查找；UNKNOWN 时遍历并调用 isSupportedType 自动识别
         * \return 找到返回对应 ImageCodecBasePtr，否则 nullptr
         */
        ImageCodecBasePtr getImageCodec(uint8_t *data, size_t size, ImageCodecBase::FileType &type) const;

        /**
         * \brief 按 FileType 键查找编解码器
         * \param [in] type : 文件格式
         * \return 找到返回 ImageCodecBasePtr，否则 nullptr
         */
        ImageCodecBasePtr getImageCodec(ImageCodecBase::FileType type) const;

        using ImageCodecMap = TMap<ImageCodecBase::FileType, ImageCodecBasePtr>;
        using ImageCodecMapItr = ImageCodecMap::iterator;
        using ImageCodecMapConstItr = ImageCodecMap::const_iterator;
        using ImageCodecMapValue = ImageCodecMap::value_type;

        /// FileType → ImageCodecBase 注册表
        ImageCodecMap   mCodecMap;
    };

    #define T3D_IMAGE_CODEC     ImageCodec::getInstance()

    T3D_EXTERN_SINGLETON(ImageCodec)
}


#endif  /*__T3D_IMAGE_CODEC_H__*/
