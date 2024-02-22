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

    TResult Event::wait(uint32_t timeout)
    {
        if (mImpl == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mImpl->wait(timeout);
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

    TResult WaitCondition::wait(uint32_t timeout)
    {
        if (mImpl == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mImpl->wait(timeout);
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
