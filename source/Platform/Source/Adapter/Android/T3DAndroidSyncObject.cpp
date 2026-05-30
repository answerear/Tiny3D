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

#include "Adapter/Android/T3DAndroidSyncObject.h"
#include "T3DCommonErrorDef.h"
#include <time.h>
#include <errno.h>


namespace Tiny3D
{
    //==========================================================================
    // AndroidCriticalSection
    //==========================================================================

    TResult AndroidCriticalSection::tryLock(uint32_t timeout)
    {
        if (timeout == 0)
        {
            int ret = pthread_mutex_trylock(&mMutex);
            return (ret == 0) ? T3D_OK : T3D_ERR_TIMEOUT;
        }

        if (timeout == (uint32_t)-1)
        {
            pthread_mutex_lock(&mMutex);
            return T3D_OK;
        }

        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout / 1000;
        ts.tv_nsec += (timeout % 1000) * 1000000L;
        if (ts.tv_nsec >= 1000000000L)
        {
            ts.tv_sec += 1;
            ts.tv_nsec -= 1000000000L;
        }

        int ret = pthread_mutex_timedlock(&mMutex, &ts);
        if (ret == ETIMEDOUT)
        {
            return T3D_ERR_TIMEOUT;
        }

        return (ret == 0) ? T3D_OK : T3D_ERR_FAIL;
    }

    //==========================================================================
    // AndroidMutex
    //==========================================================================

    TResult AndroidMutex::tryLock(uint32_t timeout)
    {
        if (timeout == 0)
        {
            int ret = pthread_mutex_trylock(&mMutex);
            return (ret == 0) ? T3D_OK : T3D_ERR_TIMEOUT;
        }

        if (timeout == (uint32_t)-1)
        {
            pthread_mutex_lock(&mMutex);
            return T3D_OK;
        }

        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout / 1000;
        ts.tv_nsec += (timeout % 1000) * 1000000L;
        if (ts.tv_nsec >= 1000000000L)
        {
            ts.tv_sec += 1;
            ts.tv_nsec -= 1000000000L;
        }

        int ret = pthread_mutex_timedlock(&mMutex, &ts);
        if (ret == ETIMEDOUT)
        {
            return T3D_ERR_TIMEOUT;
        }

        return (ret == 0) ? T3D_OK : T3D_ERR_FAIL;
    }

    //==========================================================================
    // AndroidRecursiveMutex
    //==========================================================================

    TResult AndroidRecursiveMutex::tryLock(uint32_t timeout)
    {
        if (timeout == 0)
        {
            int ret = pthread_mutex_trylock(&mMutex);
            return (ret == 0) ? T3D_OK : T3D_ERR_TIMEOUT;
        }

        if (timeout == (uint32_t)-1)
        {
            pthread_mutex_lock(&mMutex);
            return T3D_OK;
        }

        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout / 1000;
        ts.tv_nsec += (timeout % 1000) * 1000000L;
        if (ts.tv_nsec >= 1000000000L)
        {
            ts.tv_sec += 1;
            ts.tv_nsec -= 1000000000L;
        }

        int ret = pthread_mutex_timedlock(&mMutex, &ts);
        if (ret == ETIMEDOUT)
        {
            return T3D_ERR_TIMEOUT;
        }

        return (ret == 0) ? T3D_OK : T3D_ERR_FAIL;
    }

    //==========================================================================
    // AndroidSemaphore
    //==========================================================================

    AndroidSemaphore::AndroidSemaphore(uint32_t initCount, uint32_t maxCount)
        : PosixSemaphore(initCount, maxCount)
    {
    }

    //--------------------------------------------------------------------------

    TResult AndroidSemaphore::tryLock(uint32_t timeout)
    {
        if (timeout == (uint32_t)-1)
        {
            return lock();
        }

        pthread_mutex_lock(&mMutex);

        if (timeout == 0)
        {
            if (mCount > 0)
            {
                --mCount;
                pthread_mutex_unlock(&mMutex);
                return T3D_OK;
            }
            pthread_mutex_unlock(&mMutex);
            return T3D_ERR_TIMEOUT;
        }

        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout / 1000;
        ts.tv_nsec += (timeout % 1000) * 1000000L;
        if (ts.tv_nsec >= 1000000000L)
        {
            ts.tv_sec += 1;
            ts.tv_nsec -= 1000000000L;
        }

        while (mCount == 0)
        {
            int ret = pthread_cond_timedwait(&mCond, &mMutex, &ts);
            if (ret == ETIMEDOUT)
            {
                pthread_mutex_unlock(&mMutex);
                return T3D_ERR_TIMEOUT;
            }
        }

        --mCount;
        pthread_mutex_unlock(&mMutex);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
