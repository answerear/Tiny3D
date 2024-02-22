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

#ifndef __T3D_THREAD_MANAGER_H__
#define __T3D_THREAD_MANAGER_H__


#include "T3DType.h"
#include "T3DMacro.h"
#include "T3DPlatformPrerequisites.h"
#include "T3DRunnableThread.h"
#include "T3DSingleton.h"
#include "Thread/T3DSyncObject.h"


namespace Tiny3D
{
    class RunnableThread;
    
    class T3D_PLATFORM_API ThreadManager : public Singleton<ThreadManager>
    {
    public:
        ThreadManager() = default;

        ~ThreadManager() = default;

        void addThread(ulong_t threadID, RunnableThread *thread);

        void removeThread(RunnableThread *thread);

        size_t numOfThreads() const { return mThreads.size(); }

        void update();

        void foreachThread(TFunction<void(ulong_t,RunnableThread*)> func);

    protected:
        using Threads = TMap<ulong_t, RunnableThread*>;
        using ThreadsItr = Threads::iterator;
        using ThreadsConstItr = Threads::const_iterator;
        using ThreadsValue = Threads::value_type;

        Threads         mThreads;
        CriticalSection mThreadsCS;
    };

    #define T3D_THREAD_MGR  (ThreadManager::getInstance())
}

#endif  /*__T3D_THREAD_MANAGER_H__*/
