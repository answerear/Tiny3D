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

#include "TouchCommandSource.h"
#include "AutoCycleCommandSource.h"
#include "Input/T3DInput.h"
#include "Kernel/T3DAgent.h"
#include "Kernel/T3DTime.h"
#include "Render/T3DRenderWindow.h"


namespace Tiny3D
{
    void TouchCommandSource::setAutoCycle(AutoCycleCommandSource *cycle)
    {
        mAutoCycle = cycle;
    }

    void TouchCommandSource::notifyUserInteracted()
    {
        if (mAutoCycle != nullptr)
        {
            mAutoCycle->disable();
        }
    }

    void TouchCommandSource::enqueue(PresetCommandType type, int32_t value)
    {
        mHasPending = true;
        mPending.type = type;
        mPending.value = value;
        notifyUserInteracted();
    }

    void TouchCommandSource::resetSingleFinger()
    {
        mPendingTap = false;
        mIgnoreNextTapEnd = false;
        mDragging = false;
        mFingerDown = false;
        mLongPressFired = false;
    }

    void TouchCommandSource::resetTwoFinger()
    {
        mTwoFingerActive = false;
        mTwoFingerMoved = false;
    }

    Real TouchCommandSource::screenWidth() const
    {
        RenderWindowPtr rw = T3D_AGENT.getDefaultRenderWindow();
        if (rw == nullptr)
        {
            return 1080.0f;
        }

        const uint32_t width = rw->getDescriptor().Width;
        return (width > 0) ? Real(width) : 1080.0f;
    }

    Real TouchCommandSource::tapMoveThreshold() const
    {
        return screenWidth() * 0.02f;
    }

    Real TouchCommandSource::swipeThreshold() const
    {
        return screenWidth() * 0.08f;
    }

    void TouchCommandSource::handleTouches()
    {
        if (Input::getInstancePtr() == nullptr)
        {
            return;
        }

        if (mPendingTap && (Time::unscaledTime() - mPendingTapTime) >= mDoubleTapWindowMs)
        {
            mPendingTap = false;
        }

        const uint32_t count = T3D_INPUT.getTouchCount();
        if (count >= 2)
        {
            notifyUserInteracted();
            resetSingleFinger();

            const Touch a = T3D_INPUT.getTouch(0);
            const Touch b = T3D_INPUT.getTouch(1);
            if (!mTwoFingerActive)
            {
                mTwoFingerActive = true;
                mTwoFingerMoved = false;
                mTwoFingerPos0 = a.position;
                mTwoFingerPos1 = b.position;
            }
            else
            {
                const Vector2 da = a.position - mTwoFingerPos0;
                const Vector2 db = b.position - mTwoFingerPos1;
                const Real magA = Math::sqrt(da.x() * da.x() + da.y() * da.y());
                const Real magB = Math::sqrt(db.x() * db.x() + db.y() * db.y());
                if (magA > tapMoveThreshold() || magB > tapMoveThreshold())
                {
                    mTwoFingerMoved = true;
                }
            }
            return;
        }

        if (mTwoFingerActive)
        {
            if (!mTwoFingerMoved)
            {
                enqueue(PresetCommandType::kSwapOrder);
            }
            resetTwoFinger();
            resetSingleFinger();
            return;
        }

        if (count != 1)
        {
            return;
        }

        const Touch touch = T3D_INPUT.getTouch(0);
        if (touch.phase == TouchPhase::Began)
        {
            notifyUserInteracted();
            const uint64_t now = Time::unscaledTime();
            mFingerDownPos = touch.position;
            mFingerDownTime = now;
            mFingerDown = true;
            mDragging = false;
            mLongPressFired = false;
            if (mPendingTap && (now - mPendingTapTime) <= mDoubleTapWindowMs)
            {
                mPendingTap = false;
                mIgnoreNextTapEnd = true;
                enqueue(PresetCommandType::kSetPreset, 0);
            }
        }
        else if (touch.phase == TouchPhase::Moved)
        {
            const Vector2 moved = touch.position - mFingerDownPos;
            const Real mag = Math::sqrt(moved.x() * moved.x() + moved.y() * moved.y());
            if (mDragging || mag > tapMoveThreshold())
            {
                mDragging = true;
                mPendingTap = false;
            }
        }
        else if (touch.phase == TouchPhase::Ended)
        {
            if (mIgnoreNextTapEnd)
            {
                mIgnoreNextTapEnd = false;
            }
            else if (mLongPressFired)
            {
                // 长按已在按住期间发出 kToggleLog
            }
            else if (mDragging)
            {
                const Vector2 moved = touch.position - mFingerDownPos;
                const Real absX = Math::abs(moved.x());
                const Real absY = Math::abs(moved.y());
                if (absX >= swipeThreshold() && absX > absY)
                {
                    enqueue(moved.x() > 0.0f
                        ? PresetCommandType::kNext
                        : PresetCommandType::kPrev);
                }
            }
            else if (mFingerDown)
            {
                mPendingTap = true;
                mPendingTapTime = Time::unscaledTime();
            }
            mDragging = false;
            mFingerDown = false;
            mLongPressFired = false;
        }
        else if (touch.phase == TouchPhase::Canceled)
        {
            resetSingleFinger();
        }

        if (mFingerDown && !mDragging && !mLongPressFired
            && (Time::unscaledTime() - mFingerDownTime) >= mLongPressMs)
        {
            enqueue(PresetCommandType::kToggleLog);
            mLongPressFired = true;
            mPendingTap = false;
        }
    }

    bool TouchCommandSource::poll(PresetCommand &cmd)
    {
        handleTouches();

        if (!mHasPending)
        {
            cmd = PresetCommand{};
            return false;
        }

        cmd = mPending;
        mHasPending = false;
        mPending = PresetCommand{};
        return true;
    }

    const char *TouchCommandSource::usage() const
    {
        return "[PostProcess] touch: swipe L/R prev/next, double-tap reset, two-finger tap swap order, long-press toggle log";
    }
}
