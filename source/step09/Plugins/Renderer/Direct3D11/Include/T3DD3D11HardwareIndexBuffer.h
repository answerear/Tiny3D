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


#ifndef __T3D_D3D11_HARDWARE_INDEX_BUFFER_H__
#define __T3D_D3D11_HARDWARE_INDEX_BUFFER_H__


#include "T3DD3D11Prerequisites.h"


namespace Tiny3D
{
    /**
     * @class   D3D11HardwareIndexBuffer.
     * @brief   DirectX 11 渲染器相关的索引缓冲区类
     */
    class D3D11HardwareIndexBuffer : public HardwareIndexBuffer
    {
    public:
        /**
         * @fn  static D3D11HardwareIndexBufferPtr
         *  D3D11HardwareIndexBuffer::create(Type indexType,
         *  size_t indexCount, const void *indices,
         *  HardwareBuffer::Usage usage, bool useSystemMemory,
         *  bool useShadowBuffer);
         * @brief   创建 Direct3D9 渲染器相关的索引缓冲区对象
         * @param   indexType       Type of the index.
         * @param   indexCount      Number of indexes.
         * @param   indices         The indices.
         * @param   usage           The usage.
         * @param   useSystemMemory True to use system memory.
         * @param   useShadowBuffer True to use shadow buffer.
         * @returns A D3D11HardwareIndexBufferPtr.
         */
        static D3D11HardwareIndexBufferPtr create(Type indexType,
            size_t indexCount, const void *indices, HardwareBuffer::Usage usage,
            bool useSystemMemory, bool useShadowBuffer);

        /**
         * @fn  virtual D3D11HardwareIndexBuffer::~D3D11HardwareIndexBuffer();
         * @brief   析构函数
         */
        virtual ~D3D11HardwareIndexBuffer();

        /**
         * @fn  virtual size_t D3D11HardwareIndexBuffer::readData(size_t offset, 
         *      size_t size, void *dst) override;
         * @brief   从缓冲区读取数据出来，实现基类接口
         * @param           offset  The offset.
         * @param           size    The size.
         * @param [in,out]  dst     If non-null, destination for the.
         * @returns The data.
         * @sa  size_t HardwareVertexBuffer::readData(size_t offset,
         *      size_t size, void *dst)
         */
        virtual size_t readData(size_t offset, size_t size, void *dst) override;

        /**
         * @fn  virtual size_t D3D11HardwareIndexBuffer::writeData(
         *      size_t offset, size_t size, const void *src, 
         *      bool discardWholeBuffer = false) override;
         * @brief   向缓冲区写数据，实现基类接口
         * @param   offset              The offset.
         * @param   size                The size.
         * @param   src                 Source for the.
         * @param   discardWholeBuffer  (Optional) True to discard whole buffer.
         * @returns A size_t.
         * @sa  size_t HardwareVertexBuffer::writeData(size_t offset,
         *      size_t size, const void *src, bool discardWholeBuffer)
         */
        virtual size_t writeData(size_t offset, size_t size, const void *src,
            bool discardWholeBuffer = false) override;

    protected:
        /**
         * @fn  D3D11HardwareIndexBuffer::D3D11HardwareIndexBuffer(
         *      Type indexType, size_t indexCount, HardwareBuffer::Usage usage, 
         *      bool useSystemMemory, bool useShadowBuffer);
         * @brief   构造函数
         * @param   indexType       Type of the index.
         * @param   indexCount      Number of indexes.
         * @param   usage           The usage.
         * @param   useSystemMemory True to use system memory.
         * @param   useShadowBuffer True to use shadow buffer.
         */
        D3D11HardwareIndexBuffer(Type indexType, size_t indexCount,
            HardwareBuffer::Usage usage, bool useSystemMemory,
            bool useShadowBuffer);

        /**
         * @fn  virtual TResult 
         *      D3D11HardwareIndexBuffer::init(const void *indices);
         * @brief   初始化对象
         * @param   indices The indices.
         * @returns A TResult.
         */
        virtual TResult init(const void *indices);

        /**
         * @fn  virtual void D3D11HardwareIndexBuffer::*lockImpl(size_t offset, 
         *      size_t size, LockOptions options) override;
         * @brief   锁定缓冲区的具体实现接口，实现基类接口
         * @param   offset  The offset.
         * @param   size    The size.
         * @param   options Options for controlling the operation.
         * @returns Null if it fails, else a pointer to a void.
         * @sa  void *HardwareVertexBuffer::lockImpl(size_t offset, size_t size,
         *      LockOptions options)
         */
        virtual void *lockImpl(size_t offset, size_t size,
            LockOptions options) override;

        /**
         * @fn  virtual TResult D3D11HardwareIndexBuffer::unlockImpl() override;
         * @brief   解锁缓冲区的具体实现接口，实现基类接口
         * @returns A TResult.
         * @sa  TResult HardwareVertexBuffer::unlockImpl()
         */
        virtual TResult unlockImpl() override;

    protected:
    };
}


#endif  /*__T3D_D3D11_HARDWARE_INDEX_BUFFER_H__*/
