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

#ifndef __TOUCH_COMMAND_SOURCE_H__
#define __TOUCH_COMMAND_SOURCE_H__


#include "PostProcessCommand.h"


namespace Tiny3D
{
    class AutoCycleCommandSource;

    class TouchCommandSource : public IPresetCommandSource
    {
    public:
        void setAutoCycle(AutoCycleCommandSource *cycle);

        bool poll(PresetCommand &cmd) override;
        const char *usage() const override;

    private:
        void notifyUserInteracted();
        void enqueue(PresetCommandType type, int32_t value = 0);
        void resetSingleFinger();
        void resetTwoFinger();
        Real screenWidth() const;
        Real tapMoveThreshold() const;
        Real swipeThreshold() const;
        void handleTouches();

    private:
        AutoCycleCommandSource *mAutoCycle {nullptr};

        bool            mHasPending {false};
        PresetCommand   mPending {};

        uint64_t    mDoubleTapWindowMs {300};
        uint64_t    mLongPressMs {800};
        uint64_t    mPendingTapTime {0};
        uint64_t    mFingerDownTime {0};
        bool        mPendingTap {false};
        bool        mIgnoreNextTapEnd {false};
        bool        mDragging {false};
        bool        mFingerDown {false};
        bool        mLongPressFired {false};
        Vector2     mFingerDownPos {0.0f, 0.0f};

        bool        mTwoFingerActive {false};
        bool        mTwoFingerMoved {false};
        Vector2     mTwoFingerPos0 {0.0f, 0.0f};
        Vector2     mTwoFingerPos1 {0.0f, 0.0f};
    };
}


#endif  /*__TOUCH_COMMAND_SOURCE_H__*/
