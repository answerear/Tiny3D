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

#ifndef __T3D_WIN32_SYNC_OBJECT_H__
#define __T3D_WIN32_SYNC_OBJECT_H__


#include "T3DNoncopyable.h"
#include "Adapter/T3DSyncObjectinterface.h"
#include <windows.h>


namespace Tiny3D
{
    //--------------------------------------------------------------------------
    
    class Win32CriticalSection : public ICriticalSection, public Noncopyable
    {
    public:
        Win32CriticalSection();

        ~Win32CriticalSection() override;

        TResult lock() override;
        
        TResult tryLock(uint32_t timeout) override;

        TResult unlock() override;

    protected:
        bool try_lock();
        
        CRITICAL_SECTION    mCS;
    };

    //--------------------------------------------------------------------------
    
    class Win32Mutex : public IMutex, public Noncopyable
    {
    public:
        Win32Mutex();

        ~Win32Mutex() override;

        TResult lock() override;
        
        TResult tryLock(uint32_t timeout) override;

        TResult unlock() override;

    protected:
        HANDLE  mMutex = nullptr;
    };

    //--------------------------------------------------------------------------
    
    class Win32RecursiveMutex : public IRecursiveMutex, public Noncopyable
    {
    public:
        Win32RecursiveMutex();

        ~Win32RecursiveMutex() override;

        TResult lock() override;

        TResult tryLock(uint32_t timeout) override;

        TResult unlock() override;

    protected:
        HANDLE  mMutex = nullptr;
        DWORD   mOwner = 0;
        int32_t mCount = 0;
    };

    //--------------------------------------------------------------------------
    
    class Win32Semaphore : public ISemaphore, public Noncopyable
    {
    public:
        Win32Semaphore();

        ~Win32Semaphore() override;

        TResult lock() override;

        TResult tryLock(uint32_t timeout) override;

        TResult unlock() override;

    protected:
        HANDLE  mSemaphore = nullptr;
    };

    //--------------------------------------------------------------------------
    
    class Win32Event : public IEvent, public Noncopyable
    {
    public:
        Win32Event();

        ~Win32Event() override;

        TResult wait(ICriticalSection *cs, uint32_t timeout) override;

        TResult trigger() override;

        TResult reset() override;
    };

    //--------------------------------------------------------------------------
    
    class Win32WaitCondition : public IWaitCondition, public Noncopyable
    {
    public:
        Win32WaitCondition();

        ~Win32WaitCondition() override;

        TResult wait(ICriticalSection *cs, uint32_t timeout) override;

        TResult wakeOne() override;

        TResult wakeAll() override;
    };
}

#endif  /*__T3D_WIN32_SYNC_OBJECT_H__*/
