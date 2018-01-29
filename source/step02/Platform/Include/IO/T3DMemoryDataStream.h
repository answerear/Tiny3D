/*******************************************************************************
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
 ******************************************************************************/

#ifndef __T3D_MEMORY_DATA_STREAM_H__
#define __T3D_MEMORY_DATA_STREAM_H__


#include "IO/T3DDataStream.h"


namespace Tiny3D
{
    /**
     * @class T3DMemoryDataStream
     * @brief �ڴ���������.
     * @note �����ṩ�����ڴ��������Ĵ洢����.
     */
    class T3D_PLATFORM_API MemoryDataStream : public DataStream
    {
    public:
        static const uint32_t MEMORY_BUFFER_SIZE;   /**< Ĭ���ڴ�����С */

        MemoryDataStream();

        /**
         * @brief Constructor for T3DMemoryDataStream.
         * @note �ù��캯���ڲ����Զ�����ռ䣬ֱ�ӶԴ���Ļ���������
         * @param [in] pBuffer : ���ݻ���
         * @param [in] unSize : �����С
         * @param [in] reallocate : �Ƿ��ڲ����·���ռ䣬
         *                          Ĭ�������ڲ����·���ռ�洢����
         */
        MemoryDataStream(uchar_t *pBuffer, size_t unSize, bool reallocate = true);

        /**
         * @brief Constructor for T3DMemoryDataStream.
         * @note �ڲ��Զ�����ռ�.
         * @param [in] unSize : ����ռ��С��
         *                      Ĭ��ΪV_MEMORY_DATA_STREAM_BUFFER_SIZE.
         * @see V_MEMORY_DATA_STREAM_BUFFER_SIZE
         */
        MemoryDataStream(size_t unSize);

        /**
         * @brief Constructor for T3DMemoryDataStream.
         * @note ��������.
         * @param [in] other : Դ����
         */
        MemoryDataStream(const MemoryDataStream &other);

        /**
         * @brief Destructor for T3DMemoryDataStream.
         */
        virtual ~MemoryDataStream();

        /**
         * @brief ���ظ�ֵ������.
         */
        MemoryDataStream &operator =(const MemoryDataStream &other);

        /**
         * @brief ��ȡ�ڴ�������.
         * @param [in] pBuffer : ���ݻ�����
         * @param [in] nSize : ���ݻ�������С
         * @return ��ȡ�������ݴ�С
         */
        virtual size_t read(void *pBuffer, size_t nSize) override;

        /**
         * @brief д���ڴ�������.
         * @param [in] pBuffer : ���ݻ�����
         * @param [in] nSize : ���ݻ�������С
         * @return д�����ݴ�С
         */
        virtual size_t write(void *pBuffer, size_t nSize) override;

        /**
         * @brief �����ڴ���������дƫ��λ��.
         * @param [in] unPos : ƫ��λ��
         * @return �����óɹ�ʱ������true
         */
        virtual bool seek(long_t lPos, bool relative) override;

        /**
         * @brief ��ȡ��ǰ�ڴ���������дƫ��λ��.
         * @return ���ض�дƫ��λ��
         */
        virtual long_t tell() const override;

        /**
         * @brief ��ȡ�ڴ���������С.
         * @return �����ڴ���������С
         */
        virtual long_t size() const override;

        /**
         * @brief �Ƿ񵽴��ڴ�������ĩβ.
         * @return ����ĩβ����true�����򷵻�false.
         */
        virtual bool eof() const override;

        /**
         * @brief һ�ΰ�������������ȡ�겢�������buffer
         * @note ���ص�pData�ڲ������ⲿ���ø����ͷ�
         * @param [in][out] pData : ���ض�ȡ�������׵�ַ
         * @return ���ض�ȡ�ĳ���
         */
        virtual size_t read(uint8_t *&pData) override;

        void setBuffer(uint8_t *buffer, size_t bufSize, bool reallocate = true);

        void getBuffer(uint8_t *&buffer, size_t &bufSize) const;

    protected:
        void copy(const MemoryDataStream &other);

    protected:
        uchar_t     *m_pBuffer;     /**< ���ݻ����� */
        long_t      m_lSize;        /**< ���ݻ�������С */
        long_t      m_lCurPos;      /**< ��ǰ��дλ�� */

        bool        m_bCreated;     /**< �Ƿ��ڴ洴����� */
    };
}


#endif  /*__T3D_MEMORY_DATA_STREAM_H__*/
