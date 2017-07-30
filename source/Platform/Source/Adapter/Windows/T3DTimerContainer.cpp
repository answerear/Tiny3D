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

#include "T3DTimerContainer.h"
#include "T3DTimer_Windows.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(TimerContainer);

    TimerContainer::TimerContainer()
    {

    }

    TimerContainer::~TimerContainer()
    {

    }

    bool TimerContainer::insertTimer(uint32_t unTimerID, Timer_Windows *pTimer)
    {
        std::pair<TimerMap::iterator, bool> p = m_TimerMap.insert(T3DTimerPair(unTimerID, pTimer));
        return p.second;
    }

    void TimerContainer::removeTimer(uint32_t unTimerID)
    {
        TimerMap::iterator itr = m_TimerMap.find(unTimerID);
        if (itr != m_TimerMap.end())
        {
            m_TimerMap.erase(itr);
        }
    }

    Timer_Windows *TimerContainer::getTimerAdapter(uint32_t unTimerID)
    {
        Timer_Windows *pTimer = nullptr;
        TimerMap::iterator itr = m_TimerMap.find(unTimerID);

        if (itr != m_TimerMap.end())
        {
            pTimer = itr->second;
        }

        return pTimer;
    }
}
