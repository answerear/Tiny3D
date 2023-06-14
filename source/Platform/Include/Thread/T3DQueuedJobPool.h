/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
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

#ifndef __T3D_QUEUED_JOB_POOL_H__
#define __T3D_QUEUED_JOB_POOL_H__


#include "T3DType.h"
#include "T3DMacro.h"
#include "T3DNoncopyable.h"
#include "T3DPlatformPrerequisites.h"
#include "Thread/T3DThreadConstant.h"
#include "Thread/T3DSyncObject.h"


namespace Tiny3D
{
    class IQueuedJob;
    class QueuedThread;

    /**
     * \brief 执行 IQueuedJob 任务列表的线程池
     */
    class T3D_PLATFORM_API QueuedJobPool
    {
        T3D_DECLARE_INTERFACE(QueuedJobPool)
        
    public:
        /**
         * \brief 创建指定数量、栈大小、优先级的线程
         * \param [in] numOfQueuedThreads : 线程数量
         * \param [in] stackSize : 线程栈大小
         * \param [in] priority : 线程优先级
         * \param [in] name : 线程池名称 
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult create(uint32_t numOfQueuedThreads, uint32_t stackSize = (32 * 1024), ThreadPriority priority = ThreadPriority::kNormal, const String &name = "UnknownThreadPool") = 0;

        /**
         * \brief 销毁线程池内的后台线程
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult destroy() = 0;

        /**
         * \brief 加入队列化任务. 如果有可用的线程, 则立即执行; 否则会稍后再执行
         * \param [in] job : 要加入的队列化任务对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult addQueuedJob(IQueuedJob *job) = 0;

        /**
         * \brief 撤销指定队列化任务
         * \param [in] job : 要撤销的队列化任务对象
         * \return 调用成功返回 T3D_OK
         */
        virtual TResult retractQueuedJob(IQueuedJob *job) = 0;

        /**
         * \brief 获取线程数量
         * \return 返回线程数量
         */
        virtual size_t getNumOfThreads() const = 0;

    protected:
        static uint32_t msOverrideStackSize;
    };

    /**
     * \brief 默认队列化任务线程池
     */
    class T3D_PLATFORM_API QueuedJobPoolDefault : public QueuedJobPool, public Noncopyable
    {
        friend class QueuedThread;
        
    public:
        QueuedJobPoolDefault() = default;

        ~QueuedJobPoolDefault() override;

        TResult create(uint32_t numOfQueuedThreads, uint32_t stackSize, ThreadPriority priority, const String &name) override;

        TResult destroy() final;

        TResult addQueuedJob(IQueuedJob *job) override;

        TResult retractQueuedJob(IQueuedJob *job) override;

        size_t getNumOfQueuedJobs() const { return mQueuedJobs.size(); }

        size_t getNumOfThreads() const override { return mAllThreads.size(); }

    protected:
        IQueuedJob *returnToPoolAndGetNextJob(QueuedThread *thread);
        
    protected:
        using QueuedJobs = TList<IQueuedJob*>;
        using Threads = TList<QueuedThread*>;

        /// 需要执行的任务列表
        QueuedJobs      mQueuedJobs;
        /// 线程池内可用线程
        Threads         mQueuedThreads;
        /// 线程池内所有线程
        Threads         mAllThreads;
        /// 同步临界区对象
        CriticalSection mSyncQueue;
        /// 超时标记
        bool            mTimeToDie = false;
    };
}


#endif  /*__T3D_QUEUED_JOB_POOL_H__*/
