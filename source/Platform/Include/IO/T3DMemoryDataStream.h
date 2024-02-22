﻿/*******************************************************************************
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

#ifndef __T3D_MEMORY_DATA_STREAM_H__
#define __T3D_MEMORY_DATA_STREAM_H__


#include "IO/T3DDataStream.h"


namespace Tiny3D
{
    /**
     * @class T3DMemoryDataStream
     * @brief 内存数据流类.
     * @note 该类提供基于内存数据流的存储访问.
     */
    class T3D_PLATFORM_API MemoryDataStream : public DataStream
    {
    public:
        static const uint32_t MEMORY_BUFFER_SIZE;   /**< 默认内存流大小 */

        MemoryDataStream();

        /**
         * @brief Constructor for T3DMemoryDataStream.
         * @note 该构造函数内部不自动非配空间，直接对传入的缓冲区操作
         * @param [in] pBuffer : 数据缓冲
         * @param [in] unSize : 缓冲大小
         * @param [in] reallocate : 是否内部重新分配空间，
         *                          默认由类内部重新分配空间存储数据
         */
        MemoryDataStream(uchar_t *pBuffer, size_t unSize, bool reallocate = true);

        /**
         * @brief Constructor for T3DMemoryDataStream.
         * @note 内部自动分配空间.
         * @param [in] unCapacity : 分配空间大小，
         *                      默认为V_MEMORY_DATA_STREAM_BUFFER_SIZE.
         * @see V_MEMORY_DATA_STREAM_BUFFER_SIZE
         */
        MemoryDataStream(size_t unCapacity);

        /**
         * @brief Constructor for T3DMemoryDataStream.
         * @note 拷贝构造.
         * @param [in] other : 源对象
         */
        MemoryDataStream(const MemoryDataStream &other);

        /**
         * @brief Destructor for T3DMemoryDataStream.
         */
        virtual ~MemoryDataStream();

        /**
         * @brief 重载赋值操作符.
         */
        MemoryDataStream &operator =(const MemoryDataStream &other);

        /**
         * @brief 读取内存数据流.
         * @param [in] pBuffer : 数据缓冲区
         * @param [in] nSize : 数据缓冲区大小
         * @return 读取到的数据大小
         */
        virtual size_t read(void *pBuffer, size_t nSize) override;

        /**
         * @brief 写入内存数据流.
         * @param [in] pBuffer : 数据缓冲区
         * @param [in] nSize : 数据缓冲区大小
         * @return 写入数据大小
         */
        virtual size_t write(void *pBuffer, size_t nSize) override;

        /**
         * @brief 设置内存数据流读写偏移位置.
         * @param [in] unPos : 偏移位置
         * @return 当调用成功时，返回true
         */
        virtual bool seek(long_t lPos, bool relative) override;

        /**
         * @brief 获取当前内存数据流读写偏移位置.
         * @return 返回读写偏移位置
         */
        virtual long_t tell() const override;

        /**
         * @brief 获取内存数据流大小.
         * @return 返回内存数据流大小
         */
        virtual long_t size() const override;

        /**
         * @brief 是否到达内存数据流末尾.
         * @return 到达末尾返回true，否则返回false.
         */
        virtual bool eof() const override;

        /**
         * @brief 一次把整个数据流读取完并返回这块buffer
         * @note 返回的pData内部管理，外部不用负责释放
         * @param [in][out] pData : 返回读取的数据首地址
         * @return 返回读取的长度
         */
        virtual size_t read(uint8_t *&pData) override;

        void setBuffer(uint8_t *buffer, size_t bufSize, bool reallocate = true);

        void getBuffer(uint8_t *&buffer, size_t &bufSize) const;

    protected:
        void copy(const MemoryDataStream &other);

    protected:
        uchar_t     *m_pBuffer;     /**< 数据缓冲区 */
        long_t      m_lSize;        /**< 数据缓冲区大小 */
        long_t      m_lCurPos;      /**< 当前读写位置 */
        long_t      m_lCapacity;    /**< 缓冲区容量 */

        bool        m_bCreated;     /**< 是否内存创建标记 */
    };
}


#endif  /*__T3D_MEMORY_DATA_STREAM_H__*/
