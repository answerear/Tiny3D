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


#include "Adapter/Windows/T3DWin32SyncObject.h"
#include "T3DPlatformErrorDef.h"


namespace  Tiny3D
{
    //--------------------------------------------------------------------------

    Win32CriticalSection::Win32CriticalSection()
    {
        ::InitializeCriticalSection(&mCS);
    }

    //--------------------------------------------------------------------------

    Win32CriticalSection::~Win32CriticalSection()
    {
        ::DeleteCriticalSection(&mCS);
    }

    //--------------------------------------------------------------------------

    TResult Win32CriticalSection::lock()
    {
        ::EnterCriticalSection(&mCS);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    bool Win32CriticalSection::try_lock()
    {
        return ::TryEnterCriticalSection(&mCS) != 0;
    }

    //--------------------------------------------------------------------------

    TResult Win32CriticalSection::tryLock(uint32_t timeout)
    {
        TResult ret = T3D_ERR_TIMEOUT;
        
        if (timeout == (uint32_t)-1)
        {
            if (!try_lock())
            {
                ret = T3D_ERR_TIMEOUT;
            }
        }
        else
        {
            DWORD start = GetTickCount();
            
            while (!try_lock())
            {
                DWORD elapsed = GetTickCount() - start;
                
                if (elapsed >= timeout)
                {
                    ret = T3D_ERR_TIMEOUT;
                    break;
                }

                ::Sleep(1);
            }
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Win32CriticalSection::unlock()
    {
        ::LeaveCriticalSection(&mCS);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    Win32Mutex::Win32Mutex()
    {
        mMutex = ::CreateMutex(nullptr, false, nullptr);
    }

    //--------------------------------------------------------------------------

    Win32Mutex::~Win32Mutex()
    {
        if (mMutex != nullptr)
        {
            ::CloseHandle(mMutex);
            mMutex = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    TResult Win32Mutex::lock()
    {
        ::WaitForSingleObject(mMutex, INFINITE);
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    TResult Win32Mutex::tryLock(uint32_t timeout)
    {
        TResult ret = T3D_ERR_TIMEOUT;

        DWORD t = timeout;
        
        if (timeout == (uint32_t)-1)
        {
            t = INFINITE;
        }
        
        if (WaitForSingleObject(mMutex, t) == WAIT_OBJECT_0)
        {
            ret = T3D_OK;
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Win32Mutex::unlock()
    {
        ::ReleaseMutex(mMutex);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    Win32RecursiveMutex::Win32RecursiveMutex()
    {
        mMutex = ::CreateMutex(nullptr, false, nullptr);
    }
    
    //--------------------------------------------------------------------------

    Win32RecursiveMutex::~Win32RecursiveMutex()
    {
        if (mMutex != nullptr)
        {
            ::CloseHandle(mMutex);
            mMutex = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    TResult Win32RecursiveMutex::lock()
    {
        DWORD threadId = GetCurrentThreadId();
        if (mOwner == threadId)
        {
            mCount++;
        }
        else
        {
            WaitForSingleObject(mMutex, INFINITE);
            mOwner = threadId;
            mCount = 1;
        }
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    TResult Win32RecursiveMutex::tryLock(uint32_t timeout)
    {
        TResult ret = T3D_OK;
        DWORD threadId = GetCurrentThreadId();
        
        if (mOwner == threadId)
        {
            mCount++;
        }
        else
        {
            DWORD t = timeout;
            if (timeout == (uint32_t)-1)
            {
                t = INFINITE;
            }

            if (WaitForSingleObject(mMutex, t) == WAIT_OBJECT_0)
            {
                mOwner = threadId;
                mCount = 1;
                ret = T3D_OK;
            }
            else
            {
                ret = T3D_ERR_TIMEOUT;
            }
        }
        
        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult Win32RecursiveMutex::unlock()
    {
        if (mOwner != GetCurrentThreadId())
        {
            return T3D_ERR_FAIL;
        }
        
        mCount--;
        
        if (mCount == 0)
        {
            mOwner = 0;
            ReleaseMutex(mMutex);
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    Win32Semaphore::Win32Semaphore()
    {
        mSemaphore = ::CreateSemaphore(nullptr, 1, MAXLONG, nullptr);
    }
    
    //--------------------------------------------------------------------------

    Win32Semaphore::~Win32Semaphore()
    {
        if (mSemaphore != nullptr)
        {
            ::CloseHandle(mSemaphore);
        }
    }

    //--------------------------------------------------------------------------

    TResult Win32Semaphore::lock()
    {
        ::WaitForSingleObject(mSemaphore, INFINITE);
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    TResult Win32Semaphore::tryLock(uint32_t timeout)
    {
        TResult ret = T3D_ERR_TIMEOUT;

        DWORD t = timeout;
        if (timeout == (uint32_t)-1)
        {
            t = INFINITE;
        }

        if (WaitForSingleObject(mSemaphore, t) == WAIT_OBJECT_0)
        {
            ret = T3D_OK;
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Win32Semaphore::unlock()
    {
        ReleaseSemaphore(mSemaphore, 1, nullptr);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    Win32Event::Win32Event()
    {
        mEvent = CreateEvent(nullptr, false, false, nullptr);
    }

    //--------------------------------------------------------------------------

    Win32Event::~Win32Event()
    {
        if (mEvent != nullptr)
        {
            ::CloseHandle(mEvent);
            mEvent = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    TResult Win32Event::wait(ICriticalSection *cs, uint32_t timeout)
    {
        TResult ret = T3D_ERR_TIMEOUT;

        DWORD t = timeout;
        if (timeout == (uint32_t)-1)
        {
            t = INFINITE;
        }

        if (WaitForSingleObject(mEvent, t) == WAIT_OBJECT_0)
        {
            ret = T3D_OK;
        }
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Win32Event::trigger()
    {
        ::SetEvent(mEvent);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Win32Event::reset()
    {
        ::ResetEvent(mEvent);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    Win32WaitCondition::Win32WaitCondition()
    {
        
    }

    //--------------------------------------------------------------------------

    Win32WaitCondition::~Win32WaitCondition()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult Win32WaitCondition::wait(ICriticalSection *cs, uint32_t timeout)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Win32WaitCondition::wakeOne()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Win32WaitCondition::wakeAll()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
