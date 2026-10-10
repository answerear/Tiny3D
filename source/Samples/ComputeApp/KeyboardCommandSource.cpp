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

    static const ScanCode kDigitKeys[8] =
    {
        APP_SCANCODE_1, APP_SCANCODE_2, APP_SCANCODE_3, APP_SCANCODE_4,
        APP_SCANCODE_5, APP_SCANCODE_6, APP_SCANCODE_7, APP_SCANCODE_8
    };

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
        for (int32_t i = 0; i < 8; ++i)
        {
            if (T3D_INPUT.getKeyDown(kDigitKeys[i]))
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
    return "[ComputeApp] keys: R retest, 1-8 single case, C CPU/GPU, V cull+indirect, P pause";
}
