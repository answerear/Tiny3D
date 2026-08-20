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

#include "Adapter/OSX/T3DOSXThread.h"
#include <mach/mach.h>
#include <mach/thread_policy.h>
#include <pthread.h>


namespace Tiny3D
{
    void OSXThread::setAffinityMask(uint64_t mask)
    {
        mAffinityMask = mask;

        if (!mCreated)
        {
            return;
        }

        thread_affinity_policy_data_t policy;
        policy.affinity_tag = 0;
        if (mask != 0)
        {
            for (int i = 0; i < 64; ++i)
            {
                if ((mask & (1ull << i)) != 0)
                {
                    policy.affinity_tag = i + 1;
                    break;
                }
            }
        }

        const mach_port_t machThread = pthread_mach_thread_np(mThread);
        thread_policy_set(machThread, THREAD_AFFINITY_POLICY,
            (thread_policy_t)&policy, THREAD_AFFINITY_POLICY_COUNT);
    }
}
