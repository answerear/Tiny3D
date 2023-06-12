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


#include "Thread/T3DThreadManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    void ThreadManager::addThread(ulong_t threadID, RunnableThread *thread)
    {
        T3D_ASSERT(thread != nullptr, "Thread added in ThreadManager must not be nullptr !");

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
