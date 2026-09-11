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
#include "Input/T3DInput.h"
#include "Kernel/T3DTime.h"


namespace Tiny3D
{
    bool KeyboardCommandSource::poll(PresetCommand &cmd)
    {
        cmd = PresetCommand{};

        if (Input::getInstancePtr() == nullptr)
        {
            return false;
        }

        const uint64_t frame = Time::frameCount();
        if (frame == mEmittedFrame)
        {
            return false;
        }

        if (T3D_INPUT.getKeyDown(APP_SCANCODE_0))
        {
            cmd.type = PresetCommandType::kSetPreset;
            cmd.value = 0;
        }
        else if (T3D_INPUT.getKeyDown(APP_SCANCODE_1))
        {
            cmd.type = PresetCommandType::kSetPreset;
            cmd.value = 1;
        }
        else if (T3D_INPUT.getKeyDown(APP_SCANCODE_2))
        {
            cmd.type = PresetCommandType::kSetPreset;
            cmd.value = 2;
        }
        else if (T3D_INPUT.getKeyDown(APP_SCANCODE_3))
        {
            cmd.type = PresetCommandType::kSetPreset;
            cmd.value = 3;
        }
        else if (T3D_INPUT.getKeyDown(APP_SCANCODE_4))
        {
            cmd.type = PresetCommandType::kSetPreset;
            cmd.value = 4;
        }
        else if (T3D_INPUT.getKeyDown(APP_SCANCODE_5))
        {
            cmd.type = PresetCommandType::kSetPreset;
            cmd.value = 5;
        }
        else if (T3D_INPUT.getKeyDown(APP_SCANCODE_6))
        {
            cmd.type = PresetCommandType::kSetPreset;
            cmd.value = 6;
        }
        else if (T3D_INPUT.getKeyDown(APP_SCANCODE_7))
        {
            cmd.type = PresetCommandType::kSetPreset;
            cmd.value = 7;
        }
        else if (T3D_INPUT.getKeyDown(APP_SCANCODE_O))
        {
            cmd.type = PresetCommandType::kSwapOrder;
        }
        else if (T3D_INPUT.getKeyDown(APP_SCANCODE_L))
        {
            cmd.type = PresetCommandType::kToggleLog;
        }
        else
        {
            return false;
        }

        mEmittedFrame = frame;
        return true;
    }

    const char *KeyboardCommandSource::usage() const
    {
        return "[PostProcess] keys: 0-7 preset, O swap order, L toggle camera log";
    }
}
