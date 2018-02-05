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

        int32_t stopTimer(uint32_t timerID);

    protected:
        int32_t init();

        int32_t pollEvents();

        void update();

        struct Timer
        {
            int64_t         timestamp;  /// 定时器启动时间戳
            int64_t         interval;   /// 定时器触发间隔
            ITimerListener  *listener;  /// 定时器触发的监听对象
            bool            repeat;     /// 定时器是否循环
            bool            alive;      /// 定时器是否有效
        };

        struct TimerEvent
        {
            uint32_t        timerID;    /// 定时器ID
            int32_t         dt;         /// 实际时间间隔
            ITimerListener  *listener;  /// 定时器触发的监听对象
        };

        typedef std::map<uint32_t, Timer>   TimerList;
        typedef TimerList::iterator         TimerListItr;
        typedef TimerList::const_iterator   TimerListConstItr;
        typedef TimerList::value_type       TimerValue;

        typedef std::list<TimerEvent>           TimerEventQueue;
        typedef TimerEventQueue::iterator       TimerEventQueueItr;
        typedef TimerEventQueue::const_iterator TimerEventQueueConstItr;


        TimerList       mTimerList;         /// 定时器对象列表
        TimerEventQueue mTimerEventQueue;   /// 定时器事件队列
        uint32_t        mTimerID;           /// 当前定时器ID，用于下一个生成的ID

        bool            mIsRunning;         /// 轮询线程是否在运行

        TThread         mPollThread;        /// 轮询线程
        TMutex          mTimerListMutex;    /// 操作定时器对象列表的互斥量
        TRecursiveMutex mEventListMutex;    /// 操作事件队列的互斥量
    };

    #define T3D_TIMER_MGR       TimerManager::getInstance()

    #define T3D_INVALID_TIMER_ID    TimerManager::INVALID_TIMER_ID
}


#endif  /*__T3D_TIMER_MANAGER_H__*/
