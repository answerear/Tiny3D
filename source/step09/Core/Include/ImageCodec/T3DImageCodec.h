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


#ifndef __T3D_IMAGE_CODEC_H__
#define __T3D_IMAGE_CODEC_H__


#include "ImageCodec/T3DImageCodecBase.h"


namespace Tiny3D
{
    /**
     * @brief 
     */
    class ImageCodec 
        : public Object
        , public Singleton<ImageCodec>
    {
    public:
        /**
         * @brief 创建对象
         */
        static ImageCodecPtr create();

        /**
         * @brief 析构函数
         */
        virtual ~ImageCodec();

        /**
         * @brief 添加图像编解码器对象
         * @param [in] codec : 编解码器对象
         * @return 调用成功返回 T3D_OK
         */
        TResult addImageCodec(ImageCodecBasePtr codec);

        /**
         * @brief 移除图像编解码器对象
         * @param [in] codec : 编解码器对象
         * @return 调用成功返回 T3D_OK
         */
        TResult removeImageCodec(ImageCodecBasePtr codec);

        /**
         * @brief 编码到指定文件名的文件
         * @param [in] name : 文件名
         * @param [in] image : 数据源的图像对象
         * @param [in] type : 图像文件格式类型，默认PNG格式
         * @return 调用成功返回 T3D_OK
         */
        TResult encode(const String &name, const Image &image, 
            ImageCodecBase::FileType type = ImageCodecBase::E_FT_PNG);

        /**
         * @brief 编码到数据流对象
         * @param [in][out] stream : 数据流对象
         * @param [in] image : 数据源的图像对象
         * @param [in] type : 图像文件格式类型，默认PNG格式
         * @return 调用成功返回 T3D_OK
         */
        TResult encode(DataStream &stream, const Image &image, 
            ImageCodecBase::FileType type = ImageCodecBase::E_FT_PNG);

        /**
         * @brief 编码到数据缓冲
         * @param [in][out] data : 编码后返回的数据缓冲区
         * @param [in][out] size : 编码后返回的数据缓冲区大小
         * @param [in] type : 图像文件格式类型，默认PNG格式
         * @return 调用成功返回 T3D_OK
         */
        TResult encode(uint8_t *&data, size_t &size, const Image &image, 
            ImageCodecBase::FileType type = ImageCodecBase::E_FT_PNG);

        /**
         * @brief 把指定文件读取并解码到图像对象中
         * @param [in] name : 图像文件路径
         * @param [in][out] image : 图像对象
         * @param [in] type : 图像文件格式类型，默认是根据文件内容自动判断
         * @return 调用成功返回 T3D_OK
         */
        TResult decode(const String &name, Image &image, 
            ImageCodecBase::FileType type = ImageCodecBase::E_FT_UNKNOWN);

        /**
         * @brief 把数据流对象中的数据解码到图像对象中
         * @param [in] stream : 数据流对象
         * @param [in][out] image : 图像对象
         * @param [in] type : 图像文件格式类型，默认是根据数据流内容自动判断
         * @return 调用成功返回 T3D_OK
         */
        TResult decode(DataStream &stream, Image &image, 
            ImageCodecBase::FileType type = ImageCodecBase::E_FT_UNKNOWN);

        /**
         * @brief 把数据缓冲中的数据解码到图像对象中
         * @param [in] data : 要解码的数据缓冲区
         * @param [in] size : 要解码的数据缓冲区大小
         * @param [in] type : 图像文件格式类型，默认是根据数据流内容自动判断
         * @return 调用成功返回 T3D_OK
         */
        TResult decode(uint8_t *data, size_t size, Image &image, 
            ImageCodecBase::FileType type = ImageCodecBase::E_FT_UNKNOWN);

    protected:
        /**
         * @brief 构造函数
         */
        ImageCodec();

        /**
         * @brief 根据缓冲区内容获取图像编解码器对象
         * @param [in] data : 数据缓冲区
         * @param [in] size : 数据缓冲区大小
         * @param [in] type : 数据缓冲区对应的文件类型
         * @return 调用成功返回对应的图像编解码器对象，否则返回 nullptr
         */
        ImageCodecBasePtr getImageCodec(uint8_t *data, size_t size, 
            ImageCodecBase::FileType &type) const;

        /**
         * @brief 根据类型获取图像编解码器对象
         * @param [in] type : 图像数据格式类型
         * @return 调用成功返回对应的图像编解码器对象，否则返回 nullptr
         */
        ImageCodecBasePtr getImageCodec(ImageCodecBase::FileType type) const;

        typedef TMap<ImageCodecBase::FileType, ImageCodecBasePtr> ImageCodecMap;
        typedef ImageCodecMap::iterator             ImageCodecMapItr;
        typedef ImageCodecMap::const_iterator       ImageCodecMapConstItr;
        typedef ImageCodecMap::value_type           ImageCodecMapValue;

        ImageCodecMap   mCodecMap;
    };
}


#endif  /*__T3D_IMAGE_CODEC_H__*/
