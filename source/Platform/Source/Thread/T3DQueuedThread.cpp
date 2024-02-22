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
                job->execute();
                job = mOwner->returnToPoolAndGetNextJob(this);
            }
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
