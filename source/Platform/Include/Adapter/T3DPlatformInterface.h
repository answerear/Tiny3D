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

#ifndef __T3D_PLATFORM_INTERFACE_H__
#define __T3D_PLATFORM_INTERFACE_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DType.h"
#include "T3DMacro.h"
#include "T3DSingleton.h"
#include "Memory/T3DMemory.h"


namespace Tiny3D
{
    class T3D_PLATFORM_API IPlatform : public Allocator
    {
        T3D_DECLARE_INTERFACE(IPlatform);

    public:
        virtual void memoryBarrier() = 0;

        virtual ulong_t getCurrentThreadID() = 0;

        virtual ulong_t getMainThreadID() = 0;

        virtual void sleepCurrentThread(uint32_t msec) = 0;

        virtual uint32_t getThreadHardwareConcurrency() = 0;

        virtual void traverseAllProcesses(const OnTraverseProcess &callback) = 0;

        virtual ulong_t getCurrentProcessID() = 0;

        virtual const String &getCurrentProcessName() = 0;

        virtual bool wakeupProcess(ulong_t pid) = 0;
    };
}


#endif  /*__T3D_PLATFORM_INTERFACE_H__*/
