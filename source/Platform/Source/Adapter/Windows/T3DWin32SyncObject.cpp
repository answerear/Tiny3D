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
        TResult ret = T3D_OK;
        
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
        return tryLock(-1);
    }
    
    //--------------------------------------------------------------------------

    TResult Win32Mutex::tryLock(uint32_t timeout)
    {
        TResult ret = T3D_ERR_TIMEOUT;
        
        do
        {
            if (mMutex == nullptr)
            {
                ret = T3D_ERR_NATIVE_OBJECT_NOT_CREATED;
                break;
            }
            
            if (WaitForSingleObject(mMutex, timeout) == WAIT_OBJECT_0)
            {
                ret = T3D_OK;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Win32Mutex::unlock()
    {
        TResult ret = T3D_ERR_TIMEOUT;

        do
        {
            if (mMutex == nullptr)
            {
                ret = T3D_ERR_NATIVE_OBJECT_NOT_CREATED;
                break;
            }
            
            if (::ReleaseMutex(mMutex))
            {
                ret = T3D_OK;
            }
        } while (false);

        return ret;
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
        return tryLock(-1);
    }
    
    //--------------------------------------------------------------------------

    TResult Win32RecursiveMutex::tryLock(uint32_t timeout)
    {
        TResult ret = T3D_OK;

        do
        {
            if (mMutex == nullptr)
            {
                ret = T3D_ERR_NATIVE_OBJECT_NOT_CREATED;
                break;
            }
            
            DWORD threadId = GetCurrentThreadId();
        
            if (mOwner == threadId)
            {
                mCount++;
            }
            else
            {
                if (WaitForSingleObject(mMutex, timeout) == WAIT_OBJECT_0)
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
        } while (false);
        
        return ret;
    }
    
    //--------------------------------------------------------------------------

    TResult Win32RecursiveMutex::unlock()
    {
        TResult ret = T3D_OK;

        do
        {
            if (mMutex == nullptr)
            {
                ret = T3D_ERR_NATIVE_OBJECT_NOT_CREATED;
                break;
            }
            
            if (mOwner != GetCurrentThreadId())
            {
                ret = T3D_ERR_FAIL;
                break;
            }
        
            mCount--;
        
            if (mCount == 0)
            {
                mOwner = 0;
                ReleaseMutex(mMutex);
            }
        } while (false);
        
        return ret;
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
        return tryLock(-1);
    }
    
    //--------------------------------------------------------------------------

    TResult Win32Semaphore::tryLock(uint32_t timeout)
    {
        TResult ret = T3D_ERR_TIMEOUT;

        do
        {
            if (mSemaphore == nullptr)
            {
                ret = T3D_ERR_NATIVE_OBJECT_NOT_CREATED;
                break;
            }

            if (WaitForSingleObject(mSemaphore, timeout) == WAIT_OBJECT_0)
            {
                ret = T3D_OK;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Win32Semaphore::unlock()
    {
        TResult ret = T3D_ERR_FAIL;

        do
        {
            if (mSemaphore == nullptr)
            {
                ret = T3D_ERR_NATIVE_OBJECT_NOT_CREATED;
                break;
            }
            
            if (ReleaseSemaphore(mSemaphore, 1, nullptr))
            {
                ret = T3D_OK;
            } 
        } while (false);
        
        return ret;
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

    TResult Win32Event::wait(uint32_t timeout)
    {
        TResult ret = T3D_ERR_TIMEOUT;

        do
        {
            if (mEvent == nullptr)
            {
                ret = T3D_ERR_NATIVE_OBJECT_NOT_CREATED;
                break;
            }
            
            if (WaitForSingleObject(mEvent, timeout) == WAIT_OBJECT_0)
            {
                ret = T3D_OK;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Win32Event::trigger()
    {
        TResult ret = T3D_ERR_FAIL;

        do
        {
            if (mEvent == nullptr)
            {
                ret = T3D_ERR_NATIVE_OBJECT_NOT_CREATED;
                break;
            }
            
            if (::SetEvent(mEvent))
            {
                ret = T3D_OK;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Win32Event::reset()
    {
        TResult ret = T3D_ERR_FAIL;

        do
        {
            if (mEvent == nullptr)
            {
                ret = T3D_ERR_NATIVE_OBJECT_NOT_CREATED;
                break;
            }
            
            if (::ResetEvent(mEvent))
            {
                ret = T3D_OK;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    Win32WaitCondition::Win32WaitCondition()
    {
        InitializeCriticalSection(&mCS);
        InitializeConditionVariable(&mCV);
    }

    //--------------------------------------------------------------------------

    Win32WaitCondition::~Win32WaitCondition()
    {
        DeleteCriticalSection(&mCS);
    }

    //--------------------------------------------------------------------------

    TResult Win32WaitCondition::wait(uint32_t timeout)
    {
        TResult ret = T3D_ERR_TIMEOUT;

        do
        {
            EnterCriticalSection(&mCS);
            if (SleepConditionVariableCS(&mCV, &mCS, timeout))
            {
                ret = T3D_OK;
            }
            LeaveCriticalSection(&mCS);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Win32WaitCondition::wakeOne()
    {
        EnterCriticalSection(&mCS);
        WakeConditionVariable(&mCV);
        LeaveCriticalSection(&mCS);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Win32WaitCondition::wakeAll()
    {
        EnterCriticalSection(&mCS);
        WakeAllConditionVariable(&mCV);
        LeaveCriticalSection(&mCS);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
