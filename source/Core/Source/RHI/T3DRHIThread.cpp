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


#include "RHI/T3DRHIThread.h"
#include "Kernel/T3DAgent.h"
#include "RHI/T3DRHICommand.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    RHIThreadPtr RHIThread::create()
    {
        return new RHIThread();
    }
    
    //--------------------------------------------------------------------------

    RHIThread::RHIThread()
    {
        
    }

    //--------------------------------------------------------------------------

    RHIThread::~RHIThread()
    {
        for (auto &cmdList : mCommandLists)
        {
            cmdList.clear();
        }
    }

    //--------------------------------------------------------------------------

    bool RHIThread::init()
    {
        mHanldeCommandListIdx = 0;
        mEnqueueCommandListIdx = (mHanldeCommandListIdx + 1) % kMaxCommandLists;
        mCommandLists.emplace_back();
        mCommandLists.emplace_back();
        mIsRunning = true;
        return true;
    }

    //--------------------------------------------------------------------------

    void RHIThread::exchange()
    {
        mCommandLists[mHanldeCommandListIdx].clear();
        mHanldeCommandListIdx = mEnqueueCommandListIdx;
        mEnqueueCommandListIdx = (mHanldeCommandListIdx + 1) % kMaxCommandLists;
    }

    //--------------------------------------------------------------------------

    void RHIThread::resume()
    {
        exchange();
        mEvent.trigger();
    }

    //--------------------------------------------------------------------------

    TResult RHIThread::run()
    {
        while (mIsRunning)
        {
            // 线程等待
            mEvent.wait();
            
            // 循环执行 RHI 命令
            for (auto command : mCommandLists[mHanldeCommandListIdx])
            {
                command->execute();
            }
            
            T3D_AGENT.resumeEngineThread();
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void RHIThread::stop()
    {
        mIsRunning = false;
        mEvent.trigger();
    }
    
    //--------------------------------------------------------------------------

    void RHIThread::exit()
    {

    }
    
    //--------------------------------------------------------------------------

    TResult RHIThread::addCommand(RHICommand *command)
    {
        mCommandLists[mEnqueueCommandListIdx].push_back(RHICommandSafePtr(command));
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
