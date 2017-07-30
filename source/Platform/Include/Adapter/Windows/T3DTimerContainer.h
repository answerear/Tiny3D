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

#ifndef __T3D_TIMER_CONTAINER_H__
#define __T3D_TIMER_CONTAINER_H__


#include "T3DSingleton.h"
#include <map>


namespace Tiny3D
{
    class Timer_Windows;

    class TimerContainer : public Singleton<TimerContainer>
    {
        T3D_DISABLE_COPY(TimerContainer);

    public:
        TimerContainer();
        virtual ~TimerContainer();

        bool insertTimer(uint32_t unTimerID, Timer_Windows *pTimer);
        void removeTimer(uint32_t unTimerID);
        Timer_Windows *getTimerAdapter(uint32_t unTimerID);

    protected:
        typedef std::pair<uint32_t, Timer_Windows*> T3DTimerPair;
        typedef std::map<uint32_t, Timer_Windows*>  TimerMap;

        TimerMap    m_TimerMap;
    };
}


#endif  /*__T3D_TIMER_CONTAINER_H__*/
