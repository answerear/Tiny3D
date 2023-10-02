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


#ifndef __T3D_RHI_THREAD_H__
#define __T3D_RHI_THREAD_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "RHI/T3DRHICommand.h"

namespace Tiny3D
{
    class T3D_ENGINE_API RHIThread : public Runnable, public Object, public Singleton<RHIThread>
    {
    public:
        static RHIThreadPtr create();
        
        ~RHIThread() override;
        
        bool init() override;
        TResult run() override;
        void stop() final;
        void exit() override;

        void start();
        
        bool isRunning() const { return mIsRunning; }

        void addCommand(RHICommand * command);

    protected:
        RHIThread();
        
        void exchange();
        
        enum
        {
            kMaxCommandLists = 2,
        };

        Event                           mEvent;

        /// 双缓冲命令队列
        TArray<TList<RHICommandPtr>>    mCommandLists {};
        /// 当前线程执行队列序号
        int32_t                         mHanldeCommandListIdx {0};
        /// 当前入队列序号
        int32_t                         mEnqueueCommandListIdx {0};
        /// 线程是否在运行
        bool                            mIsRunning {false};
    };

    #define T3D_RHI_THREAD      (RHIThread::getInstance())
    
    #define T3D_ENQUEUE_RHI_COMMAND(CLASS, FUNC, PARAMS) \
        if (T3D_RHI_THREAD.isRunning()) \
        { \
            RHICommand##CLASS *command = new RHICommand##CLASS(PARAMS,  FUNC); \
            T3D_RHI_THREAD.addCommand(command); \
        } \
        else \
        { \
            std:;apply(CALLBACK, PARAMS); \
        }

    #define T3D_ENQUEUE_UNIQUE_RHI_COMMAND(CLASS, FUNC, PARAMS) \
        T3D_DECLARE_UNIQUE_RHI_COMMAND(CLASS)   \
        T3D_ENQUEUE_RHI_COMMAND(CLASS, FUNC, PARAMS)
}


#endif  /*__T3D_RHI_THREAD_H__*/
