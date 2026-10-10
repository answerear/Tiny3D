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


#include "ComputeCommand.h"


/**
 * \brief 移动端触摸命令源
 * \remarks 手势骨架照抄 PostProcessingApp/TouchCommandSource，只换命令映射：
 *          双击 → kRetestAll（R）、单指横滑 → kToggleCpu（C）、
 *          双指点击 → kToggleCull（V）、长按 → kTogglePause（P）。
 *
 *          桌面的 1-8 单跑用例不映射手势：八个离散目标在触摸上没有自然对应，
 *          而那本来就是桌面调试用的功能。
 *
 *          **不做自动轮播。** 桌面版没有这个功能，移动端也不加，否则「移动端表现
 *          不同」会多出一个不必要的嫌疑人。所以这里没有 PostProcessingApp 那套
 *          setAutoCycle / notifyUserInteracted 接线。
 *
 *          阈值按屏宽百分比算而不是绝对像素，这是高 DPI 屏上不误判的前提。
 */
class TouchCommandSource : public IComputeCommandSource
{
public:
    bool poll(ComputeCommand &cmd) override;
    const char *usage() const override;

private:
    void enqueue(ComputeCommandType type, int32_t value = 0);
    void resetSingleFinger();
    void resetTwoFinger();
    Tiny3D::Real screenWidth() const;
    Tiny3D::Real tapMoveThreshold() const;
    Tiny3D::Real swipeThreshold() const;
    void handleTouches();

private:
    bool            mHasPending {false};
    ComputeCommand  mPending {};

    uint64_t    mDoubleTapWindowMs {300};
    uint64_t    mLongPressMs {800};
    uint64_t    mPendingTapTime {0};
    uint64_t    mFingerDownTime {0};
    bool        mPendingTap {false};
    bool        mIgnoreNextTapEnd {false};
    bool        mDragging {false};
    bool        mFingerDown {false};
    bool        mLongPressFired {false};
    Tiny3D::Vector2 mFingerDownPos {0.0f, 0.0f};

    bool        mTwoFingerActive {false};
    bool        mTwoFingerMoved {false};
    Tiny3D::Vector2 mTwoFingerPos0 {0.0f, 0.0f};
    Tiny3D::Vector2 mTwoFingerPos1 {0.0f, 0.0f};
};


#endif  /*__TOUCH_COMMAND_SOURCE_H__*/
