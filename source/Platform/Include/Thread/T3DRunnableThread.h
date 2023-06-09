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

#ifndef __T3D_RUNNABLE_THREAD_H__
#define __T3D_RUNNABLE_THREAD_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DNoncopyable.h"
#include "T3DType.h"
#include "Thread/T3DThreadConstant.h"
#include "T3DSingleton.h"

namespace Tiny3D
{
    class IThread;
    class IThreadSingleton;
    class Runnable;
    
    class ThreadSingleton : public Singleton<ThreadSingleton>
    {
    public:
        ThreadSingleton();

        ~ThreadSingleton();
        
        IThreadSingleton *getThreadSingleton() { return mThreadSingleton; }

    protected:
        IThreadSingleton *mThreadSingleton = nullptr;
    };
    
    class T3D_PLATFORM_API RunnableThread : public Noncopyable
    {
    public:
        RunnableThread();

        virtual ~RunnableThread() override;

        TResult start(Runnable *runnable, const String &name, uint32_t stackSize = 0, ThreadPriority priority = ThreadPriority::kNormal, uint64_t affinityMask = kNoAffinityMask);

        TResult suspend();

        TResult resume();

        TResult terminate(bool wait);

        TResult wait();

        ulong_t getID() const;

        ThreadPriority getPriority() const;

        void setPriority(ThreadPriority priority);

        uint64_t getAffinityMask() const;

        void setAffinityMask(uint64_t mask);

        const String getName() const { return mName; }

        static ulong_t getCurrentThreadID();

        static ulong_t getMainThreadID();

        static void sleepCurrentThread(uint32_t msec);

        static uint32_t getThreadHardwareConcurrency();
        
    protected:
        String      mName;
        Runnable    *mRunnable = nullptr;
        IThread     *mThread = nullptr;
    };
}

#endif  /*__T3D_RUNNABLE_THREAD_H__*/
