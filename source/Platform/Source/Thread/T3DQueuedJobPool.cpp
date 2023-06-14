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

#include "Thread/T3DQueuedJobPool.h"
#include "T3DPlatform.h"
#include "T3DPlatformErrorDef.h"
#include "T3DQueuedJob.h"
#include "T3DRunnableThread.h"
#include "T3DWin32Dir.h"
#include "Thread/T3DQueuedThread.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    uint32_t QueuedJobPool::msOverrideStackSize = 0;
    
    //--------------------------------------------------------------------------

    QueuedJobPoolDefault::~QueuedJobPoolDefault()
    {
        destroy();
    }

    //--------------------------------------------------------------------------
    
    TResult QueuedJobPoolDefault::create(uint32_t numOfQueuedThreads, uint32_t stackSize, ThreadPriority priority, const String &name)
    {
        TResult ret = T3D_OK;
        
        do
        {
            if (numOfQueuedThreads == 0)
            {
                // 不允许 0 个线程的线程池
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }

            if (!mQueuedThreads.empty())
            {
                // 已经使用的线程池，无法再次创建
                ret = T3D_ERR_THREAD_ALREADY_CREATED;
                break;
            }
            
            ScopeLock lock(&mSyncQueue);

            if (msOverrideStackSize > stackSize)
            {
                // 用重写栈大小覆盖指定栈大小
                stackSize = msOverrideStackSize;
            }

            // 创建线程
            bool isOK = true;
        
            for (uint32_t i = 0; i < numOfQueuedThreads && isOK; ++i)
            {
                // 创建 QueuedThread
                QueuedThread *thread = new QueuedThread();
                std::stringstream ss;
                ss << name << " " << i;
                // 通过 QueuedThread 创建真正的线程
                ret = thread->create(this, stackSize, priority, ss.str());
                ss.str("");
                if (T3D_SUCCEEDED(ret))
                {
                    // 创建成功，放到池里
                    isOK = true;
                    mQueuedThreads.push_back(thread);
                    mAllThreads.push_back(thread);
                }
                else
                {
                    // 创建失败，清理线程对象
                    isOK = false;
                    delete thread;
                }
            }

            if (!isOK)
            {
                // 创建线程池失败，清理数据
                destroy();
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult QueuedJobPoolDefault::destroy()
    {
        // 放弃队列中所有的 job
        {
            ScopeLock lock(&mSyncQueue);

            mTimeToDie = true;
            T3D_PLATFORM.memoryBarrier();
            
            for (auto itr = mQueuedJobs.begin(); itr != mQueuedJobs.end(); ++itr)
            {
                IQueuedJob *job = *itr;
                job->abandon();
            }

            mQueuedJobs.clear();
        }
        
        // 等待所有线程执行完成, 注意这里并没有使用同步时间, 而是使用类似自旋锁的机制.
        while (true)
        {
            {
                // 访问 AllThreads 和 QueuedThreads 的数据时先锁定临界区. 防止其它线程修改数据.
                ScopeLock lock(&mSyncQueue);                
                if (mAllThreads.size() == mQueuedThreads.size())
                {
                    break;
                }
                // 切换当前线程时间片, 防止当前线程占用cpu时钟.
                RunnableThread::sleep(0);
            }
        }
        
        // 删除所有线程
        {
            ScopeLock lock(&mSyncQueue);
            for (auto itr = mAllThreads.begin(); itr != mAllThreads.end(); ++itr)
            {
                QueuedThread *thread = *itr;
                thread->killThread();
                delete thread;
            }

            mQueuedThreads.clear();
            mAllThreads.clear();
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult QueuedJobPoolDefault::addQueuedJob(IQueuedJob *job)
    {
        TResult ret = T3D_OK;
        
        do
        {
            if (job == nullptr)
            {
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }

            if (mTimeToDie)
            {
                // 超时，要放弃了
                job->abandon();
                break;
            }

            QueuedThread *thread = nullptr;

            {
                // 操作线程池里的所有数据前都需要锁定临界区.
                ScopeLock lock(&mSyncQueue);
                const size_t availableThreadCount = mQueuedThreads.size();

                // 没有可用线程, 加入任务队列, 稍后再执行.
                if (availableThreadCount == 0)
                {
                    mQueuedJobs.push_back(job);
                    break;
                }

                // 从可用线程池中获取一个线程, 并将其从可用线程池中删除.
                thread = mQueuedThreads.back();
                mQueuedThreads.pop_back();
            }

            // 执行任务
            thread->executeJob(job);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult QueuedJobPoolDefault::retractQueuedJob(IQueuedJob *job)
    {
        TResult ret = T3D_OK;

        do
        {
            if (job == nullptr)
            {
                // 参数错误
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }
            
            if (mTimeToDie)
            {
                // 超时
                break;
            }

            ScopeLock lock(&mSyncQueue);
            auto itr = std::find(mQueuedJobs.begin(), mQueuedJobs.end(), job);
            if (itr == mQueuedJobs.end())
            {
                ret = T3D_ERR_NOT_FOUND;
                break;
            }
            
            mQueuedJobs.erase(itr);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    IQueuedJob *QueuedJobPoolDefault::returnToPoolAndGetNextJob(QueuedThread *thread)
    {
        IQueuedJob *job = nullptr;

        ScopeLock lock(&mSyncQueue);
        
        if (mTimeToDie)
        {
            T3D_ASSERT(mQueuedJobs.empty(), "Queued jobs must be empty when thread pool timeout !");
        }
        else
        {
            // 从任务队列里面取出一个执行
            job = mQueuedJobs.front();
            mQueuedJobs.pop_front();
        }

        if (job == nullptr)
        {
            // 没有任务，线程返回池里
            mQueuedThreads.push_back(thread);
        }
        
        return job;
    }

    //--------------------------------------------------------------------------
}
