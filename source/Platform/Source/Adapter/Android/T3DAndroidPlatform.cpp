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

#include "Adapter/Android/T3DAndroidPlatform.h"
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    AndroidPlatform::AndroidPlatform()
        : mMainThreadID((ulong_t)pthread_self())
    {
    }

    //--------------------------------------------------------------------------

    void AndroidPlatform::memoryBarrier()
    {
        __sync_synchronize();
    }

    //--------------------------------------------------------------------------

    ulong_t AndroidPlatform::getCurrentThreadID()
    {
        return (ulong_t)pthread_self();
    }

    //--------------------------------------------------------------------------

    ulong_t AndroidPlatform::getMainThreadID()
    {
        return mMainThreadID;
    }

    //--------------------------------------------------------------------------

    void AndroidPlatform::sleepCurrentThread(uint32_t msec)
    {
        usleep(msec * 1000);
    }

    //--------------------------------------------------------------------------

    uint32_t AndroidPlatform::getThreadHardwareConcurrency()
    {
        return std::thread::hardware_concurrency();
    }

    //--------------------------------------------------------------------------

    void AndroidPlatform::traverseAllProcesses(const OnTraverseProcess &callback)
    {
        if (!callback)
            return;

        DIR *dir = opendir("/proc");
        if (dir == nullptr)
            return;

        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr)
        {
            // Check if the entry is a number (PID directory)
            bool isNum = true;
            for (int i = 0; entry->d_name[i] != '\0'; ++i)
            {
                if (entry->d_name[i] < '0' || entry->d_name[i] > '9')
                {
                    isNum = false;
                    break;
                }
            }

            if (!isNum)
                continue;

            ulong_t pid = (ulong_t)atol(entry->d_name);

            // Read process name from /proc/[pid]/cmdline
            char path[256];
            snprintf(path, sizeof(path), "/proc/%s/cmdline", entry->d_name);

            String pname;
            FILE *fp = fopen(path, "r");
            if (fp != nullptr)
            {
                char buf[256] = {0};
                if (fgets(buf, sizeof(buf), fp) != nullptr)
                {
                    pname = buf;
                }
                fclose(fp);
            }

            if (!callback(pid, pname))
                break;
        }

        closedir(dir);
    }

    //--------------------------------------------------------------------------

    ulong_t AndroidPlatform::getCurrentProcessID()
    {
        return (ulong_t)getpid();
    }

    //--------------------------------------------------------------------------

    const String &AndroidPlatform::getCurrentProcessName()
    {
        if (mProcessName.empty())
        {
            char buf[256] = {0};
            FILE *fp = fopen("/proc/self/cmdline", "r");
            if (fp != nullptr)
            {
                if (fgets(buf, sizeof(buf), fp) != nullptr)
                {
                    mProcessName = buf;
                }
                fclose(fp);
            }
        }

        return mProcessName;
    }

    //--------------------------------------------------------------------------

    bool AndroidPlatform::wakeupProcess(ulong_t pid)
    {
        return (kill((pid_t)pid, SIGCONT) == 0);
    }

    //--------------------------------------------------------------------------
}
