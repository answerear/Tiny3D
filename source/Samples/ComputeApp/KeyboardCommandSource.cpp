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

#include "KeyboardCommandSource.h"


using namespace Tiny3D;


namespace
{
    struct CaseKey
    {
        ScanCode    code;
        char        label;
    };

    /// 用例按注册顺序依次对应 1-9、A-Z；C / P / R / V 已是功能键，字母部分跳过它们
    const CaseKey kCaseKeys[] =
    {
        { APP_SCANCODE_1, '1' }, { APP_SCANCODE_2, '2' }, { APP_SCANCODE_3, '3' },
        { APP_SCANCODE_4, '4' }, { APP_SCANCODE_5, '5' }, { APP_SCANCODE_6, '6' },
        { APP_SCANCODE_7, '7' }, { APP_SCANCODE_8, '8' }, { APP_SCANCODE_9, '9' },
        { APP_SCANCODE_A, 'A' }, { APP_SCANCODE_B, 'B' }, { APP_SCANCODE_D, 'D' },
        { APP_SCANCODE_E, 'E' }, { APP_SCANCODE_F, 'F' }, { APP_SCANCODE_G, 'G' },
        { APP_SCANCODE_H, 'H' }, { APP_SCANCODE_I, 'I' }, { APP_SCANCODE_J, 'J' },
        { APP_SCANCODE_K, 'K' }, { APP_SCANCODE_L, 'L' }, { APP_SCANCODE_M, 'M' },
        { APP_SCANCODE_N, 'N' }, { APP_SCANCODE_O, 'O' }, { APP_SCANCODE_Q, 'Q' },
        { APP_SCANCODE_S, 'S' }, { APP_SCANCODE_T, 'T' }, { APP_SCANCODE_U, 'U' },
        { APP_SCANCODE_W, 'W' }, { APP_SCANCODE_X, 'X' }, { APP_SCANCODE_Y, 'Y' },
        { APP_SCANCODE_Z, 'Z' },
    };

    const int32_t kCaseKeyCount = int32_t(sizeof(kCaseKeys) / sizeof(kCaseKeys[0]));
}


char KeyboardCommandSource::caseKeyLabel(size_t index)
{
    return (index < size_t(kCaseKeyCount)) ? kCaseKeys[index].label : '\0';
}

bool KeyboardCommandSource::poll(ComputeCommand &cmd)
{
    cmd = ComputeCommand{};

    if (Input::getInstancePtr() == nullptr)
    {
        return false;
    }

    const uint64_t frame = Time::frameCount();
    if (frame == mEmittedFrame)
    {
        return false;
    }

    if (T3D_INPUT.getKeyDown(APP_SCANCODE_R))
    {
        cmd.type = ComputeCommandType::kRetestAll;
    }
    else if (T3D_INPUT.getKeyDown(APP_SCANCODE_C))
    {
        cmd.type = ComputeCommandType::kToggleCpu;
    }
    else if (T3D_INPUT.getKeyDown(APP_SCANCODE_V))
    {
        cmd.type = ComputeCommandType::kToggleCull;
    }
    else if (T3D_INPUT.getKeyDown(APP_SCANCODE_P))
    {
        cmd.type = ComputeCommandType::kTogglePause;
    }
    else
    {
        for (int32_t i = 0; i < kCaseKeyCount; ++i)
        {
            if (T3D_INPUT.getKeyDown(kCaseKeys[i].code))
            {
                cmd.type = ComputeCommandType::kRetestOne;
                cmd.value = i;
                break;
            }
        }

        if (cmd.type == ComputeCommandType::kNone)
        {
            return false;
        }
    }

    mEmittedFrame = frame;
    return true;
}

const char *KeyboardCommandSource::usage() const
{
    return "[ComputeApp] keys: R retest, 1-9 / A-Z (except C P R V) single case, C CPU/GPU, V cull+indirect, P pause";
}
