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

#include "Adapter/Unix/T3DPosixThread.h"
#include "T3DPlatformErrorDef.h"
#include "Thread/T3DRunnable.h"
#include <unistd.h>
#include <sched.h>
#include <time.h>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    PosixThread::PosixThread()
        : mThread(0)
        , mCreated(false)
        , mAffinityMask(0)
        , mPriority(ThreadPriority::kNormal)
        , mThreadRoutine(nullptr)
        , mParameter(nullptr)
        , mSuspended(false)
    {
        pthread_mutex_init(&mSuspendMutex, nullptr);
        pthread_cond_init(&mSuspendCond, nullptr);
    }

    //--------------------------------------------------------------------------

    PosixThread::~PosixThread()
    {
        pthread_mutex_destroy(&mSuspendMutex);
        pthread_cond_destroy(&mSuspendCond);
    }

    //--------------------------------------------------------------------------

    TResult PosixThread::start(ThreadRoutine routine, void *parameter, uint32_t stackSize)
    {
        TResult ret = T3D_OK;

        do
        {
            if (mCreated)
            {
                ret = T3D_ERR_THREAD_ALREADY_CREATED;
                break;
            }

            if (routine == nullptr || parameter == nullptr)
            {
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }

            mThreadRoutine = std::move(routine);
            mParameter = parameter;

            pthread_attr_t attr;
            pthread_attr_init(&attr);

            if (stackSize > 0)
            {
                pthread_attr_setstacksize(&attr, stackSize);
            }

            int result = pthread_create(&mThread, &attr, threadEntry, this);
            pthread_attr_destroy(&attr);

            if (result != 0)
            {
                ret = T3D_ERR_THREAD_CREATED;
                break;
            }

            mCreated = true;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult PosixThread::suspend()
    {
        TResult ret = T3D_OK;

        do
        {
            if (!mCreated)
            {
                ret = T3D_ERR_THREAD_NOT_CREATED;
                break;
            }

            pthread_mutex_lock(&mSuspendMutex);
            mSuspended = true;
            pthread_mutex_unlock(&mSuspendMutex);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult PosixThread::resume()
    {
        TResult ret = T3D_OK;

        do
        {
            if (!mCreated)
            {
                ret = T3D_ERR_THREAD_NOT_CREATED;
                break;
            }

            pthread_mutex_lock(&mSuspendMutex);
            mSuspended = false;
            pthread_cond_signal(&mSuspendCond);
            pthread_mutex_unlock(&mSuspendMutex);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult PosixThread::terminate(bool wait, Runnable *runnable)
    {
        TResult ret = T3D_OK;

        do
        {
            if (!mCreated)
            {
                ret = T3D_ERR_THREAD_NOT_CREATED;
                break;
            }

            if (runnable != nullptr)
            {
                runnable->stop();
            }

            // Make sure thread is not suspended
            resume();

            if (wait)
            {
                pthread_join(mThread, nullptr);
            }
            else
            {
                pthread_detach(mThread);
            }

            mCreated = false;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult PosixThread::wait()
    {
        TResult ret = T3D_OK;

        do
        {
            if (!mCreated)
            {
                ret = T3D_ERR_THREAD_NOT_CREATED;
                break;
            }

            pthread_join(mThread, nullptr);
            mCreated = false;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    ulong_t PosixThread::getID() const
    {
        return (ulong_t)mThread;
    }

    //--------------------------------------------------------------------------

    ThreadPriority PosixThread::getPriority() const
    {
        return mPriority;
    }

    //--------------------------------------------------------------------------

    void PosixThread::setPriority(ThreadPriority priority)
    {
        if (!mCreated)
            return;

        int policy;
        struct sched_param param;
        pthread_getschedparam(mThread, &policy, &param);

        int minPri = sched_get_priority_min(policy);
        int maxPri = sched_get_priority_max(policy);
        int range = maxPri - minPri;

        switch (priority)
        {
        case ThreadPriority::kIdle:
            param.sched_priority = minPri;
            break;
        case ThreadPriority::kLowest:
            param.sched_priority = minPri + range / 6;
            break;
        case ThreadPriority::kLow:
            param.sched_priority = minPri + range / 3;
            break;
        case ThreadPriority::kNormal:
            param.sched_priority = minPri + range / 2;
            break;
        case ThreadPriority::kHigh:
            param.sched_priority = minPri + range * 2 / 3;
            break;
        case ThreadPriority::kHighest:
            param.sched_priority = minPri + range * 5 / 6;
            break;
        case ThreadPriority::kTimeCritical:
            param.sched_priority = maxPri;
            break;
        case ThreadPriority::kInherit:
        default:
            break;
        }

        pthread_setschedparam(mThread, policy, &param);
        mPriority = priority;
    }

    //--------------------------------------------------------------------------

    uint64_t PosixThread::getAffinityMask() const
    {
        return mAffinityMask;
    }

    //--------------------------------------------------------------------------

    void *PosixThread::threadEntry(void *param)
    {
        PosixThread *self = static_cast<PosixThread *>(param);

        // Check if we should start suspended
        self->checkSuspend();

        TResult result = self->mThreadRoutine(self->mParameter);
        return (void *)(intptr_t)result;
    }

    //--------------------------------------------------------------------------

    void PosixThread::checkSuspend()
    {
        pthread_mutex_lock(&mSuspendMutex);
        while (mSuspended)
        {
            pthread_cond_wait(&mSuspendCond, &mSuspendMutex);
        }
        pthread_mutex_unlock(&mSuspendMutex);
    }

    //--------------------------------------------------------------------------
}
