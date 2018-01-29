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

#ifndef __T3D_DIR_INTERFACE_H__
#define __T3D_DIR_INTERFACE_H__


#include "T3DType.h"
#include "T3DMacro.h"


namespace Tiny3D
{
    /**
     * @class IDirAdapter
     * @brief ����ϵͳ���ƽ̨���Ŀ¼�������ӿ���.
     * @note ��ز���ϵͳƽ̨��Ҫ�̳иýӿ���ʵ�־���ƽ̨��ع���.
     */
    class IDir
    {
        T3D_DECLARE_INTERFACE(IDir);

    public:
        /**
         * @brief ��ʼö������ָ��Ŀ¼�µ��ļ�.
         * @param [in] strPath : ָ��Ŀ¼�ļ���ʽ�ַ���
         * @return ���óɹ�����true��ʧ�ܷ���false.
         */
        virtual bool findFile(const String &strPath) = 0;

        /**
         * @brief ������һ���ļ�.
         * @return ö��û��������true�����򷵻�false��ʾö�ٽ���.
         */
        virtual bool findNextFile() = 0;

        /**
         * @brief ������ǰö������.
         * @return void
         */
        virtual void close() = 0;

        /**
         * @brief ��ȡö��Ŀ¼������·��.
         * @return ����ö��Ŀ¼��·������������std::string.
         */
        virtual String getRoot() const = 0;

        /**
         * @brief ��ȡ��ǰö�ٵ��������ļ���.
         * @return ���ص�ǰö�ٵ��������ļ���������std::string.
         */
        virtual String getFileName() const = 0;

        /**
         * @brief ��ȡ��ǰö�ٵ����ļ���ȫ·���������������ļ���.
         * @return ���ص�ǰö�ٵ����ļ���ȫ·����������std::string.
         */
        virtual String getFilePath() const = 0;

        /**
         * @brief ��ȡ��ǰö�ٵ����ļ��ļ�������������չ��.
         * @return ���ص�ǰö�ٵ����ļ��ļ���������std::string.
         */
        virtual String getFileTitle() const = 0;

        /**
         * @brief ��ȡ��ǰö�ٵ����ļ���С.
         * @return ���ص�ǰö�ٵ����ļ���С.
         */
        virtual uint32_t getLength() const = 0;

        /**
         * @brief �жϵ�ǰö�ٵ����ļ��Ƿ��Ŀ¼������'.'��'..' .
         * @return ��Ŀ¼����true�����򷵻�false.
         */
        virtual bool isDots() const = 0;

        /**
         * @brief �жϵ�ǰö�ٵ����ļ��Ƿ��ļ���.
         * @return �ļ�����true�����򷵻�false.
         */
        virtual bool isDirectory() const = 0;

        /**
         * @brief ��ȡ��ǰö�ٵ����ļ��Ĵ���ʱ���.
         * @return �����ļ�����ʱ�������ʱ���Ǵ�1970��1��1�յ���ǰ������
         */
        virtual long_t getCreationTime() const = 0;

        /**
         * @brief ��ȡ��ǰö�ٵ����ļ���������ʱ���.
         * @return �����ļ�������ʱ�������ʱ���Ǵ�1970��1��1�յ���ǰ������
         */
        virtual long_t getLastAccessTime() const = 0;

        /**
         * @brief ��ȡ��ǰö�ٵ����ļ�������޸�ʱ���.
         * @return �����ļ�����޸�ʱ�������ʱ���Ǵ�1970��1��1�յ���ǰ������
         */
        virtual long_t getLastWriteTime() const = 0;

        /**
         * @brief �����ļ���.
         * @note �ýӿڲ��ܴ����༶�ļ���.
         * @param [in] strDir : �ļ���·������
         * @return ���óɹ�����true�����򷵻�false.
         */
        virtual bool makeDir(const String &strDir) = 0;

        /**
         * @brief ɾ���ļ���.
         * @note �ýӿڲ���ɾ���༶�ļ���.
         * @param [in] strDir : �ļ���·������
         * @return ���óɹ�����true�����򷵻�false.
         */
        virtual bool removeDir(const String &strDir) = 0;

        /**
         * @brief ɾ���ļ�.
         * @param [in] strFileName : ��Ҫɾ���ļ�������.
         * @return ���óɹ�����true�����򷵻�false.
         */
        virtual bool remove(const String &strFileName) = 0;

        /**
         * @brief �ж�·����Ӧ���ļ��Ƿ����.
         * @param [in] strPath : ����·����
         * @return �ļ����ڷ���true�����򷵻�false.
         */
        virtual bool exists(const String &strPath) const = 0;

        /**
         * @brief ��ȡӦ�ó��򻺴����ݴ洢·������ͬƽָ̨����Ӧ��·��
         * @return ����Ӧ�ó��򻺴����ݴ洢·��.
         */
        virtual String getCachePath() const = 0;

        /**
         * @brief ��ȡӦ�ó���·��
         * @return ����Ӧ�ó���·��
         */
        virtual String getAppPath() const = 0;

        virtual char getNativeSeparator() const = 0;
    };
}


#endif  /*__T3D_DIR_INTERFACE_H__*/
