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


#include "T3DTimerManager.h"
#include "T3DTimerListener.h"
#include "T3DDateTime.h"
#include "T3DPlatformErrorDef.h"

namespace Tiny3D
{
    T3D_INIT_SINGLETON(TimerManager);

    const uint32_t TimerManager::INVALID_TIMER_ID = 0;

    TimerManager::TimerManager()
    {

    }

    TimerManager::~TimerManager()
    {

    }

    uint32_t TimerManager::startTimer(uint32_t interval, bool repeat,
        ITimerListener *listener)
    {
        if (nullptr == listener)
            return INVALID_TIMER_ID;

        uint32_t timerID = mTimerID + 1;
        uint64_t timestamp = DateTime::currentMSecsSinceEpoch();
        Timer timer = { timestamp, interval, listener, repeat, true };
        auto r = mTimerList.insert(TimerValue(timerID, timer));
        if (r.second)
        {
            mTimerID = timerID;

            if (mTimerID == (uint32_t)-1)
            {
                mTimerID = 1;
            }
        }

        return mTimerID;
    }

    uint32_t TimerManager::startTimer(ITimerListener *listener)
    {
        if (nullptr == listener)
            return INVALID_TIMER_ID;

        uint32_t timerID = mTimerID + 1;
        uint64_t timestamp = DateTime::currentMSecsSinceEpoch();
        Timer timer = { timestamp, 0, listener, true, true };
        auto r = mTimerList.insert(TimerValue(timerID, timer));
        if (r.second)
        {
            mTimerID = timerID;

            if (mTimerID == (uint32_t)-1)
            {
                mTimerID = 1;
            }
        }

        return mTimerID;
    }

    int32_t TimerManager::stopTimer(uint32_t timerID)
    {
        int32_t ret = T3D_ERR_INVALID_TIMERID;

        auto itr = mTimerList.find(timerID);

        if (itr != mTimerList.end())
        {
            itr->second.alive = false;
            ret = T3D_ERR_OK;
        }

        return ret;
    }

    int32_t TimerManager::update()
    {
        auto itr = mTimerList.begin();

        uint64_t timestamp = DateTime::currentMSecsSinceEpoch();

        while (itr != mTimerList.end())
        {
            uint32_t timerID = itr->first;
            Timer &timer = itr->second;

            if (timer.alive)
            {
                // 还活着的定时器
                int64_t dt = (int64_t)timestamp - (int64_t)timer.timestamp;

                if (dt >= (int64_t)timer.interval)
                {
                    timer.timestamp = timestamp;

                    if (!timer.repeat)
                    {
                        // 不循环的定时器
                        timer.alive = false;
                    }

                    // 回调
                    timer.listener->onTimer(timerID, dt);
                }
            }

            ++itr;
        }

        // 删除掉所有死掉的定时器
        itr = mTimerList.begin();

        while (itr != mTimerList.end())
        {
            Timer &timer = itr->second;
            auto itrCur = itr++;

            if (!timer.alive)
            {
                mTimerList.erase(itrCur);
            }
        }

        return T3D_ERR_OK;
    }
}

