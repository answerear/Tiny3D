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

#ifndef __COMPUTE_COMMAND_H__
#define __COMPUTE_COMMAND_H__


#include <Tiny3D.h>


/**
 * \brief ComputeApp 的操作命令
 * \remarks 传枚举而不是 scancode，这是「输入解耦」的实际含义：桌面用键盘、Android
 *          用触摸手势，两端产出同一套命令，ComputeApp 只认命令不认输入设备。
 */
enum class ComputeCommandType
{
    kNone = 0,
    kRetestAll,     ///< 桌面 R：重跑全部自检用例
    kRetestOne,     ///< 桌面 1-8：value = 用例下标
    kToggleCpu,     ///< 桌面 C：可视轨 GPU / CPU 切换
    kToggleCull,    ///< 桌面 V：GPU 剔除 + 间接绘制
    kTogglePause    ///< 桌面 P：冻结积分，方便抓帧
};


struct ComputeCommand
{
    ComputeCommandType type {ComputeCommandType::kNone};
    int32_t            value {0};
};


class IComputeCommandSource
{
public:
    virtual ~IComputeCommandSource() = default;

    /// 取出一条待处理命令，队列空返回 false
    virtual bool poll(ComputeCommand &cmd) = 0;

    /// 启动时打进日志的操作说明
    virtual const char *usage() const = 0;
};


#endif  /*__COMPUTE_COMMAND_H__*/
