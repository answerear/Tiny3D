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

#ifndef __T3D_WIN32_THREAD_H__
#define __T3D_WIN32_THREAD_H__


#include "T3DNoncopyable.h"
#include "Adapter/T3DThreadInterface.h"
#include "Memory/T3DMemory.h"
#include <windows.h>


namespace Tiny3D
{
    class Win32Thread : public IThread, public Noncopyable
    {
    public:
        Win32Thread();

        virtual ~Win32Thread() override;

        TResult start(ThreadRoutine routine, void *parameter, uint32_t stackSize) override;

        TResult suspend() override;

        TResult resume() override;
    
        TResult terminate(bool wait, Runnable *runnable) override;

        TResult wait() override;

        ulong_t getID() const override;

        ThreadPriority getPriority() const override;

        void setPriority(ThreadPriority priority) override;

        uint64_t getAffinityMask() const override;

        void setAffinityMask(uint64_t mask) override;
        
    protected:
        ThreadPriority fromWin32Priority(int priority) const;

        int toWin32Priority(ThreadPriority priority) const;
        
        HANDLE          mThread = nullptr;
        uint64_t        mAffinityMask = 0;
        DWORD           mThreadID = 0;
        ThreadRoutine   mThreadRoutine = nullptr;
        void            *mParameter = nullptr;
    };
}

#endif  /*__T3D_WIN32_THREAD_H__*/
