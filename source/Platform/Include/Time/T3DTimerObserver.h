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

#ifndef __T3D_TIMER_OBSERVER_H__
#define __T3D_TIMER_OBSERVER_H__


#include "T3DType.h"
#include "T3DMacro.h"
#include "T3DPlatformPrerequisites.h"


namespace Tiny3D
{
    /**
     * @class ITimerObserver
     * @brief 定时器观察者接口类.
     * @note 继承该类实现接口以对定时器事件监听.
     */
    class T3D_PLATFORM_API TimerObserver
    {
        T3D_DECLARE_INTERFACE(TimerObserver);

    public:
        /**
         * @brief 定时器时间到达回调.
         * @param [in] unTimerID : 到达时间的定时器ID
         * @return void
         */
        virtual void onTimer(uint32_t unTimerID) = 0;
    };
}


#endif  /*__T3D_TIMER_OBSERVER_H__*/
