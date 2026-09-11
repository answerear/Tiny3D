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

#ifndef __AUTO_CYCLE_COMMAND_SOURCE_H__
#define __AUTO_CYCLE_COMMAND_SOURCE_H__


#include "PostProcessCommand.h"


namespace Tiny3D
{
    class AutoCycleCommandSource : public IPresetCommandSource
    {
    public:
        explicit AutoCycleCommandSource(uint64_t intervalMs = 3000);

        void disable();
        bool isEnabled() const { return mEnabled; }

        bool poll(PresetCommand &cmd) override;
        const char *usage() const override;

    private:
        bool        mEnabled {true};
        uint64_t    mIntervalMs {3000};
        uint64_t    mLastEmitTime {0};
    };
}


#endif  /*__AUTO_CYCLE_COMMAND_SOURCE_H__*/
