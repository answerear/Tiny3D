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


#include "T3DSyncObject.h"

#include "Adapter/T3DFactoryInterface.h"
#include "Adapter/T3DSyncObjectInterface.h"
#include "T3DPlatform.h"
#include "T3DPlatformErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    CriticalSection::CriticalSection()
    {
        mImpl = T3D_PLATFORM_FACTORY.createPlatformCriticalSection();
    }

    //--------------------------------------------------------------------------

    CriticalSection::CriticalSection(CriticalSection &&other) noexcept
    {
        if (mImpl != other.mImpl)
        {
            mImpl = other.mImpl;
            other.mImpl = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    CriticalSection::~CriticalSection()
    {
        T3D_SAFE_DELETE(mImpl);
    }

    //--------------------------------------------------------------------------

    CriticalSection &CriticalSection::operator=(CriticalSection &&other) noexcept
    {
        if (mImpl != other.mImpl)
        {
            mImpl = other.mImpl;
            other.mImpl = nullptr;
        }
        
        return *this;
    }

    //--------------------------------------------------------------------------

    TResult CriticalSection::lock()
    {
        if (mImpl == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mImpl->lock();
    }

    //--------------------------------------------------------------------------

    TResult CriticalSection::tryLock(uint32_t timeout)
    {
        if (mImpl == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mImpl->tryLock(timeout);
    }
    
    //--------------------------------------------------------------------------

    TResult CriticalSection::unlock()
    {
        if (mImpl == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mImpl->unlock();
    }
    
    //--------------------------------------------------------------------------

    Mutex::Mutex()
    {
        mImpl = T3D_PLATFORM_FACTORY.createPlatformMutex();
    }

    //--------------------------------------------------------------------------

    Mutex::Mutex(Mutex &&other) noexcept
    {
        if (mImpl != other.mImpl)
        {
            mImpl = other.mImpl;
            other.mImpl = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    Mutex::~Mutex()
    {
        T3D_SAFE_DELETE(mImpl);
    }

    //--------------------------------------------------------------------------

    Mutex &Mutex::operator=(Mutex &&other) noexcept
    {
        if (mImpl != other.mImpl)
        {
            mImpl = other.mImpl;
            other.mImpl = nullptr;
        }
        
        return *this;
    }

    //--------------------------------------------------------------------------

    TResult Mutex::lock()
    {
        if (mImpl == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mImpl->lock();
    }

    //--------------------------------------------------------------------------

    TResult Mutex::tryLock(uint32_t timeout)
    {
        if (mImpl == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mImpl->tryLock(timeout);
    }
    
    //--------------------------------------------------------------------------

    TResult Mutex::unlock()
    {
        if (mImpl == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mImpl->unlock();
    }
    
    //--------------------------------------------------------------------------

    RecursiveMutex::RecursiveMutex()
    {
        mImpl = T3D_PLATFORM_FACTORY.createPlatformRecursiveMutex();
    }

    //--------------------------------------------------------------------------

    RecursiveMutex::RecursiveMutex(RecursiveMutex &&other) noexcept
    {
        if (mImpl != other.mImpl)
        {
            mImpl = other.mImpl;
            other.mImpl = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    RecursiveMutex::~RecursiveMutex()
    {
        T3D_SAFE_DELETE(mImpl);
    }

    //--------------------------------------------------------------------------

    RecursiveMutex &RecursiveMutex::operator=(RecursiveMutex &&other) noexcept
    {
        if (mImpl != other.mImpl)
        {
            mImpl = other.mImpl;
            other.mImpl = nullptr;
        }
        
        return *this;
    }

    //--------------------------------------------------------------------------

    TResult RecursiveMutex::lock()
    {
        if (mImpl == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mImpl->lock();
    }

    //--------------------------------------------------------------------------

    TResult RecursiveMutex::tryLock(uint32_t timeout)
    {
        if (mImpl == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mImpl->tryLock(timeout);
    }
    
    //--------------------------------------------------------------------------

    TResult RecursiveMutex::unlock()
    {
        if (mImpl == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mImpl->unlock();
    }
    
    //--------------------------------------------------------------------------

    Semaphore::Semaphore()
    {
        mImpl = T3D_PLATFORM_FACTORY.createPlatformSemaphore();
    }

    //--------------------------------------------------------------------------

    Semaphore::Semaphore(Semaphore &&other) noexcept
    {
        if (mImpl != other.mImpl)
        {
            mImpl = other.mImpl;
            other.mImpl = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    Semaphore::~Semaphore()
    {
        T3D_SAFE_DELETE(mImpl);
    }

    //--------------------------------------------------------------------------

    Semaphore &Semaphore::operator=(Semaphore &&other) noexcept
    {
        if (mImpl != other.mImpl)
        {
            mImpl = other.mImpl;
            other.mImpl = nullptr;
        }
        
        return *this;
    }

    //--------------------------------------------------------------------------

    TResult Semaphore::lock()
    {
        if (mImpl == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mImpl->lock();
    }

    //--------------------------------------------------------------------------

    TResult Semaphore::tryLock(uint32_t timeout)
    {
        if (mImpl == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mImpl->tryLock(timeout);
    }
    
    //--------------------------------------------------------------------------

    TResult Semaphore::unlock()
    {
        if (mImpl == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mImpl->unlock();
    }
    
    //--------------------------------------------------------------------------

    Event::Event()
    {
        mImpl = T3D_PLATFORM_FACTORY.createPlatformEvent();
    }

    //--------------------------------------------------------------------------

    Event::Event(Event &&other) noexcept
    {
        if (mImpl != other.mImpl)
        {
            mImpl = other.mImpl;
            other.mImpl = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    Event::~Event()
    {
        T3D_SAFE_DELETE(mImpl);
    }

    //--------------------------------------------------------------------------

    Event &Event::operator=(Event &&other) noexcept
    {
        if (mImpl != other.mImpl)
        {
            mImpl = other.mImpl;
            other.mImpl = nullptr;
        }

        return *this;
    }

    //--------------------------------------------------------------------------

    TResult Event::wait(CriticalSection &cs, uint32_t timeout)
    {
        if (mImpl == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mImpl->wait(cs.GetNativeHandle(), timeout);
    }

    //--------------------------------------------------------------------------

    TResult Event::trigger()
    {
        if (mImpl == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mImpl->trigger();
    }

    //--------------------------------------------------------------------------

    TResult Event::reset()
    {
        if (mImpl == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mImpl->reset();
    }
    
    //--------------------------------------------------------------------------

    WaitCondition::WaitCondition()
    {
        mImpl = T3D_PLATFORM_FACTORY.createPlatformWaitCondition();
    }

    //--------------------------------------------------------------------------

    WaitCondition::WaitCondition(WaitCondition &&other) noexcept
    {
        if (mImpl != other.mImpl)
        {
            mImpl = other.mImpl;
            other.mImpl = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    WaitCondition::~WaitCondition()
    {
        T3D_SAFE_DELETE(mImpl);
    }

    //--------------------------------------------------------------------------

    WaitCondition &WaitCondition::operator=(WaitCondition &&other) noexcept
    {
        if (mImpl != other.mImpl)
        {
            mImpl = other.mImpl;
            other.mImpl = nullptr;
        }

        return *this;
    }

    //--------------------------------------------------------------------------

    TResult WaitCondition::wait(CriticalSection &cs, uint32_t timeout)
    {
        if (mImpl == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mImpl->wait(cs.GetNativeHandle(), timeout);
    }

    //--------------------------------------------------------------------------

    TResult WaitCondition::wakeOne()
    {
        if (mImpl == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mImpl->wakeOne();
    }

    //--------------------------------------------------------------------------

    TResult WaitCondition::wakeAll()
    {
        if (mImpl == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mImpl->wakeAll();
    }
    
    //--------------------------------------------------------------------------
}
