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

#include "Adapter/Windows/T3DWin32Thread.h"
#include "T3DPlatformErrorDef.h"
#include "Thread/T3DRunnable.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Win32Thread::Win32Thread()
    {
        
    }

    //--------------------------------------------------------------------------

    Win32Thread::~Win32Thread()
    {
        T3D_ASSERT(mThread == nullptr, "Thread must terminate before free object !");
    }

    //--------------------------------------------------------------------------

    TResult Win32Thread::start(Runnable *runnable, const String &name, uint32_t stackSize, ThreadPriority priority, uint64_t affinityMask)
    {
        TResult ret = T3D_OK;

        do
        {
            if (mThread != nullptr)
            {
                ret = T3D_ERR_THREAD_ALREADY_CREATED;
                break; 
            }

            mRunnable = runnable;
            mThread = ::CreateThread(
                nullptr,
                stackSize,
                [](void *lpParameter) -> DWORD
                {
                    TResult ret = T3D_ERR_THREAD_NOT_CREATED;
        
                    Runnable *runnable = static_cast<Runnable *>(lpParameter);

                    if (runnable->init())
                    {
                        ret = runnable->run();
                        runnable->exit();
                    }

                    return ret;
                },
                this,
                STACK_SIZE_PARAM_IS_A_RESERVATION | CREATE_SUSPENDED,
                &mThreadID);
            if (mThread == nullptr)
            {
                ret = T3D_ERR_THREAD_CREATED;
                mRunnable = nullptr;
                break;
            }
            
            setPriority(priority);
            setAffinityMask(affinityMask);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Win32Thread::suspend()
    {
        TResult ret = T3D_OK;

        do
        {
            if (mThread == nullptr)
            {
                ret = T3D_ERR_THREAD_NOT_CREATED;
                break;
            }
            
            if (::SuspendThread(mThread) == (DWORD)-1)
            {
                ret = T3D_ERR_THREAD_SUSPEND;
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Win32Thread::resume()
    {
        TResult ret = T3D_OK;

        do
        {
            if (mThread == nullptr)
            {
                ret = T3D_ERR_THREAD_NOT_CREATED;
                break;
            }

            if (::ResumeThread(mThread) == (DWORD)-1)
            {
                ret = T3D_ERR_THREAD_RESUME;
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Win32Thread::terminate(bool wait)
    {
        TResult ret = T3D_OK;

        do
        {
            if (mThread == nullptr)
            {
                ret = T3D_ERR_THREAD_NOT_CREATED;
                break;
            }

            if (mRunnable != nullptr)
            {
                mRunnable->stop();
            }

            if (wait)
            {
                ::WaitForSingleObject(mThread, INFINITE);
            }

            ::CloseHandle(mThread);
            mThread = nullptr;
        }
        while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Win32Thread::wait()
    {
        TResult ret = T3D_OK;

        do
        {
            if (mThread == nullptr)
            {
                ret = T3D_ERR_THREAD_NOT_CREATED;
                break;
            }

            WaitForSingleObject(mThread, INFINITE);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    ulong_t Win32Thread::getID() const
    {
        return mThreadID;
    }
    //--------------------------------------------------------------------------

    ThreadPriority Win32Thread::getPriority() const
    {
        return fromWin32Priority(::GetThreadPriority(mThread));
    }

    //--------------------------------------------------------------------------

    void Win32Thread::setPriority(ThreadPriority priority)
    {
        ::SetThreadPriority(mThread, toWin32Priority(priority));
    }

    //--------------------------------------------------------------------------

    uint64_t Win32Thread::getAffinityMask() const
    {
        return mAffinityMask;
    }

    //--------------------------------------------------------------------------

    void Win32Thread::setAffinityMask(uint64_t mask)
    {
        if (::SetThreadAffinityMask(mThread, mask) != 0)
        {
            mAffinityMask = mask;
        }
    }

    //--------------------------------------------------------------------------

    ThreadPriority Win32Thread::fromWin32Priority(int priority) const
    {
        ThreadPriority ret;

        switch (priority)
        {
        case THREAD_PRIORITY_IDLE:
            ret = ThreadPriority::kIdle;
            break;
        case THREAD_PRIORITY_LOWEST:
            ret = ThreadPriority::kLowest;
            break;
        case THREAD_PRIORITY_BELOW_NORMAL:
            ret = ThreadPriority::kLow;
            break;
        case THREAD_PRIORITY_NORMAL:
            ret = ThreadPriority::kNormal;
            break;
        case THREAD_PRIORITY_ABOVE_NORMAL:
            ret = ThreadPriority::kHigh;
            break;
        case THREAD_PRIORITY_HIGHEST:
            ret = ThreadPriority::kHighest;
            break;
        case THREAD_PRIORITY_TIME_CRITICAL:
            ret = ThreadPriority::kTimeCritical;
            break;
        default:
            ret = ThreadPriority::kNormal;
            break;
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    int Win32Thread::toWin32Priority(ThreadPriority priority) const
    {
        int ret = THREAD_PRIORITY_NORMAL;

        switch (priority)
        {
        case ThreadPriority::kIdle:
            ret = THREAD_PRIORITY_IDLE;
            break;
        case ThreadPriority::kLowest:
            ret = THREAD_PRIORITY_LOWEST;
            break;
        case ThreadPriority::kLow:
            ret = THREAD_PRIORITY_BELOW_NORMAL;
            break;
        case ThreadPriority::kNormal:
            ret = THREAD_PRIORITY_NORMAL;
            break;
        case ThreadPriority::kHigh:
            ret = THREAD_PRIORITY_ABOVE_NORMAL;
            break;
        case ThreadPriority::kHighest:
            ret = THREAD_PRIORITY_HIGHEST;
            break;
        case ThreadPriority::kTimeCritical:
            ret = THREAD_PRIORITY_TIME_CRITICAL;
            break;
        case ThreadPriority::kInherit:
        default:
            ret = THREAD_PRIORITY_NORMAL;
            break;
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    ulong_t Win32ThreadSingleton::getCurrentThreadID()
    {
        return ::GetCurrentThreadId();
    }

    //--------------------------------------------------------------------------

    ulong_t Win32ThreadSingleton::getMainThreadID()
    {
        static ulong_t threadID = ::GetCurrentThreadId();
        return threadID;
    }

    //--------------------------------------------------------------------------

    void Win32ThreadSingleton::sleepCurrentThread(uint32_t msec)
    {
        ::Sleep(msec);
    }
    
    //--------------------------------------------------------------------------
    
    uint32_t Win32ThreadSingleton::getThreadHardwareConcurrency()
    {
        return 0;
    }

    //--------------------------------------------------------------------------
}
