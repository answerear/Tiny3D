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

#ifndef __T3D_HLSL_CROSS_PREREQUISITES_H__
#define __T3D_HLSL_CROSS_PREREQUISITES_H__


#include <T3DPlatformLib.h>


// 不直接用 T3D_EXPORT_API / T3D_IMPORT_API：那两个宏在 macOS / Linux 分支上是空的，
// 等于默认导出全部符号，会让静态吸收进来的 spirv-cross 符号泄漏到全局符号表。
// 本库配合 CXX_VISIBILITY_PRESET hidden，只导出显式标注的接口。
#if defined (T3D_OS_WINDOWS)
    #if defined (HLSLCROSS_EXPORT)
        #define T3D_HLSLCROSS_API   __declspec(dllexport)
    #else
        #define T3D_HLSLCROSS_API   __declspec(dllimport)
    #endif
#else
    #if defined (HLSLCROSS_EXPORT)
        #define T3D_HLSLCROSS_API   __attribute__((visibility("default")))
    #else
        #define T3D_HLSLCROSS_API
    #endif
#endif


#endif  /*__T3D_HLSL_CROSS_PREREQUISITES_H__*/
