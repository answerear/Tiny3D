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

#ifndef __T3D_WINDOW_EVENT_LISTENER_H__
#define __T3D_WINDOW_EVENT_LISTENER_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DType.h"
#include "T3DMacro.h"


namespace Tiny3D
{
    /**
     * @brief 窗口事件监听者
     */
    class T3D_PLATFORM_API WindowEventListener
    {
    public:
        T3D_DECLARE_INTERFACE(WindowEventListener);

        /**
         * @brief 窗口大小改变通知
         * @param [in] w : 新的窗口宽度
         * @param [in] h : 新的窗口高度
         * @return void
         */
        virtual void windowResized(int32_t w, int32_t h) = 0;

        /**
         * @brief 窗口移动通知
         * @param [in] x : 新的窗口位置
         * @param [in] y : 新的窗口位置
         */
        virtual void windowMoved(int32_t x, int32_t y) = 0;

        /**
         * @brief 窗口事件循环通知
         */
        virtual void windowRender() = 0;

        /**
         * @brief 
         */
        virtual void windowClosed() = 0;
    };
}


#endif  /*__T3D_WINDOW_EVENT_LISTENER_H__*/