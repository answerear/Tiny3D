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

#include "Thread/T3DRunnableThread.h"
#include "Thread/T3DRunnable.h"
#include "Adapter/T3DFactoryInterface.h"
#include "Adapter/T3DThreadInterface.h"
#include "T3DPlatform.h"
#include "T3DPlatformErrorDef.h"

namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ThreadSingleton::ThreadSingleton()
    {
        mThreadSingleton = T3D_PLATFORM_FACTORY.createPlatformThreadSingleton();
    }

    //--------------------------------------------------------------------------

    ThreadSingleton::~ThreadSingleton()
    {
        T3D_SAFE_DELETE(mThreadSingleton);
    }
    
    //--------------------------------------------------------------------------

    RunnableThread::RunnableThread()
    {
        mThread = T3D_PLATFORM_FACTORY.createPlatformThread(); 
    }

    //--------------------------------------------------------------------------

    RunnableThread::~RunnableThread()
    {
        T3D_SAFE_DELETE(mThread);
    }

    //--------------------------------------------------------------------------

    TResult RunnableThread::start(Runnable *runnable, const String &name, uint32_t stackSize, ThreadPriority priority, uint64_t affinityMask)
    {
        TResult ret = T3D_OK;

        do
        {
            if (mThread == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                break;
            }

            if (runnable == nullptr)
            {
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }

            // 启动线程，不过线程默认挂起状态
            ret = mThread->start(runnable, stackSize);
            if (T3D_FAILED(ret))
            {
                break;
            }

            mRunnable = runnable;
            mName = name;

            // 设置线程优先级
            mThread->setPriority(priority);
            // 设置线程亲和性
            mThread->setAffinityMask(affinityMask);
            // 因线程启动时候挂起，现在开始执行
            mThread->resume();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult RunnableThread::suspend()
    {
        if (mThread == nullptr)
        {
            return T3D_ERR_INVALID_POINTER;
        }

        return mThread->suspend();
    }

    //--------------------------------------------------------------------------

    TResult RunnableThread::resume()
    {
        if (mThread == nullptr)
        {
            return T3D_ERR_INVALID_POINTER;
        }

        return mThread->resume();
    }

    //--------------------------------------------------------------------------

    TResult RunnableThread::terminate(bool wait)
    {
        if (mThread == nullptr)
        {
            return T3D_ERR_INVALID_POINTER;
        }

        return mThread->terminate(wait);
    }

    //--------------------------------------------------------------------------

    TResult RunnableThread::wait()
    {
        if (mThread == nullptr)
        {
            return T3D_ERR_INVALID_POINTER;
        }

        return mThread->wait();
    }

    //--------------------------------------------------------------------------

    ulong_t RunnableThread::getID() const
    {
        if (mThread == nullptr)
        {
            return -1;
        }

        return mThread->getID();
    }

    //--------------------------------------------------------------------------

    ThreadPriority RunnableThread::getPriority() const
    {
        if (mThread == nullptr)
        {
            return ThreadPriority::kMax;
        }

        return mThread->getPriority();
    }
    
    //--------------------------------------------------------------------------

    void RunnableThread::setPriority(ThreadPriority priority)
    {
        if (mThread != nullptr)
        {
            mThread->setPriority(priority);
        }
    }
    
    //--------------------------------------------------------------------------

    uint64_t RunnableThread::getAffinityMask() const
    {
        if (mThread == nullptr)
        {
            return 0;
        }

        return mThread->getAffinityMask();
    }
    
    //--------------------------------------------------------------------------

    void RunnableThread::setAffinityMask(uint64_t mask)
    {
        if (mThread != nullptr)
        {
            mThread->setAffinityMask(mask);
        }
    }
    
    //--------------------------------------------------------------------------

    ulong_t RunnableThread::getCurrentThreadID()
    {
        return ThreadSingleton::getInstance().getThreadSingleton()->getCurrentThreadID();
    }
    
    //--------------------------------------------------------------------------

    ulong_t RunnableThread::getMainThreadID()
    {
        return ThreadSingleton::getInstance().getThreadSingleton()->getMainThreadID();
    }
    
    //--------------------------------------------------------------------------

    void RunnableThread::sleepCurrentThread(uint32_t msec)
    {
        ThreadSingleton::getInstance().getThreadSingleton()->sleepCurrentThread(msec);
    }
    
    //--------------------------------------------------------------------------

    uint32_t RunnableThread::getThreadHardwareConcurrency()
    {
        return ThreadSingleton::getInstance().getThreadSingleton()->getThreadHardwareConcurrency();
    }
    
    //--------------------------------------------------------------------------
}
