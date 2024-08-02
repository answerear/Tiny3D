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


#include "Thread/T3DThreadManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    void ThreadManager::addThread(ulong_t threadID, RunnableThread *thread)
    {
        T3D_ASSERT(thread != nullptr);

        ScopeLock lock(&mThreadsCS);

        if (mThreads.find(threadID) == mThreads.end())
        {
            mThreads.insert(ThreadsValue(threadID, thread));
        }
    }

    //--------------------------------------------------------------------------

    void ThreadManager::removeThread(RunnableThread *thread)
    {
        ScopeLock lock(&mThreadsCS);
        mThreads.erase(thread->getID());
    }

    //--------------------------------------------------------------------------

    void ThreadManager::update()
    {
        foreachThread([](ulong_t threadID, RunnableThread *thread)
        {
            
        });
    }

    //--------------------------------------------------------------------------

    void ThreadManager::foreachThread(TFunction<void(ulong_t, RunnableThread *)> func)
    {
        ScopeLock lock(&mThreadsCS);

        Threads threads = mThreads;

        for (const auto &val : threads)
        {
            func(val.first, val.second);
        }
    }

    //--------------------------------------------------------------------------
}
