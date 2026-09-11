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

#ifndef __POST_PROCESS_COMMAND_H__
#define __POST_PROCESS_COMMAND_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    enum class PresetCommandType
    {
        kNone = 0,
        kSetPreset,     // value = 目标预设 0-7
        kNext,          // 下一个预设（7 之后回 0）
        kPrev,
        kSwapOrder,     // 对应桌面 O 键
        kToggleLog      // 对应桌面 L 键
    };

    struct PresetCommand
    {
        PresetCommandType type {PresetCommandType::kNone};
        int32_t           value {0};
    };

    class IPresetCommandSource
    {
    public:
        virtual ~IPresetCommandSource() = default;
        /// 取出一条待处理命令，队列空返回 false
        virtual bool poll(PresetCommand &cmd) = 0;
        /// 启动时打进日志的操作说明
        virtual const char *usage() const = 0;
    };
}


#endif  /*__POST_PROCESS_COMMAND_H__*/
