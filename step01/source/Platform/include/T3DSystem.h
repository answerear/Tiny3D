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

#ifndef __T3D_SYSTEM_H__
#define __T3D_SYSTEM_H__


#include "T3DSingleton.h"
#include "T3DPlatformPrerequisites.h"


namespace Tiny3D
{
    class IFactory;

    /**
     * @class VSystem
     * @brief ϵͳ��.
     * @note �����һЩȫ�ֵĵ����������ͷţ�Ϊ�ϲ��ṩ��ܲ��ϵͳ��ͳһ�ӿ�.
     */
    class T3D_PLATFORM_API System : public Singleton<System>
    {
        T3D_DISABLE_COPY(System);

    public:
        /**
         * @brief Constructor for VSystem.
         */
        System();

        /**
         * @brief Destructor for VSystem.
         */
        ~System();

        /**
         * @brief ÿ������ѭ�����ô���.
         * @return void
         */
        void process();

        /**
         * @brief ��ȡ����ϵͳ����㹤���ӿڶ���
         */
        IFactory &getPlatformFactory()
        {
            return (*mPlatformFactory);
        }

    private:
        IFactory        *mPlatformFactory;
    };

    #define T3D_SYSTEM              (System::getInstance())
    #define T3D_PLATFORM_FACTORY    (T3D_SYSTEM.getPlatformFactory())
}


#endif
