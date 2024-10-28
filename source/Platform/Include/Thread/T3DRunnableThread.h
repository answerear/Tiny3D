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

#ifndef __T3D_RUNNABLE_THREAD_H__
#define __T3D_RUNNABLE_THREAD_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DNoncopyable.h"
#include "T3DType.h"
#include "Thread/T3DSyncObject.h"
#include "Thread/T3DThreadConstant.h"
#include "T3DSingleton.h"
#include "Memory/T3DMemory.h"


namespace Tiny3D
{
    class IThread;
    class Runnable;

    /**
     * \brief 可运行线程类
     */
    class T3D_PLATFORM_API RunnableThread : public Allocator, public Noncopyable
    {
    public:
        /**
         * \brief Constructor
         */
        RunnableThread();

        /**
         * \brief Destructor
         */
        ~RunnableThread() override;

        /**
         * \brief 启动线程
         * \param [in] runnable : 可多线程执行对象
         * \param [in] name : 线程名称
         * \param [in] stackSize : 线程堆栈大小
         * \param [in] priority : 线程优先级，参考 ThreadPriority
         * \param [in] affinityMask : 线程亲缘性，最多支持 64 核
         * \return 调用成功返回 T3D_OK
         */
        TResult start(Runnable *runnable, const String &name, uint32_t stackSize = 0, ThreadPriority priority = ThreadPriority::kNormal, uint64_t affinityMask = kNoAffinityMask);

        /**
         * \brief 挂起线程执行
         * \return 调用成功返回 T3D_OK
         */
        TResult suspend();

        /**
         * \brief 继续线程执行
         * \return 调用成功返回 T3D_OK
         */
        TResult resume();

        /**
         * \brief 终止线程执行。 
         * \param [in] wait : 是否阻塞等待线程退出
         * \return 调用成功返回 T3D_OK
         */
        TResult terminate(bool wait);

        /**
         * \brief 等待线程结束
         * \return 调用成功返回 T3D_OK
         */
        TResult wait();

        /**
         * \brief 获取线程对象的线程 ID
         * \return 返回线程对象的线程 ID
         */
        ulong_t getID() const;

        /**
         * \brief 获取线程当前优先级
         * \return 返回线程当前优先级
         */
        ThreadPriority getPriority() const;

        /**
         * \brief 设置线程优先级
         * \param [in] priority : 线程优先级
         */
        void setPriority(ThreadPriority priority);

        /**
         * \brief 获取线程亲缘性 mask
         * \return 返回线程亲缘性 mask
         */
        uint64_t getAffinityMask() const;

        /**
         * \brief 设置线程亲缘性 mask
         * \param [in] mask : 线程亲缘性 mask 值
         */
        void setAffinityMask(uint64_t mask);

        /**
         * \brief 获取线程名称
         * \return 返回线程名称
         */
        const String getName() const { return mName; }

        /**
         * \brief 获取调用该函数的线程 ID
         * \return 返回当前线程 ID
         */
        static ulong_t getCurrentThreadID();

        /**
         * \brief 获取主线程 ID
         * \return 返回主线程 ID
         */
        static ulong_t getMainThreadID();

        /**
         * \brief 线程休眠
         * \param [in] msec : 休眠时长，单位：毫秒 
         */
        static void sleep(uint32_t msec);

        /**
         * \brief 获取硬件支持线程最大并发数，跟 CPU 核数有关
         * \return 返回硬件支持线程最大并发数
         */
        static uint32_t getThreadHardwareConcurrency();
        
    protected:
        TResult run();
        
        String      mName {};
        Event       mInitSyncEvent {};
        Runnable    *mRunnable {nullptr};
        IThread     *mThread {nullptr};
    };
}

#endif  /*__T3D_RUNNABLE_THREAD_H__*/
