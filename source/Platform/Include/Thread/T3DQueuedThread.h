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

#ifndef __T3D_QUEUED_THREAD_H__
#define __T3D_QUEUED_THREAD_H__


#include "Thread/T3DRunnable.h"
#include "Thread/T3DSyncObject.h"
#include "Thread/T3DThreadConstant.h"
#include "Memory/T3DMemory.h"


namespace Tiny3D
{
    class IQueuedJob;
    class QueuedJobPoolDefault;
    class RunnableThread;
    
    class T3D_PLATFORM_API QueuedThread
        : public Runnable
        , public Noncopyable
    {
    public:
        QueuedThread() = default;
        ~QueuedThread() override = default;
        
        virtual TResult create(QueuedJobPoolDefault *pool, uint32_t stackSize = 0, ThreadPriority priority = ThreadPriority::kNormal, const String &name = "");

        TResult killThread();

        void executeJob(IQueuedJob *job);
        
    protected:
        TResult run() override;
        
        Event                   mJobEvent;
        TAtomic<bool>           mTimeToDie { false };
        IQueuedJob* volatile    mQueuedJob = nullptr;
        QueuedJobPoolDefault    *mOwner = nullptr;
        RunnableThread          *mThread = nullptr;
    };
}


#endif  /*__T3D_QUEUED_THREAD_H__*/
