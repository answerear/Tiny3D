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

#ifndef __T3D_PROCESS_H__
#define __T3D_PROCESS_H__


#include "T3DMacro.h"
#include "T3DNoncopyable.h"
#include "T3DPlatformPrerequisites.h"


namespace Tiny3D
{
    class IProcess;
    
    class T3D_PLATFORM_API Process : public Noncopyable
    {
    public:
        Process();
        
        ~Process() override;

        ulong_t getID() const;

        const String &getName() const;
        
        TResult start(const String &exePath, const String &cmdLine);

        TResult wait(uint32_t timeout = -1);

        TResult terminate(int32_t exitCode);

        static ulong_t getCurrentProcessID();

        static const String &getCurrentProcessName();

        static void traverseAllProcesses(const OnTraverseProcess &callback);

        static bool isProcessRunning(const String &procName);

        static bool isProcessRunning(ulong_t pid);

    protected:
        IProcess    *mProcess {nullptr};
    };
}

#endif  /*__T3D_NONCOPYABLE_H__*/
