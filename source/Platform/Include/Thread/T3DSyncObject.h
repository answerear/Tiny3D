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
