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

#ifndef __T3D_FILE_DATA_STREAM_H__
#define __T3D_FILE_DATA_STREAM_H__



#include "IO/T3DDataStream.h"
#include <stdio.h>


namespace Tiny3D
{
    /**
     * @class T3DFileDataStream
     * @brief �ļ�����.
     * @note �����ṩ�ļ�����д����.
     */
    class T3D_PLATFORM_API FileDataStream : public DataStream
    {
        T3D_DISABLE_COPY(FileDataStream);

    public:
        /**
         * @brief �ļ�����ģʽ
         */
        enum EOpenMode
        {
            /// �ļ�û�д�
            E_MODE_NOT_OPEN = 0x00000000L,
            /// ֻ����ʽ��
            E_MODE_READ_ONLY = 0x00000001L,
            /// ֻд��ʽ��
            E_MODE_WRITE_ONLY = 0x00000002L,
            /// ��д��ʽ��
            E_MODE_READ_WRITE = E_MODE_READ_ONLY | E_MODE_WRITE_ONLY,
            /// ׷�ӷ�ʽ��
            E_MODE_APPEND = 0x00000004L,
            /// �ļ��Ѿ�����ʱ���ԭ�����ݴ�
            E_MODE_TRUNCATE = 0x00000008L,
            /// �ı��ļ�
            E_MODE_TEXT = 0x00000010L,
        };

        /**
         * @brief Constructor for T3DFileDataStream.
         */
        FileDataStream();

        /**
         * @brief Destructor for T3DFileDataStream.
         */
        virtual ~FileDataStream();

        /**
         * @brief ���ļ���.
         * @param [in] szFileName : �ļ���
         * @param [in] eMode : �򿪷�ʽ
         * @return �򿪳ɹ�����true�����򷵻�false.
         * @see enum EOpenMode
         */
        bool open(const char *szFileName, uint32_t unMode);

        /**
         * @brief �ر��ļ���.
         * @return void
         */
        void close();

        /**
         * @brief ��ȡ�ļ���.
         * @param [in] pBuffer : ���ݻ�����
         * @param [in] nSize : ���ݻ�������С
         * @return ��ȡ�������ݴ�С.
         */
        virtual size_t read(void *pBuffer, size_t nSize) override;

        /**
         * @brief д���ļ���.
         * @param [in] pBuffer : ��д�����ݻ�����
         * @param [in] nSize : ���ݻ�������С
         * @return д������ݴ�С
         */
        virtual size_t write(void *pBuffer, size_t nSize) override;

        /**
         * @brief ���ļ��������ڴ������ȫ��ǿ��д���ļ���
         */
        void flush();

        /**
         * @brief �����ļ�����дƫ��λ��.
         * @param [in] llPos : ƫ��λ��
         * @return �����óɹ�ʱ�����ص�ǰ��дλ�ã�������ʱ������-1
         */
        virtual bool seek(long_t lPos, bool relative) override;

        /**
         * @brief ��ȡ��ǰ�ļ�����ǰ��дλ��.
         * @return �����ļ�����ǰ��дλ��
         */
        virtual long_t tell() const override;

        /**
         * @brief ��ȡ�ļ���С.
         * @return �����ļ���С
         */
        virtual long_t size() const override;

        /**
         * @brief ��ȡ�Ƿ��ļ���ĩβ.
         * @return �ѵ��ļ���ĩβ
         */
        virtual bool eof() const override;

        /**
         * @brief һ�ΰ�������������ȡ�겢�������buffer
         * @note ���ص�pData�ڲ������ⲿ���ø����ͷ�
         * @param [in][out] pData : ���ض�ȡ�������׵�ַ
         * @return ���ض�ȡ�ĳ���
         */
        virtual size_t read(uint8_t *&pData) override;

        bool isOpened() const   { return m_bIsOpened; }

    protected:
        FILE                *m_pFileHandle;     /**< �ļ�����ָ�� */
        mutable long_t      m_lSize;            /**< �ļ���С */
        bool                m_bIsOpened;        /**< �ļ��Ƿ�� */
        uint8_t             *m_pData;           /**< ��ȡ���������� */
    };
}


#endif  /*__T3D_FILE_DATA_STREAM_H__*/
