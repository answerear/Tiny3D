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


#include "Kernel/T3DTime.h"
#include "Time/T3DDateTime.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    void Time::start()
    {
        mStartMSec = mLastMSec = (uint64_t)DateTime::currentMSecsSinceEpoch();
        mDeltaTime = 0;
        mUnscaledDelta = 0;
        mTime = 0;
        mUnscaledTime = 0;
        mFrameCount = 0;
        mFixedAccumulator = 0;
    }

    //--------------------------------------------------------------------------

    void Time::tick()
    {
        uint64_t now = (uint64_t)DateTime::currentMSecsSinceEpoch();

        // 墙钟非单调保护：回退 / 跳变时钳到 [0, mMaxDeltaTime]
        uint64_t raw = (now > mLastMSec) ? (now - mLastMSec) : 0;
        if (raw > mMaxDeltaTime)
        {
            raw = mMaxDeltaTime;
        }
        mLastMSec = now;

        mUnscaledDelta = raw;
        // 千分比缩放，保持全整数运算
        mDeltaTime = raw * (uint64_t)mTimeScale / 1000;

        mUnscaledTime += mUnscaledDelta;
        mTime += mDeltaTime;

        // 用缩放后时间累积，使慢放 / 暂停同样作用于 FixedUpdate
        mFixedAccumulator += mDeltaTime;
        ++mFrameCount;
    }

    //--------------------------------------------------------------------------

    bool Time::stepFixed()
    {
        if (mFixedAccumulator < mFixedDeltaTime)
        {
            return false;
        }

        mFixedAccumulator -= mFixedDeltaTime;
        return true;
    }

    //--------------------------------------------------------------------------

    T3D_INSTANTIATE_SINGLETON(Time)
}

