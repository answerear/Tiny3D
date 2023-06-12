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
    
    class T3D_PLATFORM_API ThreadManager
        : public Singleton<ThreadManager>
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
