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

        TResult wait(uint32_t timeout) override;

        TResult trigger() override;

        TResult reset() override;

    protected:
        HANDLE  mEvent = nullptr;
    };

    //--------------------------------------------------------------------------
    
    class Win32WaitCondition : public IWaitCondition, public Noncopyable
    {
    public:
        Win32WaitCondition();

        ~Win32WaitCondition() override;

        TResult wait(uint32_t timeout) override;

        TResult wakeOne() override;

        TResult wakeAll() override;

    protected:
        CONDITION_VARIABLE  mCV {nullptr };
        CRITICAL_SECTION    mCS;
    };
}

#endif  /*__T3D_WIN32_SYNC_OBJECT_H__*/
