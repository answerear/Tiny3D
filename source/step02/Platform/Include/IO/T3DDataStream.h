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

#ifndef __T3D_DATA_STREAM_H__
#define __T3D_DATA_STREAM_H__


#include "T3DType.h"
#include "T3DMacro.h"
#include "T3DPlatformPrerequisites.h"


namespace Tiny3D
{
    class T3D_PLATFORM_API DataStream
    {
        T3D_DECLARE_INTERFACE(DataStream);

    public:
        /**
         * @brief ���������ж����ݵ�������.
         * @param [in] pBuffer : ���ݻ�����
         * @param [in] nSize : ���ݻ�������С
         * @return ���ض�ȡ�������ݴ�С.
         */
        virtual size_t read(void *pBuffer, size_t nSize) = 0;

        /**
         * @brief д���������ݵ���������.
         * @param [in] pBuffer : ���ݻ�����
         * @param [in] nSize : ���ݻ�������С
         * @return ����д������ݴ�С.
         */
        virtual size_t write(void *pBuffer, size_t nSize) = 0;

        /**
         * @brief ��������ָ���ƶ���ָ��λ��.
         * @param [in] llPos : ָ��ƫ��λ��
         * @param [in] relative : �Ƿ����λ��
         * @return �����óɹ�ʱ������true
         */
        virtual bool seek(long_t lPos, bool relative) = 0;

        /**
         * @brief ��ȡ��ǰ��������дλ��.
         * @return ���ص�ǰ��������дλ��.
         */
        virtual long_t tell() const = 0;

        /**
         * @brief ��ȡ�������Ĵ�С.
         * @return ���ص�ǰ�������Ĵ�С
         */
        virtual long_t size() const = 0;

        /**
         * @brief �Ƿ�������ĩβ.
         * @return ��ĩβ����true�����򷵻�false.
         */
        virtual bool eof() const = 0;

        /**
         * @brief һ�ΰ�������������ȡ�겢�������buffer
         * @note ���ص�pData�ڲ������ⲿ���ø����ͷ�
         * @param [in][out] pData : ���ض�ȡ�������׵�ַ
         * @return ���ض�ȡ�ĳ���
         */
        virtual size_t read(uint8_t *&pData) = 0;

        /**
         * @brief ��ȡһ���ı�
         * @param [in] trim : �Ƿ�ü���ǰ���tab���ո����Ч�ַ�
         * @return ����һ���ı�
         */
        String readLine(bool trim = true);

        /**
         * @brief дһ���ı�
         * @param [in] strLine : �ı���
         * @return ����д���ı�����
         */
        size_t writeLine(const String &strLine);

        /**
         * @brief �����������ֵ��������
         */
        DataStream &operator<<(bool val);
        DataStream &operator<<(int8_t val);
        DataStream &operator<<(uint8_t val);
        DataStream &operator<<(int16_t val);
        DataStream &operator<<(uint16_t val);
        DataStream &operator<<(int32_t val);
        DataStream &operator<<(uint32_t val);
        DataStream &operator<<(int64_t val);
        DataStream &operator<<(uint64_t val);
        DataStream &operator<<(float val);
        DataStream &operator<<(double val);
        DataStream &operator<<(const char *s);
        DataStream &operator<<(const String &s);

        /**
         * @brief ��������������������ֵ
         */
        DataStream &operator>>(bool &val);
        DataStream &operator>>(int8_t &val);
        DataStream &operator>>(uint8_t &val);
        DataStream &operator>>(int16_t &val);
        DataStream &operator>>(uint16_t &val);
        DataStream &operator>>(int32_t &val);
        DataStream &operator>>(uint32_t &val);
        DataStream &operator>>(int64_t &val);
        DataStream &operator>>(uint64_t &val);
        DataStream &operator>>(float &val);
        DataStream &operator>>(double &val);
        DataStream &operator>>(char *&s);
        DataStream &operator>>(String &s);
    };
}

#endif  /*__T3D_DATA_STREAM_H__*/
