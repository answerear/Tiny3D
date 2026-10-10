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

#ifndef __T3D_SPIRV_CROSS_DRIVER_H__
#define __T3D_SPIRV_CROSS_DRIVER_H__


#include "T3DHLSLCrossCompiler.h"


namespace Tiny3D
{
    /**
     * @class   SpirvCrossDriver
     * @brief   SPIRV-Cross 封装：SPIR-V → GLSL / ESSL / HLSL / MSL
     * @remarks 选项与资源重映射逐项沿用 ShaderConductor 4f36caf 的 ConvertBinary()，
     *          保证与旧产物逐字节一致。改任何一项都要重新对拍。
     */
    class SpirvCrossDriver
    {
    public:
        static bool translate(const TArray<uint8_t> &spirv,
                              HLSLStage stage,
                              const String &entryPoint,
                              const HLSLCrossTarget &target,
                              String &outSource,
                              String &outMessage);
    };
}


#endif  /*__T3D_SPIRV_CROSS_DRIVER_H__*/
