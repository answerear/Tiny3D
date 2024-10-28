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

#ifndef __T3D_THREAD_INTERFACE_H__
#define __T3D_THREAD_INTERFACE_H__


#include "T3DMacro.h"
#include "T3DPlatformPrerequisites.h"
#include "T3DType.h"
#include "Thread/T3DThreadConstant.h"
#include "Memory/T3DMemory.h"


namespace Tiny3D
{
    class Runnable;
    class RunnableThread;

    using ThreadRoutine = TFunction<TResult(void*)>;
    
    class IThread : public Allocator
    {
        T3D_DECLARE_INTERFACE(IThread);

    public:
        virtual TResult start(ThreadRoutine routine, void *parameter, uint32_t stackSize) = 0;

        virtual TResult suspend() = 0;

        virtual TResult resume() = 0;

        virtual TResult terminate(bool wait, Runnable *runnable) = 0;

        virtual TResult wait() = 0;

        virtual ulong_t getID() const = 0;

        virtual ThreadPriority getPriority() const = 0;

        virtual void setPriority(ThreadPriority priority) = 0;

        virtual uint64_t getAffinityMask() const = 0;

        virtual void setAffinityMask(uint64_t mask) = 0;
    };
}

#endif  /*__T3D_THREAD_INTERFACE_H__*/
