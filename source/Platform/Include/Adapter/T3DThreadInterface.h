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

#ifndef __T3D_THREAD_INTERFACE_H__
#define __T3D_THREAD_INTERFACE_H__


#include "T3DMacro.h"
#include "T3DPlatformPrerequisites.h"
#include "T3DType.h"
#include "Thread/T3DThreadConstant.h"

namespace Tiny3D
{
    class Runnable;
    class RunnableThread;

    using ThreadRoutine = TFunction<TResult(void*)>;
    
    class IThread
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

    class IThreadSingleton
    {
        T3D_DECLARE_INTERFACE(IThreadSingleton);
        
    public:
        virtual ulong_t getCurrentThreadID() = 0;

        virtual ulong_t getMainThreadID() = 0;

        virtual void sleepCurrentThread(uint32_t msec) = 0;

        virtual uint32_t getThreadHardwareConcurrency() = 0;
    };
}

#endif  /*__T3D_THREAD_INTERFACE_H__*/
