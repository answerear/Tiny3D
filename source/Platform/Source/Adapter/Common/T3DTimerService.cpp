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

#include "T3DRunnableThread.h"


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

    ID TimerService::startTimer(uint32_t interval, bool repeat, const TimerCallback &callback)
    {
        if (nullptr == callback)
            return INVALID_TIMER_ID;

        int64_t timestamp = DateTime::currentMSecsSinceEpoch();
        int64_t timeout = timestamp + interval;

        ID timerID = timerID = mTimerID + 1;
        
        if (timerID == (uint32_t)-1)
        {
            // 越界了，重置为1
            timerID = 1;
        }

        mTimerID = timerID;
        Timer timer = { timestamp, interval, timerID, callback, repeat, true };
        
        if (mInCallback)
        {
            mToAddTimers.emplace(timeout, timer);
        }
        else
        {
            TAutoLock<TMutex> lockL(mTimerListMutex);
            mTimers.emplace(timeout, timer);
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

            {
                // 事件队列里面把这个ID的都干掉
                TAutoLock<TRecursiveMutex> lockQ(mEventListMutex);

                auto itrQ = std::find_if(
                    mTimerEventQueue.begin(),
                    mTimerEventQueue.end(),
                    [timerID](const TimerEvent &evt)
                    {
                        return (timerID == evt.timerID);
                    });
                if (itrQ != mTimerEventQueue.end())
                {
                    itrQ->alive = false;
                }
            }
            
            {
                if (mInCallback)
                {
                    mToDeleteTimers.push_back(timerID);
                }
                else
                {
                    TAutoLock<TMutex> lockL(mTimerListMutex);
                    deleteTimer(timerID);
                }
            }
        } while (false);

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
            // if (!mTimerList.empty())
            // {
            //     int64_t timestamp = DateTime::currentMSecsSinceEpoch();
            //
            //     TAutoLock<TRecursiveMutex> lockL(mTimerListMutex);
            //
            //     auto itr = mTimerList.begin();
            //
            //     while (itr != mTimerList.end())
            //     {
            //         ID timerID = itr->first;
            //         auto itrCur = itr++;
            //
            //         Timer &timer = itrCur->second;
            //         int32_t dt = int32_t(timestamp - timer.timestamp);
            //
            //         if (dt >= timer.interval)
            //         {
            //             timer.timestamp = timestamp;
            //
            //             // 放到事件队列里
            //             TimerEvent ev = { timerID, dt, timer.callback };
            //
            //             if (!timer.repeat)
            //             {
            //                 // 不循环的定时器，直接删掉
            //                 mTimerList.erase(itrCur);
            //             }
            //
            //             TAutoLock<TRecursiveMutex> lockQ(mEventListMutex);
            //             mTimerEventQueue.push_back(ev);
            //         }
            //     }
            //
            //     lockL.unlock();
            // }

            int64_t now = DateTime::currentMSecsSinceEpoch();
            TAutoLock<TMutex> lockL(mTimerListMutex);

            auto itr = mTimers.begin();
            while (!mTimers.empty() && itr->first <= now)
            {
                Timer &timer = itr->second;

                uint32_t dt = (uint32_t)(now - timer.timestamp);
                    
                // 放到事件队列里
                TimerEvent ev = {timer.timerID, dt, timer.callback, true};

                if (timer.repeat)
                {
                    // 循环定时器，更新时间戳，重新放到优先队列中
                    int64_t timeout = now + timer.interval;
                    mTimers.emplace(timeout, timer);
                }

                // 因为要更新到时时间戳，所以不管怎样，都从优先队列删掉
                itr = mTimers.erase(itr);
                    
                TAutoLock<TRecursiveMutex> lockQ(mEventListMutex);
                mTimerEventQueue.push_back(ev);
            }

            lockL.unlock();

            // 挂起10ms
            // std::this_thread::sleep_for(std::chrono::milliseconds(10));
            RunnableThread::sleep(10);
        }
    }

    //--------------------------------------------------------------------------

    TResult TimerService::pollEvents()
    {
        {
            // 查询事件并派发
            TAutoLock<TRecursiveMutex> lockQ(mEventListMutex);

            auto itr = mTimerEventQueue.begin();

            mInCallback = true;
        
            while (itr != mTimerEventQueue.end())
            {
                TimerEvent ev = *itr++;
                if (ev.alive)
                {
                    ev.callback(ev.timerID, ev.dt);
                }
            }
        
            mInCallback = false;

            mTimerEventQueue.clear();
        }

        {
            TAutoLock<TMutex> lockL(mTimerListMutex);

            // 删除定时器
            for (auto timerID : mToDeleteTimers)
            {
                deleteTimer(timerID);
            }

            mToDeleteTimers.clear();

            // 添加定时器
            for (const auto &item : mToAddTimers)
            {
                mTimers.emplace(item.first, item.second);
            }

            mToAddTimers.clear();
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    void TimerService::deleteTimer(uint32_t timerID)
    {
        auto it = std::find_if(
            mTimers.begin(),
            mTimers.end(),
            [timerID](const std::pair<int64_t, Timer>& item)
            {
                return (timerID == item.second.timerID);
            });
        if (it != mTimers.end())
        {
            mTimers.erase(it);
        }
    }

    //--------------------------------------------------------------------------
}
