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


#ifndef __T3D_TEXTURE_DESC_H__
#define __T3D_TEXTURE_DESC_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Kernel/T3DConstant.h"
#include "Render/T3DMultisamplerDesc.h"


namespace Tiny3D
{
    TSTRUCT()
    struct T3D_ENGINE_API TextureDesc
    {
        /// 纹理宽度
        TPROPERTY()
        int32_t             Width {0};
        /// 纹理高度
        TPROPERTY()
        int32_t             Height {0};
        /// 纹理色深
        TPROPERTY()
        int32_t             ColorDepth {0};
        /// 抗锯齿采样参数
        TPROPERTY()
        MultisamplerDesc    MSAA {};
        /// Mipmap
        TPROPERTY()
        int32_t             Mipmap {0};
        /// 纹理名称
        TPROPERTY()
        String              Name {};
        /// 纹理格式
        TPROPERTY()
        PixelFormat         Format {PixelFormat::E_PF_UNKNOWN};
    };
}


#endif  /*__T3D_TEXTURE_DESC_H__*/
