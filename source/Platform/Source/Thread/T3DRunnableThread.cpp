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
#include "Thread/T3DSyncObject.h"
#include "Thread/T3DThreadManager.h"
#include "Adapter/T3DFactoryInterface.h"
#include "Adapter/T3DThreadInterface.h"
#include "T3DPlatform.h"
#include "Adapter/T3DPlatformInterface.h"
#include "T3DPlatformErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    RunnableThread::RunnableThread()
    {
        mThread = T3D_PLATFORM_FACTORY.createPlatformThread(); 
    }

    //--------------------------------------------------------------------------

    RunnableThread::~RunnableThread()
    {
        terminate(true);
        T3D_THREAD_MGR.removeThread(this);
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
                ret = T3D_ERR_IMPLEMENT_NOT_CREATED;
                break;
            }

            if (runnable == nullptr)
            {
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }

            // 启动线程，不过线程默认挂起状态
            ret = mThread->start(
                [](void *paramter) -> TResult
                {
                    RunnableThread *thread = static_cast<RunnableThread *>(paramter);
                    // 加入线程管理器里
                    T3D_THREAD_MGR.addThread(thread->getID(), thread);
                    return thread->run();
                },
                this,
                stackSize);
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

            // 保证 Runnable::init() 运行后才继续当前线程执行，先在这里等待
            ret = mInitSyncEvent.wait();
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult RunnableThread::run()
    {
        TResult ret;
                    
        if (mRunnable->init())
        {
            // 初始化执行完毕，继续执行 start 线程
            mInitSyncEvent.trigger();
            
            ret = mRunnable->run();
            mRunnable->exit();
            mRunnable = nullptr;
        }
        else
        {
            // 初始化执行完毕，继续执行 start 线程
            mInitSyncEvent.trigger();
            
            ret = T3D_ERR_THREAD_INIT;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult RunnableThread::suspend()
    {
        if (mThread == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mThread->suspend();
    }

    //--------------------------------------------------------------------------

    TResult RunnableThread::resume()
    {
        if (mThread == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mThread->resume();
    }

    //--------------------------------------------------------------------------

    TResult RunnableThread::terminate(bool wait)
    {
        if (mThread == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        return mThread->terminate(wait, mRunnable);
    }

    //--------------------------------------------------------------------------

    TResult RunnableThread::wait()
    {
        if (mThread == nullptr)
        {
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
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
        return T3D_PLATFORM.getPlatformImpl()->getCurrentThreadID();
    }
    
    //--------------------------------------------------------------------------

    ulong_t RunnableThread::getMainThreadID()
    {
        return T3D_PLATFORM.getPlatformImpl()->getMainThreadID();
    }
    
    //--------------------------------------------------------------------------

    void RunnableThread::sleep(uint32_t msec)
    {
        T3D_PLATFORM.getPlatformImpl()->sleepCurrentThread(msec);
    }
    
    //--------------------------------------------------------------------------

    uint32_t RunnableThread::getThreadHardwareConcurrency()
    {
        return T3D_PLATFORM.getPlatformImpl()->getThreadHardwareConcurrency();
    }
    
    //--------------------------------------------------------------------------
}
