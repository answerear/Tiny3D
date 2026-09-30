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

#ifndef __T3D_DXC_DRIVER_H__
#define __T3D_DXC_DRIVER_H__


#include "T3DHLSLCrossCompiler.h"


namespace Tiny3D
{
    /**
     * @class   DxcDriver
     * @brief   DXC 封装：HLSL → SPIR-V
     * @remarks dxcompiler 在运行期经 SharedLibrary 加载，dxcapi.h 只在 .cpp 里可见。
     *          走 IDxcLibrary / IDxcCompiler 这组老接口，新旧 DXC 都提供，
     *          ShaderConductor 当年也是这组，便于对拍。
     */
    class DxcDriver
    {
    public:
        static bool compileToSpirV(const HLSLCrossSource &source,
                                   const HLSLCrossOptions &options,
                                   TArray<uint8_t> &outSpirv,
                                   String &outMessage);

        static bool isAvailable(String &outMessage);
    };
}


#endif  /*__T3D_DXC_DRIVER_H__*/
