/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
 * For latest info, see https://github.com/answerear/D3D11
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


#ifndef __T3D_D3D11_HARDWARE_PIXEL_BUFFER_H__
#define __T3D_D3D11_HARDWARE_PIXEL_BUFFER_H__


#include "T3DD3D11Prerequisites.h"


namespace Tiny3D
{
    /**
     * @brief DirectX 11 渲染器相关的像素缓冲区类
     */
    class D3D11HardwarePixelBuffer : public HardwarePixelBuffer
    {
    public:
        /**
         * @brief 创建 Direct3D9 渲染器相关的像素缓冲区对象
         */
        static D3D11HardwarePixelBufferPtr create(size_t width, size_t height,
            PixelFormat format, const void *pixels, Usage usage, 
            bool useSystemMemory, bool useShadowBuffer);

        /**
         * @brief 析构函数
         */
        virtual ~D3D11HardwarePixelBuffer();

        /**
         * @brief 按照源区域从image读取数据到目标区域。 实现基类接口
         */
        virtual TResult readImage(const Image &image, Rect *srcRect = nullptr,
            Rect *dstRect = nullptr) override;

        /**
         * @brief 把指定源区域范围数据写到image的目标区域。 实现基类接口
         */
        virtual TResult writeImage(Image &image, Rect *dstRect = nullptr,
            Rect *srcRect = nullptr) override;

    protected:
        /**
        * @brief 构造函数
        */
        D3D11HardwarePixelBuffer(size_t width, size_t height,
            PixelFormat format, Usage usage, bool useSystemMemory,
            bool useShadowBuffer);

        /**
         * @brief 初始化对象
         */
        virtual TResult init(const void *pixels);

        /**
         * @brief 获取锁定硬件缓冲区不同渲染器实现接口。 实现基类接口
         */
        virtual void *lockImpl(const Rect &rect, LockOptions options,
            int32_t &lockedPitch) override;

        /**
         * @brief 解锁缓冲区的具体实现接口，实现基类接口
         * @see TResult HardwareVertexBuffer::unlockImpl()
         */
        virtual TResult unlockImpl() override;

    protected:
    };
}


#endif  /*__T3D_D3D11_HARDWARE_PIXEL_BUFFER_H__*/