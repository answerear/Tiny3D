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

#ifndef __T3D_TIMER_MANAGER_H__
#define __T3D_TIMER_MANAGER_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DSingleton.h"

namespace Tiny3D
{
    class ITimerListener;

    class T3D_PLATFORM_API TimerManager : public Singleton<TimerManager>
    {
        friend class System;

    private:
        /**
         * @brief Constructor
         */
        TimerManager();

    public:
        static const uint32_t INVALID_TIMER_ID;

        /**
         * @brief Destructor
         */
        virtual ~TimerManager();

        /**
         * @brief 启动定时器
         */
        uint32_t startTimer(uint32_t interval, bool repeat, 
            ITimerListener *listener);

        uint32_t startTimer(ITimerListener *listener);

        int32_t stopTimer(uint32_t timerID);

    protected:
        int32_t update();

        struct Timer
        {
            uint64_t        timestamp;
            uint64_t        interval;
            ITimerListener  *listener;
            bool            repeat;
            bool            alive;
        };

        typedef std::map<uint32_t, Timer>   TimerList;
        typedef TimerList::iterator         TimerListItr;
        typedef TimerList::const_iterator   TimerListConstItr;
        typedef TimerList::value_type       TimerValue;

        TimerList   mTimerList;
        uint32_t    mTimerID;
    };

    #define T3D_TIMER_MGR       (TimerManager::getInstance())
}


#endif  /*__T3D_TIMER_MANAGER_H__*/
