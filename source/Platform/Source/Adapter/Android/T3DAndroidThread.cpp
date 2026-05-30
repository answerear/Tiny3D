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

#include "Adapter/Android/T3DAndroidThread.h"
#include <sched.h>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    void AndroidThread::setAffinityMask(uint64_t mask)
    {
        if (!mCreated)
            return;

        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);

        for (int i = 0; i < 64; ++i)
        {
            if (mask & ((uint64_t)1 << i))
            {
                CPU_SET(i, &cpuset);
            }
        }

        if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) == 0)
        {
            mAffinityMask = mask;
        }
    }

    //--------------------------------------------------------------------------
}
