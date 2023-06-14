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

#include "Thread/T3DQueuedThread.h"
#include "T3DPlatform.h"
#include "T3DQueuedJob.h"
#include "T3DQueuedJobPool.h"
#include "Thread/T3DRunnableThread.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------
    
    TResult QueuedThread::create(QueuedJobPoolDefault *pool, uint32_t stackSize, ThreadPriority priority, const String &name)
    {
        String threadName;
        
        if (name.empty())
        {
            static int32_t poolThreadIndex = 0;
            std::stringstream ss;
            ss << "PoolThread " << poolThreadIndex;
            threadName = ss.str();
        }
        else
        {
            threadName = name;
        }

        mOwner = pool;
        mThread = new RunnableThread();
        return mThread->start(this, threadName, stackSize, priority);
    }

    //--------------------------------------------------------------------------

    TResult QueuedThread::killThread()
    {
        mTimeToDie = true;
        mJobEvent.trigger();
        TResult ret = mThread->wait();
        delete mThread;
        return ret;
    }

    //--------------------------------------------------------------------------

    void QueuedThread::executeJob(IQueuedJob *job)
    {
        mQueuedJob = job;
        mJobEvent.trigger();
    }

    //--------------------------------------------------------------------------

    
    TResult QueuedThread::run()
    {
        while (!mTimeToDie.load(std::memory_order_relaxed))
        {
            mJobEvent.wait();

            IQueuedJob *job = mQueuedJob;
            mQueuedJob = nullptr;
            T3D_PLATFORM.memoryBarrier();
            T3D_ASSERT(job != nullptr || mTimeToDie.load(std::memory_order_relaxed), "Queued job must not be nullptr or time to die !");
            while (job != nullptr)
            {
                job->doThreadedJob();
                job = mOwner->returnToPoolAndGetNextJob(this);
            }
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
