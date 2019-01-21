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


#ifndef __T3D_IMAGE_CODEC_FREEIMAGE_H__
#define __T3D_IMAGE_CODEC_FREEIMAGE_H__


#include "T3DFreeImageCodecPrerequisites.h"


namespace Tiny3D
{
    /**
     * @brief 
     */
    class FreeImageCodec : public ImageCodecBase
    {
    public:
        /**
         * @brief 是否支持的类型
         * @param [in] data : 图像数据
         * @param [in] size : 图像数据大小
         * @param [in] type : 图像文件类型，默认可以自动识别
         * @return 支持的返回true，否则返回false
         * @remarks 具体图像类型编解码器实现本接口
         */
        virtual bool isSupportedType(uint8_t *data, size_t size,
            FileType &type) const override;

        /**
         * @brief 获取文件类型
         * @return 返回文件类型
         * @remarks 具体图像类型编解码器实现本接口
         */
        virtual FileType getFileType() const override;

        /**
         * @brief 把图像对象编码到数据缓冲中
         * @param [in][out] data : 编码后的数据
         * @param [in][out] size : 编码后的数据大小
         * @param [in] image : 图像对象，数据源
         * @param [in] type : 文件类型
         * @return 调用成功返回 T3D_ERR_OK
         * @remarks 具体图像类型编解码器实现本接口
         */
        virtual TResult encode(uint8_t *&data, size_t &size, const Image &image,
            FileType type) override;

        /**
         * @brief 把缓冲数据解码到图像对象中
         * @param [in] data : 要解码的数据
         * @param [in] size : 要解码的数据大小
         * @param [in] image : 图像对象，解码后的数据保存在此对象中
         * @param [in] type : 图像类型
         * @return 调用成功返回 T3D_ERR_OK
         * @remarks 具体图像类型编解码器实现本接口
         */
        virtual TResult decode(uint8_t *data, size_t size, Image &image,
            FileType type) override;
    };
}


#endif  /*__T3D_IMAGE_CODEC_FREEIMAGE_H__*/
