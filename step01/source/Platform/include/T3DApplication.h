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


#ifndef __T3D_APPLICATION_H__
#define __T3D_APPLICATION_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DSingleton.h"

namespace Tiny3D
{
    class System;
    class IApplication;

    class T3D_PLATFORM_API Application : public Singleton<Application>
    {
    public:
        /**
         * Constructor
         * @param [in] is GLApp : �Ƿ�OpenGLӦ�ó���
         */
        Application(bool isGLApp);

        /**
         * Destructor
         */
        virtual ~Application();

        /**
         * @brief Ӧ�ó�ʼ��������ɾ���ƽ̨�����ã��û�����Ҫ����
         * @return ���óɹ�����true�����򷵻�false
         */
        bool init();

        /**
         * @brief Ӧ���¼�����
         */
        void pollEvents();

        /**
         * @brief Ӧ�ó����˳��ͷ���Դ
         * @return void
         */
        void release();

        void *getNativeAppObject();

        virtual bool applicationDidFinishLaunching() = 0;

        virtual void applicationDidEnterBackground() = 0;

        virtual void applicationWillEnterForeground() = 0;

        virtual void applicationWillTerminate() = 0;

    private:
        System          *mSystem;       /// ����ƽ̨ϵͳ����
        IApplication    *mApp;          /// ƽ̨APP����
    };
}



#endif  /*__T3D_APPLICATION_H__*/
