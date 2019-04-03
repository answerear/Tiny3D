/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
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


#ifndef __T3D_HARDWARE_BUFFER_H__
#define __T3D_HARDWARE_BUFFER_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "T3DErrorDef.h"


namespace Tiny3D
{
    /**
     * @brief GPU 硬件缓冲区类，用于存储传递给GPU的数据缓冲区
     */
    class T3D_ENGINE_API HardwareBuffer : public Object
    {
    public:
        /**
         * @brief 缓冲区用法
         */
        enum Usage
        {
            /**< 静态缓存，数据放置在显存中，对于静态缓存读写数据是很慢的。
                一般用于创建后就不更新的数据 */
            E_HBU_STATIC = (1 << 0),
            /**< 动态缓存，数据存放在AGP内存中，这种数据能很快的更新 */
            E_HBU_DYNAMIC = (1 << 1),
            /**< 应用程序只能写缓存。 它允许驱动程序分配最适合的内存地址作为写
                缓存。 注意如果从创建好的这种缓存中读数据，将会返回错误信息。 */
            E_HBU_WRITE_ONLY = (1 << 2),
            /**< 联合E_HBU_STATIC和E_HBU_WRITE_ONLY */
            E_HBU_STATIC_WRITE_ONLY = (E_HBU_STATIC | E_HBU_WRITE_ONLY),
            /**< 联合E_HBU_DYNAMIC和E_HBU_WRITE_ONLY */
            E_HBU_DYNAMIC_WRITE_ONLY = (E_HBU_DYNAMIC | E_HBU_WRITE_ONLY),
        };

        /**
         * @brief 锁定缓冲区选项
         */
        enum LockOptions
        {
            E_HBL_NORMAL = 0,                   /**< 普通方式，能读写缓冲区 */
            E_HBL_DISCARD,                      /**< 缓冲区数据会被丢弃 */
            E_HBL_NO_OVERWRITE,                 /**< 不要覆盖缓冲区数据 */
            E_HBL_READ_ONLY,                    /**< 只读缓冲区 */
            E_HBL_WRITE_ONLY = E_HBL_DISCARD,   /**< 只写缓冲区 */
        };

        /**
         * @brief 析构函数
         */
        virtual ~HardwareBuffer();

        /**
         * @brief 锁定缓冲区
         * @param [in] offset : 锁定区域相对缓冲区开始的偏移位置
         * @param [in] size : 锁定缓冲区大小
         * @param [in] options : 锁定选项
         * @return 成功返回锁定区域首地址，否则返回nullptr。
         * @remarks 一般要读写缓冲区，需要先调用本接口
         */
        void *lock(size_t offset, size_t size, LockOptions options);

        /**
         * @brief 锁定整个缓冲区
         * @return 成功返回锁定区域首地址，否则返回nullptr。
         * @remarks 一般读写缓冲区，需要先调用本接口
         */
        virtual void *lock(LockOptions options);

        /**
         * @brief 解锁缓冲区
         * @return 成功调用返回 T3D_OK
         * @remarks 一般读写完缓冲区，需要调用本接口，否则数据无法传输到GPU上
         */
        TResult unlock();

        /**
         * @brief 从缓冲区读取数据出来
         * @param [in] offset : 要读取的相对缓冲区首地址的偏移位置
         * @param [in] size : 要读取缓冲区大小
         * @param [in][out] dst : 存储返回读取到数据的缓冲区首地址
         * @return 返回读取的字节数
         * @remarks 具体的渲染系统子类需要实现该接口
         */
        virtual size_t readData(size_t offset, size_t size, void *dst) = 0;

        /**
         * @brief 向缓冲区写数据
         * @param [in] offset : 要写入的相对缓冲区首地址的偏移位置
         * @param [in] size : 要写入缓冲区大小
         * @param [in] src : 写入的数据地址
         * @param [in] discardWholeBuffer : 是否直接丢弃硬件缓冲区，重建新缓冲区
         * @return 返回写入的字节数
         * @remarks 具体的渲染系统子类需要实现该接口
         */
        virtual size_t writeData(size_t offset, size_t size, const void *src,
            bool discardWholeBuffer = false) = 0;

        /**
         * @brief 从源缓冲区复制数据到本缓冲区里
         * @param [in] srcBuffer : 源缓冲区对象
         * @param [in] srcOffset : 源缓冲区偏移位置
         * @param [in] dstOffset : 目标缓冲区偏移位置
         * @param [in] size : 要复制的数据大小
         * @param [in] discardWholeBuffer : 是否丢弃原来硬件缓冲区，重建新缓冲区
         * @return 返回复制的字节数
         */
        virtual size_t copyData(HardwareBufferPtr srcBuffer, size_t srcOffset,
            size_t dstOffset, size_t size, bool discardWholeBuffer = false);

        /**
         * @brief 从源缓冲区全部复制数据到本缓冲区
         * @param [in] srcBuffer : 源缓冲区
         * @return 返回复制的字节数
         */
        virtual size_t copyData(HardwareBufferPtr srcBuffer);

        /**
         * @brief 获取缓冲区大小
         */
        size_t getBufferSize() const { return mBufferSize; }

        /**
         * @brief 获取缓冲区用法
         */
        Usage getUsage() const { return mUsage; }

        /**
         * @brief 获取是否使用系统内存
         */
        bool isSystemMemory() const { return mUseSystemMemory; }

        /**
         * @brief 获取是否使用影子缓存
         */
        bool hasShadowBuffer() const { return mUseShadowBuffer; }

        /**
         * @brief 获取缓存是否被锁
         */
        bool isLocked() const { return mIsLocked; }

    protected:
        /**
         * @brief 构造函数
         * @param [in] usage : 缓冲区用法
         * @param [in] isSystemMemory : 是否系统内存
         * @param [in] useShadowBuffer : 是否使用影子缓存
         */
        HardwareBuffer(Usage usage, bool useSystemMemory, bool useShadowBuffer);

        /**
         * @brief 锁定缓冲区的具体实现接口
         * @param [in] offset : 锁定区域相对缓冲区开始偏移位置
         * @param [in] size : 锁定区域大小
         * @param [in] options : 锁定选项
         * @return 返回锁定缓冲区区域首地址
         * @remarks 具体的渲染系统子类需要实现该接口
         */
        virtual void *lockImpl(size_t offset, size_t size, 
            LockOptions options) = 0;

        /**
         * @brief 解锁缓冲区的具体实现接口
         * @return 成功调用返回 T3D_OK
         * @remarks 具体的渲染系统子类需要实现该接口
         */
        virtual TResult unlockImpl() = 0;

        /**
         * @brief 从影子缓存更新数据到硬件缓冲区
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult updateFromShadow();

    protected:
        size_t  mBufferSize;            /**< 缓冲区大小 */
        size_t  mLockSize;              /**< 锁缓存大小 */
        size_t  mLockOffset;            /**< 锁缓存起始偏移 */

        Usage   mUsage;                 /**< 缓冲区用法类型 */

        HardwareBufferPtr   mShadowBuffer;  /**< 影子缓存，提高读写效率 */

        bool    mUseSystemMemory;       /**< 是否使用系统内存 */
        bool    mUseShadowBuffer;       /**< 是否使用影子缓存 */
        bool    mIsLocked;              /**< 硬件缓存是否被锁 */
        bool    mIsShadowBufferDirty;   /**< 影子缓存是否需要更新 */
    };
}


#endif  /*__T3D_HARDWARE_BUFFER_H__*/
