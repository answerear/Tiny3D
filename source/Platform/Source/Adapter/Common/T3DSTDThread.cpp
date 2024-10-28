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


#include "T3DCommonErrorDef.h"
#include "Thread/T3DRunnable.h"
#include "Adapter/Common/T3DSTDThread.h"


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

    TResult STDThread::start(ThreadRoutine routine, void *parameter, uint32_t stackSize)
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

    TResult STDThread::terminate(bool wait, Runnable *runnable)
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
}
