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

#ifndef __T3D_WINDOW_INTERFACE_H__
#define __T3D_WINDOW_INTERFACE_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DType.h"
#include "T3DMacro.h"
#include "T3DWindowEventListener.h"


namespace Tiny3D
{
    /**
     * @brief ƽ̨���ڳ�����
     * @remarks ��ͬƽ̨���ݽӿڸ���ʵ�־���Ĳ���
     */
    class IWindow
    {
        T3D_DECLARE_INTERFACE(IWindow);

    public:
        IWindow()
            : mEventListener(nullptr)
        {
        }

        /**
         * @brief ��������.
         * @param [in] x : ����λ��
         * @param [in] y : ����λ��
         * @param [in] w : ���ڿ��
         * @param [in] h : ���ڸ߶�
         * @param [in] isFullscreen : �Ƿ�ȫ��
         * @param [in] argc : ���ڸ��Ӳ�������
         * @param [in] args : ���Ӳ����б�
         * @return �����ɹ�����true�����򷵻�false.
         * @remarks ��ƽ̨���Զ���argc��ֵ��args������
         */
        virtual bool create(const char *title, int32_t x, int32_t y,
            int32_t w, int32_t h, bool isFullscreen, 
            int32_t argc, va_list args) = 0;

        /**
         * @brief ���ٴ���
         * @return void
         */
        virtual void destroy() = 0;

        /**
         * @brief �������¼�
         * @return void
         */
        virtual void pollEvents() = 0;

        /**
         * @brief ��Ⱦ����
         * @return void
         */
        virtual void render()
        {
            if (mEventListener != nullptr)
            {
                mEventListener->windowRender();
            }
        }

        /**
         * @brief ����ԭ�����ڶ���
         * @return ����ƽ̨ԭ�����ڶ�����߾��
         * @remarks ��ͬƽ̨���ز�ͬ�Ķ��󣬸��ݸ���ƽ̨���Խ���
         */
        virtual void *getNativeWinObject() = 0;

        virtual void setWindowEventListener(WindowEventListener *listener)
        {
            mEventListener = listener;
        }

    protected:
        WindowEventListener *mEventListener;
    };
}


#endif  /*__T3D_WINDOW_INTERFACE_H__*/