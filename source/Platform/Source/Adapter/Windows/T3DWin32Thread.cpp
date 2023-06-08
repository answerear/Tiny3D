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

#include "T3DCommonErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Win32Thread::Win32Thread()
    {
        
    }

    //--------------------------------------------------------------------------

    Win32Thread::~Win32Thread()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult Win32Thread::start(ThreadProc proc, void *data, const String &name, uint32_t stackSize, ThreadPriority priority, uint64_t affinityMask)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Win32Thread::suspend()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Win32Thread::resume()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Win32Thread::terminate(bool wait)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Win32Thread::wait()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    ulong_t Win32Thread::getID() const
    {
        return 0;
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
        return 0;
    }

    //--------------------------------------------------------------------------

    void Win32Thread::setAffinityMask(uint64_t mask)
    {
        ::SetThreadAffinityMask(mThread, mask);
    }

    //--------------------------------------------------------------------------

    ThreadPriority Win32Thread::fromWin32Priority(int priority) const
    {
        ThreadPriority pri;

        return pri;
    }

    //--------------------------------------------------------------------------

    int Win32Thread::toWin32Priority(ThreadPriority priority) const
    {
        int pri;

        return pri;
    }

    //--------------------------------------------------------------------------

    ulong_t Win32ThreadSingleton::getCurrentThreadID()
    {
        return 0;
    }

    //--------------------------------------------------------------------------

    ulong_t Win32ThreadSingleton::getMainThreadID()
    {
        return 0;
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
