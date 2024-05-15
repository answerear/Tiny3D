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


#ifndef __T3D_RENDER_TARGET_DESC_H__
#define __T3D_RENDER_TARGET_DESC_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Render/T3DMultisamplerDesc.h"


namespace Tiny3D
{
    TSTRUCT()
    struct T3D_ENGINE_API RenderWindowDesc
    {
        THandle externalHandle {nullptr};
        /// 窗口横向坐标位置，全屏窗口忽略该参数
        TPROPERTY()
        int32_t Left {0};
        /// 窗口纵向坐标位置，全屏窗口忽略该参数
        TPROPERTY()
        int32_t Top {0};
        /// 窗口宽度
        TPROPERTY()
        int32_t Width {0};
        /// 窗口高度
        TPROPERTY()
        int32_t Height {0};
        /// 窗口色深
        TPROPERTY()
        int32_t ColorDepth {0};
        /// 抗锯齿质量
        TPROPERTY()
        MultisamplerDesc    MSAA {};
        TPROPERTY()
        /// 窗口标题
        String  Title {};
        /// 图标文件路径
        TPROPERTY()
        String  IconPath {};
        /// 是否全屏，终端平台忽略该参数
        TPROPERTY()
        bool    IsFullscreen {false};
        /// 是否开启垂直同步
        TPROPERTY()
        bool    IsVsync {false};
        /// 窗口是否可调整大小
        TPROPERTY()
        bool    IsResizable {false};
    };
}


#endif  /*__T3D_RENDER_TARGET_DESC_H__*/
