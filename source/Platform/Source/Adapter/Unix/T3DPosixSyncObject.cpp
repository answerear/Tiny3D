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

#include "Adapter/Unix/T3DPosixSyncObject.h"
#include "T3DCommonErrorDef.h"
#include <time.h>
#include <errno.h>


namespace Tiny3D
{
    //==========================================================================
    // PosixCriticalSection
    //==========================================================================

    PosixCriticalSection::PosixCriticalSection()
    {
        pthread_mutex_init(&mMutex, nullptr);
    }

    //--------------------------------------------------------------------------

    PosixCriticalSection::~PosixCriticalSection()
    {
        pthread_mutex_destroy(&mMutex);
    }

    //--------------------------------------------------------------------------

    TResult PosixCriticalSection::lock()
    {
        pthread_mutex_lock(&mMutex);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult PosixCriticalSection::unlock()
    {
        pthread_mutex_unlock(&mMutex);
        return T3D_OK;
    }

    //==========================================================================
    // PosixMutex
    //==========================================================================

    PosixMutex::PosixMutex()
    {
        pthread_mutex_init(&mMutex, nullptr);
    }

    //--------------------------------------------------------------------------

    PosixMutex::~PosixMutex()
    {
        pthread_mutex_destroy(&mMutex);
    }

    //--------------------------------------------------------------------------

    TResult PosixMutex::lock()
    {
        pthread_mutex_lock(&mMutex);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult PosixMutex::unlock()
    {
        pthread_mutex_unlock(&mMutex);
        return T3D_OK;
    }

    //==========================================================================
    // PosixRecursiveMutex
    //==========================================================================

    PosixRecursiveMutex::PosixRecursiveMutex()
    {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&mMutex, &attr);
        pthread_mutexattr_destroy(&attr);
    }

    //--------------------------------------------------------------------------

    PosixRecursiveMutex::~PosixRecursiveMutex()
    {
        pthread_mutex_destroy(&mMutex);
    }

    //--------------------------------------------------------------------------

    TResult PosixRecursiveMutex::lock()
    {
        pthread_mutex_lock(&mMutex);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult PosixRecursiveMutex::unlock()
    {
        pthread_mutex_unlock(&mMutex);
        return T3D_OK;
    }

    //==========================================================================
    // PosixSemaphore
    //==========================================================================

    PosixSemaphore::PosixSemaphore(uint32_t initCount, uint32_t maxCount)
        : mCount(initCount)
        , mMaxCount(maxCount)
    {
        pthread_mutex_init(&mMutex, nullptr);
        pthread_cond_init(&mCond, nullptr);
    }

    //--------------------------------------------------------------------------

    PosixSemaphore::~PosixSemaphore()
    {
        pthread_cond_destroy(&mCond);
        pthread_mutex_destroy(&mMutex);
    }

    //--------------------------------------------------------------------------

    TResult PosixSemaphore::lock()
    {
        pthread_mutex_lock(&mMutex);
        while (mCount == 0)
        {
            pthread_cond_wait(&mCond, &mMutex);
        }
        --mCount;
        pthread_mutex_unlock(&mMutex);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult PosixSemaphore::unlock()
    {
        pthread_mutex_lock(&mMutex);
        if (mCount < mMaxCount)
        {
            ++mCount;
            pthread_cond_signal(&mCond);
        }
        pthread_mutex_unlock(&mMutex);
        return T3D_OK;
    }

    //==========================================================================
    // PosixEvent
    //==========================================================================

    PosixEvent::PosixEvent()
        : mTriggered(false)
    {
        pthread_mutex_init(&mMutex, nullptr);
        pthread_cond_init(&mCond, nullptr);
    }

    //--------------------------------------------------------------------------

    PosixEvent::~PosixEvent()
    {
        pthread_cond_destroy(&mCond);
        pthread_mutex_destroy(&mMutex);
    }

    //--------------------------------------------------------------------------

    TResult PosixEvent::wait(uint32_t timeout)
    {
        TResult ret = T3D_OK;

        pthread_mutex_lock(&mMutex);

        if (timeout == (uint32_t)-1)
        {
            // Infinite wait
            while (!mTriggered)
            {
                pthread_cond_wait(&mCond, &mMutex);
            }
        }
        else
        {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += timeout / 1000;
            ts.tv_nsec += (timeout % 1000) * 1000000L;
            if (ts.tv_nsec >= 1000000000L)
            {
                ts.tv_sec += 1;
                ts.tv_nsec -= 1000000000L;
            }

            while (!mTriggered)
            {
                int result = pthread_cond_timedwait(&mCond, &mMutex, &ts);
                if (result == ETIMEDOUT)
                {
                    if (!mTriggered)
                    {
                        ret = T3D_ERR_TIMEOUT;
                    }
                    break;
                }
            }
        }

        // Auto-reset after wait
        mTriggered = false;

        pthread_mutex_unlock(&mMutex);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult PosixEvent::trigger()
    {
        pthread_mutex_lock(&mMutex);
        mTriggered = true;
        pthread_cond_signal(&mCond);
        pthread_mutex_unlock(&mMutex);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult PosixEvent::reset()
    {
        pthread_mutex_lock(&mMutex);
        mTriggered = false;
        pthread_mutex_unlock(&mMutex);
        return T3D_OK;
    }

    //==========================================================================
    // PosixWaitCondition
    //==========================================================================

    PosixWaitCondition::PosixWaitCondition()
    {
        pthread_mutex_init(&mMutex, nullptr);
        pthread_cond_init(&mCond, nullptr);
    }

    //--------------------------------------------------------------------------

    PosixWaitCondition::~PosixWaitCondition()
    {
        pthread_cond_destroy(&mCond);
        pthread_mutex_destroy(&mMutex);
    }

    //--------------------------------------------------------------------------

    TResult PosixWaitCondition::wait(uint32_t timeout)
    {
        TResult ret = T3D_OK;

        pthread_mutex_lock(&mMutex);

        if (timeout == (uint32_t)-1)
        {
            pthread_cond_wait(&mCond, &mMutex);
        }
        else
        {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += timeout / 1000;
            ts.tv_nsec += (timeout % 1000) * 1000000L;
            if (ts.tv_nsec >= 1000000000L)
            {
                ts.tv_sec += 1;
                ts.tv_nsec -= 1000000000L;
            }

            int result = pthread_cond_timedwait(&mCond, &mMutex, &ts);
            if (result == ETIMEDOUT)
            {
                ret = T3D_ERR_TIMEOUT;
            }
        }

        pthread_mutex_unlock(&mMutex);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult PosixWaitCondition::wakeOne()
    {
        pthread_cond_signal(&mCond);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult PosixWaitCondition::wakeAll()
    {
        pthread_cond_broadcast(&mCond);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
