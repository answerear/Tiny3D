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

#ifndef __T3D_HLSL_SEMANTIC_FIX_H__
#define __T3D_HLSL_SEMANTIC_FIX_H__


#include "T3DHLSLCrossCompiler.h"


namespace Tiny3D
{
    /**
     * @brief   把 SPIRV-Cross 输出 HLSL 里统一挂上的 TEXCOORD<n> 语义还原成原语义
     * @remarks DXC 把 stage IO 命名为 in.var.NORMAL，SPIRV-Cross 转成 in_var_NORMAL
     *          并一律标 TEXCOORD<n>；这里从变量名尾部把语义抠回来。
     *          算法原样搬自 ShaderCompiler::fixSpirVCrossForHLSLSemantics。
     */
    void fixSpirVCrossForHLSLSemantics(String &content);
}


#endif  /*__T3D_HLSL_SEMANTIC_FIX_H__*/
