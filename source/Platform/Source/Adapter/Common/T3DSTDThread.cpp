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

#include "Adapter/Common/T3DSTDThread.h"

#include "T3DCommonErrorDef.h"
#include "Thread/T3DRunnable.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    STDThread::STDThread()
    {
        
    }

    //--------------------------------------------------------------------------

    STDThread::~STDThread()
    {
        
    }

    //--------------------------------------------------------------------------

    TResult STDThread::start(RunnableThread *runnableThread, Runnable *runnable, uint32_t stackSize)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult STDThread::suspend()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult STDThread::resume()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult STDThread::terminate(bool wait)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult STDThread::wait()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    ulong_t STDThread::getID() const
    {
        return 0;
    }
    
    //--------------------------------------------------------------------------

    ulong_t STDThreadSingleton::getCurrentThreadID()
    {
        return 0;
    }

    //--------------------------------------------------------------------------

    ulong_t STDThreadSingleton::getMainThreadID()
    {
        return 0;
    }

    //--------------------------------------------------------------------------

    void STDThreadSingleton::sleepCurrentThread(uint32_t msec)
    {
        
    }

    //--------------------------------------------------------------------------

    uint32_t STDThreadSingleton::getThreadHardwareConcurrency()
    {
        return 0;
    }

    //--------------------------------------------------------------------------
}
