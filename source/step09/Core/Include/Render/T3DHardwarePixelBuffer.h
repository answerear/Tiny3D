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


#ifndef __T3D_HARDWARE_PIXEL_BUFFER_H__
#define __T3D_HARDWARE_PIXEL_BUFFER_H__


#include "Render/T3DHardwareBuffer.h"
#include "Kernel/T3DPixelFormat.h"


namespace Tiny3D
{
    /**
     * @brief 像素缓冲区
     */
    class T3D_ENGINE_API HardwarePixelBuffer : public HardwareBuffer
    {
    public:
        /**
         * @brief 析构函数
         */
        virtual ~HardwarePixelBuffer();

        /**
         * @brief 获取硬件缓冲区数据
         * @param [in] rect : 要获取数据的区域
         * @param [in] options : 获取数据选项
         * @param [out] lockedPitch : 返回锁定区域的pitch
         * @return 返回锁定的硬件数据地址
         * @see enum LockOptions
         */
        virtual void *lock(const Rect &rect, LockOptions options, 
            int32_t &lockedPitch);

        /**
         * @brief 获取硬件缓冲区数据
         * @param [in] options : 获取数据选项
         * @return 返回数据指针地址
         * @note 这个是默认获取整个像素缓冲区区域的数据
         * @see enum LockOptions
         */
        virtual void *lock(LockOptions options) override;

        /**
         * @brief 获取锁定硬件缓冲区不同渲染器实现接口
         * @param [in] rect : 要获取数据的区域
         * @param [in] options : 获取数据选项
         * @param [out] lockedPitch : 返回锁定区域的pitch
         * @return 返回锁定的硬件数据地址
         * @see enum LockOptions
         */
        virtual void *lockImpl(const Rect &rect, LockOptions options, 
            int32_t &lockedPitch) = 0;

        /**
         * @brief 复制数据到另外一个HardwarePixelBuffer
         * @param [in] dst : 目标pixel buffer
         * @param [in] dstRect : 目标数据区域，默认为nullptr的时候，
         *                       表示整个目标区域，会自动缩放匹配源区域
         * @param [in] srcRect : 源数据区域，默认为nullptr的时候，
         *                       表示整个源数据区域，会自动缩放匹配目标区域
         * @return 调用成功返回 T3D_ERR_OK
         * @remarks dstRect 和 srcRect 均为nullptr的时候，
         *          会自动缩放匹配源和目标区域
         */
        virtual TResult copyTo(HardwarePixelBufferPtr dst, 
            Rect *dstRect = nullptr, Rect *srcRect = nullptr);

        /**
         * @brief 按照源区域从image读取数据到目标区域
         * @param [in] image : 要读取的图像对象
         * @param [in] srcRect : 源数据区域，默认为nullptr的时候，
         *                       表示整个源目标区域，会自动缩放匹配目标区域
         * @param [in] dstRect : 目标数据区域，默认为nullptr的时候，
         *                       表示整个目标区域，会自动缩放匹配源区域
         * @return 调用成功返回 T3D_ERR_OK
         * @note dstRect和srcRect均为nullptr的时候，会自动缩放匹配源和目标区域
         */
        virtual TResult readImage(const Image &image, Rect *srcRect = nullptr, 
            Rect *dstRect = nullptr) = 0;

        /**
         * @brief 把指定源区域范围数据写到image的目标区域
         * @param [in] image : 要写入的图像对象
         * @param [in] dstRect : 目标区域，默认为nullptr的时候，
         *                      表示整个目标区域，会自动缩放匹配源区域
         * @param [in] srcRect : 源区域，默认为nullptr的时候，
         *                      表示整个源目标区域，会自动缩放匹配目标区域
         * @return 调用成功返回 T3D_ERR_OK
         * @note dstRect和srcRect均为nullptr的时候，会自动缩放匹配源和目标区域
         */
        virtual TResult writeImage(Image &image, Rect *dstRect = nullptr, 
            Rect *srcRect = nullptr) = 0;

        /**
         * @brief 返回像素缓冲宽度
         */
        uint32_t getWidth() const { return mWidth; }

        /**
         * @brief 返回像素缓冲高度
         */
        uint32_t getHeight() const { return mHeight; }

        /**
         * @brief 获取像素缓冲的pitch
         */
        size_t getPitch() const { return mPitch; }

        /**
         * @brief 获取像素缓冲的像素格式
         */
        PixelFormat getFormat() const { return mFormat; }

    protected:
        /**
         * @brief 构造函数
         * @param [in] width : 像素缓冲区宽度
         * @param [in] height : 像素缓冲区高度
         * @param [in] format : 像素缓冲区格式
         * @param [in] usage : 缓冲区用途
         * @param [in] useSystemMemory : 是否使用系统内存
         * @param [in] useShadowBuffer : 是否使用系统内存减少GPU显存读写次数
         */
        HardwarePixelBuffer(uint32_t width, uint32_t height, PixelFormat format,
            Usage usage, bool useSystemMemory, bool useShadowBuffer);

    private:
        /**
         * @brief 从父类继承而来，对于像素缓冲区来说，本接口没有意义，
         *      请使用readImage来代替
         */
        virtual size_t readData(size_t offset, size_t size, void *dst) override;

        /**
         * @brief 从父类继承而来，对于像素缓冲区来说，本接口没有意义，
         *      请使用writeImage来代替
         */
        virtual size_t writeData(size_t offset, size_t size, const void *src, 
            bool discardWholeBuffer /* = false */) override;

        /**
         * @brief 从父类继承而来，对于像素缓冲区来说，本接口没有意义，
         *      请使用lockImpl(const Rect &, LockOptions)来代替
         * @see void *lockImpl(const Rect &, LockOptions)
         */
        virtual void *lockImpl(size_t offset, size_t size, 
            LockOptions options) override;

    protected:
        uint32_t    mWidth;         /// 像素缓冲区的宽度
        uint32_t    mHeight;        /// 像素缓冲区的高度
        size_t      mPitch;         /// 像素缓冲区的pitch
        PixelFormat mFormat;        /// 像素缓冲区的格式
    };
}


#endif  /*__T3D_HARDWARE_PIXEL_BUFFER_H__*/
