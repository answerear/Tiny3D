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


#ifndef __T3D_ENGINE_H__
#define __T3D_ENGINE_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    /**
     * @brief ���������
     */
    class T3D_ENGINE_API Engine : public Singleton<Engine>
    {
        T3D_DISABLE_COPY(Engine);

    public:
        /**
         * @brief ���캯��
         */
        Engine();

        /**
         * @brief ��������
         */
        virtual ~Engine();

        /**
         * @brief ��������
         * @remarks �����һ��Ӧ�ö�Ҫ�ڵ��ñ��ӿ�֮�����Ч��
         */
        bool startup(/*Window *window = nullptr*/);

        /**
         * @brief ��������
         */
        bool run();

        /**
         * @brief ��Ⱦһ֡
         */
        void renderOneFrame();

//     protected:
//         /**
//         * @brief ���ڴ�С�ı�֪ͨ
//         * @param [in] w : �µĴ��ڿ��
//         * @param [in] h : �µĴ��ڸ߶�
//         * @return void
//         */
//         virtual void windowResized(int32_t w, int32_t h) override;
// 
//         /**
//         * @brief �����ƶ�֪ͨ
//         * @param [in] x : �µĴ���λ��
//         * @param [in] y : �µĴ���λ��
//         */
//         virtual void windowMoved(int32_t x, int32_t y) override;
// 
//         /**
//         * @brief �����¼�ѭ��֪ͨ
//         */
//         virtual void windowRender() override;
// 
//         /**
//         * @brief
//         */
//         virtual void windowClosed() override;

    protected:
        EventManager        *mEventMgr;             /// �¼�����������

        Window              *mWindow;               /// ����
        bool                mIsRunning;             /// �����Ƿ���������
    };

    #define T3D_ENGINE      (Engine::getInstance())
}


#endif  /*__T3D_APP_EVENT_LISTENER_H__*/
