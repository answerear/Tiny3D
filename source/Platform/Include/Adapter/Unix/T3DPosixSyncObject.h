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

#ifndef __T3D_POSIX_SYNC_OBJECT_H__
#define __T3D_POSIX_SYNC_OBJECT_H__


#include "T3DNoncopyable.h"
#include "Adapter/T3DSyncObjectinterface.h"
#include "Memory/T3DMemory.h"
#include <pthread.h>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    class PosixCriticalSection : public ICriticalSection, public Noncopyable
    {
    public:
        PosixCriticalSection();

        ~PosixCriticalSection() override;

        TResult lock() override;

        virtual TResult tryLock(uint32_t timeout) override = 0;

        TResult unlock() override;

    protected:
        pthread_mutex_t mMutex;
    };

    //--------------------------------------------------------------------------

    class PosixMutex : public IMutex, public Noncopyable
    {
    public:
        PosixMutex();

        ~PosixMutex() override;

        TResult lock() override;

        virtual TResult tryLock(uint32_t timeout) override = 0;

        TResult unlock() override;

    protected:
        pthread_mutex_t mMutex;
    };

    //--------------------------------------------------------------------------

    class PosixRecursiveMutex : public IRecursiveMutex, public Noncopyable
    {
    public:
        PosixRecursiveMutex();

        ~PosixRecursiveMutex() override;

        TResult lock() override;

        virtual TResult tryLock(uint32_t timeout) override = 0;

        TResult unlock() override;

    protected:
        pthread_mutex_t mMutex;
    };

    //--------------------------------------------------------------------------

    class PosixSemaphore : public ISemaphore, public Noncopyable
    {
    public:
        PosixSemaphore(uint32_t initCount = 0, uint32_t maxCount = 1);

        ~PosixSemaphore() override;

        TResult lock() override;

        virtual TResult tryLock(uint32_t timeout) override = 0;

        TResult unlock() override;

    protected:
        pthread_mutex_t mMutex;
        pthread_cond_t  mCond;
        uint32_t        mCount;
        uint32_t        mMaxCount;
    };

    //--------------------------------------------------------------------------

    class PosixEvent : public IEvent, public Noncopyable
    {
    public:
        PosixEvent();

        ~PosixEvent() override;

        TResult wait(uint32_t timeout) override;

        TResult trigger() override;

        TResult reset() override;

    protected:
        pthread_mutex_t mMutex;
        pthread_cond_t  mCond;
        bool            mTriggered;
    };

    //--------------------------------------------------------------------------

    class PosixWaitCondition : public IWaitCondition, public Noncopyable
    {
    public:
        PosixWaitCondition();

        ~PosixWaitCondition() override;

        TResult wait(uint32_t timeout) override;

        TResult wakeOne() override;

        TResult wakeAll() override;

    protected:
        pthread_mutex_t mMutex;
        pthread_cond_t  mCond;
    };
}


#endif  /*__T3D_POSIX_SYNC_OBJECT_H__*/
