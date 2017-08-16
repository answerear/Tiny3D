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

#ifndef __T3D_MEMORY_TRACER_H__
#define __T3D_MEMORY_TRACER_H__


#include "T3DPrerequisites.h"
#include "Misc/T3DObject.h"


namespace Tiny3D
{
    /**
     * @class MemoryTracer
     * @brief һ�������ڴ���࣬�ܹ���������Object���������ڴ�й©�����
     * @note ����ĸ��ٵ����������ܴ��ʱ�����һ����������ģ����������ڶ�λ���ڴ�����͹رյ�����
     */
    class T3D_ENGINE_API MemoryTracer : public Singleton<MemoryTracer>
    {
        friend class Entrance;
        friend class Object;

    public:
        /**
         * @brief ��������
         */
        virtual ~MemoryTracer();

        /**
         * @brief ��ȡ�ڴ�����Ƿ��
         * @return �����ڴ����������״̬
         */
        bool isTracingEnabled() const { return mIsEnabled; }

        /**
         * @brief �����ǰ�ڴ���Ϣ����Ӧƽ̨Console
         * @return void
         * @note ��������������Ӧƽ̨��console�ϣ���ϸƽ̨���£�
         *  - ��windows�ϣ�����������console�ϣ����һ������vs��Output������
         *  - ��iOS��Mac OS X�ϣ�����������xcode��Console������
         *  - ��Android�ϣ�����������logcat��
         */
        void dumpMemoryInfo() const;

        /**
         * @brief �����ǰ�ڴ���Ϣ���ļ�����
         * @param [in] fs : �ļ���
         * @return void
         */
        void dumpMemoryInfo(FileDataStream &fs) const;

    protected:
        /**
         * @brief Ĭ�Ϲ��캯��
         * @param [in] enabled : �Ƿ����ڴ���ٹ��ܣ�Ĭ���ǹرյ�
         */
        MemoryTracer(bool enabled = false);

        /**
         * @brief ���һ�������ڴ��������
         */
        void addObject(Object *object)
        {
            if (mIsEnabled)
                mObjects.insert(object);
        }

        /**
         * @brief ���ڴ���������Ƴ�һ������
         */
        void removeObject(Object *object)
        {
            if (mIsEnabled)
                mObjects.erase(object);
        }

        /** 
         * @brief �����Ϣ
         * @param [in] str : ��Ϣ�ַ���
         */
        void printInfo(const String &str) const;

        typedef std::set<Object*>       Objects;
        typedef Objects::iterator       ObjectsItr;
        typedef Objects::const_iterator ObjectsConstItr;
        typedef Objects::value_type     ObjectsValue;

        bool                    mIsEnabled;     /// �Ƿ������ڴ����
        Objects                 mObjects;       /// ���󼯺�

        mutable FileDataStream  *mStream;       /// ��ʱ�����������dumpMemoryInfo��ʱ��
    };
}


#endif  /*__T3D_MEMORY_TRACER_H__*/
