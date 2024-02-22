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

#ifndef __T3D_SYNC_OBJECT_H__
#define __T3D_SYNC_OBJECT_H__


#include "T3DCommonErrorDef.h"
#include "T3DType.h"
#include "T3DMacro.h"
#include "T3DNoncopyable.h"
#include "T3DPlatformPrerequisites.h"


namespace Tiny3D
{
    class ICriticalSection;
    class IMutex;
    class IRecursiveMutex;
    class ISemaphore;
    class IEvent;
    class IWaitCondition;

    //--------------------------------------------------------------------------
    
    class ISyncObject
    {
    public:
        virtual TResult lock() = 0;
        virtual TResult tryLock(uint32_t timeout) = 0;
        virtual TResult unlock() = 0;
    };

    //--------------------------------------------------------------------------
    
    class T3D_PLATFORM_API CriticalSection : public ISyncObject
    {
    public:
        CriticalSection();

        CriticalSection(CriticalSection &&other) noexcept;

        CriticalSection &operator=(CriticalSection &&other) noexcept;

        CriticalSection(const CriticalSection &other) = delete;
        CriticalSection &operator=(const CriticalSection &other) = delete;
        
        virtual ~CriticalSection();

        TResult lock() override;

        TResult tryLock(uint32_t timeout) override;

        TResult unlock() override;

    protected:
        ICriticalSection *mImpl = nullptr;
    };

    //--------------------------------------------------------------------------
    
    class T3D_PLATFORM_API Mutex : public ISyncObject
    {
    public:
        Mutex();

        Mutex(Mutex &&other) noexcept;

        Mutex &operator=(Mutex &&other) noexcept;

        Mutex(const Mutex &other) = delete;
        Mutex &operator=(const Mutex &other) = delete;
        
        virtual ~Mutex();

        TResult lock() override;

        TResult tryLock(uint32_t timeout) override;

        TResult unlock() override;

    protected:
        IMutex *mImpl = nullptr;
    };

    //--------------------------------------------------------------------------
    
    class T3D_PLATFORM_API RecursiveMutex : public ISyncObject
    {
    public:
        RecursiveMutex();

        RecursiveMutex(RecursiveMutex &&other) noexcept;

        RecursiveMutex &operator=(RecursiveMutex &&other) noexcept;

        RecursiveMutex(const RecursiveMutex &other) = delete;
        RecursiveMutex &operator=(const RecursiveMutex &other) = delete;
        
        virtual ~RecursiveMutex();

        TResult lock() override;

        TResult tryLock(uint32_t timeout) override;

        TResult unlock() override;

    protected:
        IRecursiveMutex *mImpl = nullptr;
    };

    //--------------------------------------------------------------------------
    
    class T3D_PLATFORM_API Semaphore : public ISyncObject
    {
    public:
        Semaphore();

        Semaphore(Semaphore &&other) noexcept;

        Semaphore &operator=(Semaphore &&other) noexcept;

        Semaphore(const Semaphore &other) = delete;
        Semaphore &operator=(const Semaphore &other) = delete;
        
        virtual ~Semaphore();

        TResult lock() override;

        TResult tryLock(uint32_t timeout) override;

        TResult unlock() override;

    protected:
        ISemaphore *mImpl = nullptr;
    };

    //--------------------------------------------------------------------------
    
    class T3D_PLATFORM_API Event
    {
    public:
        Event();

        Event(Event &&other) noexcept;

        Event &operator=(Event &&other) noexcept;

        Event(const Event &other) = delete;
        Event &operator=(const Event &other) = delete;
        
        virtual ~Event();

        TResult wait(uint32_t timeout = -1);

        TResult trigger();

        TResult reset();

    protected:
        IEvent *mImpl = nullptr;
    };

    //--------------------------------------------------------------------------
    
    class T3D_PLATFORM_API WaitCondition
    {
    public:
        WaitCondition();

        WaitCondition(WaitCondition &&other) noexcept;

        WaitCondition &operator=(WaitCondition &&other) noexcept;

        WaitCondition(const WaitCondition &other) = delete;
        WaitCondition &operator=(const WaitCondition &other) = delete;
        
        virtual ~WaitCondition();

        TResult wait(uint32_t timeout = -1);

        TResult wakeOne();

        TResult wakeAll();

    protected:
        IWaitCondition *mImpl = nullptr;
    };

    //--------------------------------------------------------------------------
    
    class T3D_PLATFORM_API ScopeLock : public Noncopyable
    {
    public:
        ScopeLock(ISyncObject *syncObj)
            : mSyncObject(syncObj)
        {
            T3D_ASSERT(mSyncObject != nullptr, "Sync object must not be nullptr !");
            mSyncObject->lock();
        }
        
        ~ScopeLock()
        {
            unlock();
        }

        TResult unlock()
        {
            TResult ret = T3D_OK;
            if (mSyncObject != nullptr)
            {
                ret = mSyncObject->unlock();
                if (ret == T3D_OK)
                {
                    mSyncObject = nullptr;
                }
            }
            return ret;
        }
        
    private:
        ISyncObject *mSyncObject = nullptr;
    };

    //--------------------------------------------------------------------------
}

#endif  /*__T3D_SYNC_OBJECT_H__*/
