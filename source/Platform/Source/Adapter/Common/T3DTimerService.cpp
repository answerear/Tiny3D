/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#include "Adapter/Common/T3DTimerService.h"
#include "T3DTimerListener.h"
#include "T3DDateTime.h"
#include <chrono>
#include <functional>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const ID ITimerService::INVALID_TIMER_ID = T3D_INVALID_ID;

    //--------------------------------------------------------------------------

    TimerService::TimerService()
        : mTimerID(0)
        , mIsRunning(false)
        , mPollThread()
        , mTimerListMutex()
        , mEventListMutex()
    {

    }

    //--------------------------------------------------------------------------

    TimerService::~TimerService()
    {
        // 设置线程退出，等待线程结束，才析构
        if (mIsRunning)
        {
            mIsRunning = false;
            mPollThread.join();
        }
    }

    //--------------------------------------------------------------------------

    ID TimerService::startTimer(uint32_t interval, bool repeat,
        ITimerListener *listener)
    {
        if (nullptr == listener)
            return INVALID_TIMER_ID;

        int64_t timestamp = DateTime::currentMSecsSinceEpoch();
        Timer timer = { timestamp, interval, listener, repeat, true };

        TAutoLock<TMutex> lockL(mTimerListMutex);

        ID timerID = mTimerID + 1;
        auto r = mTimerList.insert(TimerValue(timerID, timer));
        if (r.second)
        {
            if (timerID == (uint32_t)-1)
            {
                // 越界了，重置为1
                timerID = 1;
            }

            mTimerID = timerID;
        }

        return timerID;
    }

    //--------------------------------------------------------------------------

    TResult TimerService::stopTimer(ID timerID)
    {
        TResult ret = T3D_OK;

        do
        {
            if (INVALID_TIMER_ID == timerID)
            {
                ret = T3D_ERR_INVALID_TIMERID;
                break;
            }

            TAutoLock<TMutex> lockL(mTimerListMutex);
            mTimerList.erase(timerID);
            lockL.unlock();

            // 事件队列里面把这个ID的都干掉
            TAutoLock<TRecursiveMutex> lockQ(mEventListMutex);

            auto itrQ = mTimerEventQueue.begin();

            while (itrQ != mTimerEventQueue.end())
            {
                auto itrCur = itrQ++;
                if (timerID == itrCur->timerID)
                {
                    mTimerEventQueue.erase(itrCur);
                }
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult TimerService::init()
    {
        TResult ret = T3D_OK;
        mIsRunning = true;
        mPollThread = TThread(std::bind(&TimerService::update, this));
        return ret;
    }

    //--------------------------------------------------------------------------

    void TimerService::update()
    {
        // 这里算法可以优化一下，使用二叉堆，可以快速的通过判断对顶元素是否时间
        // 到达触发回调来减少遍历

        while (mIsRunning)
        {
            if (!mTimerList.empty())
            {
                int64_t timestamp = DateTime::currentMSecsSinceEpoch();

                TAutoLock<TMutex> lockL(mTimerListMutex);

                auto itr = mTimerList.begin();

                while (itr != mTimerList.end())
                {
                    ID timerID = itr->first;
                    auto itrCur = itr++;

                    Timer &timer = itrCur->second;
                    int32_t dt = int32_t(timestamp - timer.timestamp);

                    if (dt >= timer.interval)
                    {
                        timer.timestamp = timestamp;

                        // 放到事件队列里
                        TimerEvent ev = { timerID, dt, timer.listener };

                        if (!timer.repeat)
                        {
                            // 不循环的定时器，直接删掉
                            mTimerList.erase(itrCur);
                        }

                        TAutoLock<TRecursiveMutex> lockQ(mEventListMutex);
                        mTimerEventQueue.push_back(ev);
                    }
                }

                lockL.unlock();
            }

            // 挂起10ms
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    //--------------------------------------------------------------------------

    TResult TimerService::pollEvents()
    {
        // 查询事件并派发
        TAutoLock<TRecursiveMutex> lockQ(mEventListMutex);

        auto itr = mTimerEventQueue.begin();

        while (itr != mTimerEventQueue.end())
        {
            TimerEvent ev = *itr++;
            ev.listener->onTimer(ev.timerID, ev.dt);
        }

        mTimerEventQueue.clear();

        return T3D_OK;
    }
}
