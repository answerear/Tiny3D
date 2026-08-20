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

#include "Adapter/OSX/T3DOSXPlatform.h"
#include "T3DDir.h"

#include <cstring>
#include <limits.h>
#include <mach-o/dyld.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#import <AppKit/AppKit.h>


namespace Tiny3D
{
    OSXPlatform::OSXPlatform()
        : mMainThreadID((ulong_t)pthread_self())
    {
    }

    void OSXPlatform::memoryBarrier()
    {
        __sync_synchronize();
    }

    ulong_t OSXPlatform::getCurrentThreadID()
    {
        return (ulong_t)pthread_self();
    }

    ulong_t OSXPlatform::getMainThreadID()
    {
        return mMainThreadID;
    }

    void OSXPlatform::sleepCurrentThread(uint32_t msec)
    {
        usleep(msec * 1000);
    }

    uint32_t OSXPlatform::getThreadHardwareConcurrency()
    {
        int cores = 0;
        size_t size = sizeof(cores);
        if (sysctlbyname("hw.logicalcpu", &cores, &size, nullptr, 0) != 0 || cores <= 0)
        {
            cores = 0;
            size = sizeof(cores);
            sysctlbyname("hw.ncpu", &cores, &size, nullptr, 0);
        }

        return (cores > 0) ? (uint32_t)cores : 1u;
    }

    void OSXPlatform::traverseAllProcesses(const OnTraverseProcess &callback)
    {
        if (callback == nullptr)
        {
            return;
        }

        int mib[3] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL };
        size_t length = 0;
        if (sysctl(mib, 3, nullptr, &length, nullptr, 0) != 0 || length == 0)
        {
            return;
        }

        std::vector<uint8_t> buffer(length);
        if (sysctl(mib, 3, buffer.data(), &length, nullptr, 0) != 0)
        {
            return;
        }

        const size_t count = length / sizeof(kinfo_proc);
        const kinfo_proc *procs = reinterpret_cast<const kinfo_proc *>(buffer.data());
        for (size_t i = 0; i < count; ++i)
        {
            char name[MAXCOMLEN + 1] = {0};
            memcpy(name, procs[i].kp_proc.p_comm, MAXCOMLEN);
            if (callback((ulong_t)procs[i].kp_proc.p_pid, name))
            {
                break;
            }
        }
    }

    ulong_t OSXPlatform::getCurrentProcessID()
    {
        return (ulong_t)getpid();
    }

    const String &OSXPlatform::getCurrentProcessName()
    {
        if (mProcessName.empty())
        {
            char raw[PATH_MAX];
            uint32_t size = sizeof(raw);
            if (_NSGetExecutablePath(raw, &size) == 0)
            {
                char resolved[PATH_MAX];
                const char *path = (realpath(raw, resolved) != nullptr) ? resolved : raw;
                String dir;
                Dir::parsePath(String(path), dir, mProcessName);
            }

            if (mProcessName.empty())
            {
                const char *name = getprogname();
                mProcessName = (name != nullptr) ? name : "";
            }
        }

        return mProcessName;
    }

    bool OSXPlatform::wakeupProcess(ulong_t pid)
    {
        @autoreleasepool
        {
            NSRunningApplication *app = [NSRunningApplication
                runningApplicationWithProcessIdentifier:(pid_t)pid];
            if (app == nil)
            {
                return false;
            }

            return [app activateWithOptions:NSApplicationActivateAllWindows] == YES;
        }
    }
}
