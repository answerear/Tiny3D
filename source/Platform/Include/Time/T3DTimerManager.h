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

#ifndef __T3D_TIMER_MANAGER_H__
#define __T3D_TIMER_MANAGER_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DSingleton.h"
#include "T3DPlatformErrorDef.h"


namespace Tiny3D
{
    class ITimerListener;
    class ITimerService;

    class T3D_PLATFORM_API TimerManager : public Singleton<TimerManager>
    {
        friend class Platform;

    private:
        /**
         * @brief 构造函数
         */
        TimerManager();

    public:
        static const ID INVALID_TIMER_ID;

        /**
         * @brief 析构函数
         */
        virtual ~TimerManager();

        /**
         * @brief 启动定时器
         * @param [in] interval : 时间间隔
         * @param [in] repeat : 是否循环定时器
         * @param [in] callback : 定时器回调对象
         * @return 调用成功返回有效定时器ID，否则返回T3D_INVALID_TIMER_ID
         */
        ID startTimer(uint32_t interval, bool repeat, const TimerCallback &callback);

        /**
         * @brief 停止定时器
         * @param [in] timerID : 有效定时器ID，通过startTimer返回
         * @return 调用成功返回T3D_OK
         */
        TResult stopTimer(ID timerID);

    protected:
        /**
         * @brief 初始化定时器服务
         */
        TResult init();

        /**
         * @brief 轮询是否有定时器事件触发
         */
        TResult pollEvents();

    protected:
        ITimerService   *mTimerService;
    };

    #define T3D_TIMER_MGR       TimerManager::getInstance()

    #define T3D_INVALID_TIMER_ID    TimerManager::INVALID_TIMER_ID
}


#endif  /*__T3D_TIMER_MANAGER_H__*/
