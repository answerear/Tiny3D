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

#ifndef __T3D_TIEMR_SERVICE_H__
#define __T3D_TIEMR_SERVICE_H__


#include "Adapter/T3DTimerInterface.h"
#include "T3DPlatformErrorDef.h"


namespace Tiny3D
{
    class TimerService : public ITimerService
    {
    public:
        /** 构造函数 */
        TimerService();

        /** 析构函数 */
        virtual ~TimerService();

    protected:
        /**
         * @brief 启动定时器
         * @param [in] interval : 时间间隔
         * @param [in] repeat : 是否循环定时器
         * @param [in] listener : 定时器回调对象
         * @return 调用成功返回有效定时器ID，否则返回T3D_INVALID_TIMER_ID
         */
        virtual ID startTimer(uint32_t interval, bool repeat,
            ITimerListener *listener) override;

        /**
         * @brief 停止定时器
         * @param [in] timerID : 有效定时器ID，通过startTimer返回
         * @return 调用成功返回T3D_OK
         */
        virtual TResult stopTimer(ID timerID) override;

        /**
         * @brief 初始化定时器服务
         */
        virtual TResult init() override;

        /**
         * @brief 轮询是否有定时器事件触发
         */
        virtual TResult pollEvents() override;

        /** 线程函数，循环检测是否超时 */
        void update();

        struct Timer
        {
            int64_t         timestamp;  /**< 定时器启动时间戳 */
            int64_t         interval;   /**< 定时器触发间隔 */
            ITimerListener  *listener;  /**< 定时器触发的监听对象 */
            bool            repeat;     /**< 定时器是否循环 */
            bool            alive;      /**< 定时器是否有效 */
        };

        struct TimerEvent
        {
            ID              timerID;    /**< 定时器ID */
            int32_t         dt;         /**< 实际时间间隔 */
            ITimerListener  *listener;  /**< 定时器触发的监听对象 */
        };

        typedef TMap<uint32_t, Timer>       TimerList;
        typedef TimerList::iterator         TimerListItr;
        typedef TimerList::const_iterator   TimerListConstItr;
        typedef TimerList::value_type       TimerValue;

        typedef TList<TimerEvent>               TimerEventQueue;
        typedef TimerEventQueue::iterator       TimerEventQueueItr;
        typedef TimerEventQueue::const_iterator TimerEventQueueConstItr;


        TimerList       mTimerList;         /// 定时器对象列表
        TimerEventQueue mTimerEventQueue;   /// 定时器事件队列
        ID              mTimerID;           /// 当前定时器ID，用于下一个生成的ID

        bool            mIsRunning;         /// 轮询线程是否在运行

        TThread         mPollThread;        /// 轮询线程
        TMutex          mTimerListMutex;    /// 操作定时器对象列表的互斥量
        TRecursiveMutex mEventListMutex;    /// 操作事件队列的互斥量
    };
}


#endif  /*__T3D_TIEMR_SERVICE_H__*/