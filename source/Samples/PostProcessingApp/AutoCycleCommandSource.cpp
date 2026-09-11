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

#include "AutoCycleCommandSource.h"
#include "Kernel/T3DTime.h"


namespace Tiny3D
{
    AutoCycleCommandSource::AutoCycleCommandSource(uint64_t intervalMs)
        : mIntervalMs(intervalMs == 0 ? 3000 : intervalMs)
    {
    }

    void AutoCycleCommandSource::disable()
    {
        mEnabled = false;
    }

    bool AutoCycleCommandSource::poll(PresetCommand &cmd)
    {
        cmd = PresetCommand{};

        if (!mEnabled)
        {
            return false;
        }

        const uint64_t now = Time::unscaledTime();
        if (mLastEmitTime == 0)
        {
            mLastEmitTime = now;
            return false;
        }

        if (now - mLastEmitTime < mIntervalMs)
        {
            return false;
        }

        mLastEmitTime = now;
        cmd.type = PresetCommandType::kNext;
        return true;
    }

    const char *AutoCycleCommandSource::usage() const
    {
        return "[PostProcess] auto-cycle: next preset every 3s; any touch disables auto-cycle";
    }
}
