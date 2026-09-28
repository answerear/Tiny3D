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

#ifndef __KEYBOARD_COMMAND_SOURCE_H__
#define __KEYBOARD_COMMAND_SOURCE_H__


#include "ComputeCommand.h"


/**
 * \brief 桌面键盘命令源
 * \remarks 键位与改造前的 ComputeApp::pollKeys 一字不差：R 重跑全部、1-8 单跑、
 *          C 切 CPU / GPU、V 开关剔除、P 暂停积分。
 *
 *          热键不响应先查输入法：中文输入法开着时按键被 IME 截走，SDL 收不到
 *          scancode，getKeyDown 自然一直是 false。
 */
class KeyboardCommandSource : public IComputeCommandSource
{
public:
    bool poll(ComputeCommand &cmd) override;
    const char *usage() const override;

private:
    /// getKeyDown 在 endFrame 前一直为 true，while (poll) 同帧只能吐一条
    uint64_t mEmittedFrame {~0ull};
};


#endif  /*__KEYBOARD_COMMAND_SOURCE_H__*/
