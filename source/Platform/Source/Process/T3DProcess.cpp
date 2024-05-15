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


#include "Process/T3DProcess.h"
#include "T3DPlatformErrorDef.h"
#include "T3DPlatform.h"
#include "Adapter/T3DFactoryInterface.h"
#include "Adapter/T3DProcessInterface.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    Process::Process()
    {
        mProcess = T3D_PLATFORM_FACTORY.createPlatformProcess();
    }

    //--------------------------------------------------------------------------

    Process::~Process()
    {
        T3D_SAFE_DELETE(mProcess);
    }

    //--------------------------------------------------------------------------

    TResult Process::start(const String &exePath, const String &cmdLine)
    {
        if (mProcess != nullptr)
        {
            return mProcess->start(exePath, cmdLine);
        }

        return T3D_ERR_IMPLEMENT_NOT_CREATED;
    }

    //--------------------------------------------------------------------------

    TResult Process::wait(uint32_t timeout)
    {
        if (mProcess != nullptr)
        {
            return mProcess->wait(timeout);
        }

        return T3D_ERR_IMPLEMENT_NOT_CREATED;
    }

    //--------------------------------------------------------------------------

    TResult Process::terminate(int32_t exitCode)
    {
        if (mProcess != nullptr)
        {
            return mProcess->terminate(exitCode);
        }

        return T3D_ERR_IMPLEMENT_NOT_CREATED;
    }

    //--------------------------------------------------------------------------
}
