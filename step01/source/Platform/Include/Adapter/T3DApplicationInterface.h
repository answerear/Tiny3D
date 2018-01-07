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


#ifndef __T3D_APPLICATION_INTERFACE_H__
#define __T3D_APPLICATION_INTERFACE_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DType.h"
#include "T3DMacro.h"


namespace Tiny3D
{
    class T3D_PLATFORM_API IApplication
    {
    public:
        T3D_DECLARE_INTERFACE(IApplication);

        /**
         * @brief ��ʼ��Ӧ�ó���
         * @return ���óɹ�����true�����򷵻�false
         */
        virtual bool init() = 0;

        /**
         * @brief ��ѯ����Ӧ�ó����¼�
         */
        virtual void pollEvents() = 0;

        /**
         * @brief �ͷ�Ӧ�ó��������Դ
         */
        virtual void release() = 0;

        /**
         * @brief ��ȡƽ̨ԭ��Ӧ�ö���
         */
        virtual void *getNativeAppObject() = 0;
    };
}



#endif  /*__T3D_APPLICATION_INTERFACE_H__*/
