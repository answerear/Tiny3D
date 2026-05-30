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

#ifndef __T3D_POSIX_PROCESS_H__
#define __T3D_POSIX_PROCESS_H__


#include "T3DNoncopyable.h"
#include "Adapter/T3DProcessInterface.h"
#include "Memory/T3DMemory.h"
#include <sys/types.h>


namespace Tiny3D
{
    class PosixProcess : public IProcess, public Noncopyable
    {
    public:
        PosixProcess();

        ~PosixProcess() override;

        ulong_t getID() const override;

        const String &getName() const override;

        TResult start(const String &exePath, const String &cmdLine) override;

        TResult wait(uint32_t timeout = -1) override;

        TResult terminate(int32_t exitCode) override;

        uint32_t getExitCode() const override;

    protected:
        pid_t       mPid;
        String      mName;
        uint32_t    mExitCode;
        bool        mRunning;
    };
}


#endif  /*__T3D_POSIX_PROCESS_H__*/
