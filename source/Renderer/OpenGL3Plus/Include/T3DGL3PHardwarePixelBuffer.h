/***************************************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
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
 **************************************************************************************************/

#ifndef __T3D_GL3P_HARDWARE_PIXEL_BUFFER_H__
#define __T3D_GL3P_HARDWARE_PIXEL_BUFFER_H__

#include "T3DGL3PPrerequisites.h"


namespace Tiny3D
{
    class GL3PHardwarePixelBuffer : public HardwarePixelBuffer
    {
    public:
        GL3PHardwarePixelBuffer(uint32_t width, uint32_t height,
            PixelFormat format, HardwareBuffer::Usage usage,
            bool useSystemMemory, bool useShadowBuffer);
        virtual ~GL3PHardwarePixelBuffer();

        bool createTexture();

        /**
        * @brief 获取锁定硬件缓冲区不同渲染器实现接口
        * @param [in] rect : 要获取数据的区域
        * @param [in] options : 获取数据选项
        * @param [out] lockedPitch : 返回锁定区域的pitch
        * @return 返回锁定的硬件数据地址
        * @see enum LockOptions
        */
        virtual void *lockImpl(const Rect &rect, LockOptions options, int32_t &lockedPitch) override;

        /**
        * @brief 解锁硬件缓冲区
        */
        virtual void unlockImpl() override;

        /**
        * @brief 按照源区域从image读取数据到目标区域
        * @param [in] image : 要读取的图像对象
        * @param [in] srcRect : 源数据区域，默认为nullptr的时候，表示整个源目标区域，会自动缩放匹配目标区域
        * @param [in] dstRect : 目标数据区域，默认为nullptr的时候，表示整个目标区域，会自动缩放匹配源区域
        * @return 调用成功返回true，否则返回false
        * @note dstRect 和 srcRect 均为nullptr的时候，会自动缩放匹配源和目标区域
        */
        virtual bool readImage(const Image &image, Rect *srcRect = nullptr, Rect *dstRect = nullptr) override;

        /**
        * @brief 把指定源区域范围数据写到image的目标区域
        * @param [in] image : 要写入的图像对象
        * @param [in] dstRect : 目标区域，默认为nullptr的时候，表示整个目标区域，会自动缩放匹配源区域
        * @param [in] srcRect : 源区域，默认为nullptr的时候，表示整个源目标区域，会自动缩放匹配目标区域
        * @return 调用成功返回true，否则返回false
        * @note dstRect 和 srcRect 均为nullptr的时候，会自动缩放匹配源和目标区域
        */
        virtual bool writeImage(Image &image, Rect *dstRect = nullptr, Rect *srcRect = nullptr) override;

    protected:
    };
}


#endif  /*__T3D_GL3P_HARDWARE_PIXEL_BUFFER_H__*/