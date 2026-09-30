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

#include "T3DHLSLCrossCompiler.h"
#include "T3DDxcDriver.h"
#include "T3DSpirvCrossDriver.h"
#include "T3DHLSLSemanticFix.h"


namespace Tiny3D
{
    namespace
    {
        void appendMessage(String &dst, const String &msg)
        {
            if (msg.empty())
            {
                return;
            }
            if (!dst.empty())
            {
                dst += "\n";
            }
            dst += msg;
        }

        HLSLCrossResult compileImpl(const HLSLCrossSource &source,
            const HLSLCrossOptions &options, const HLSLCrossTarget &target)
        {
            HLSLCrossResult result;

            TArray<uint8_t> spirv;
            String dxcMessage;
            const bool ok = DxcDriver::compileToSpirV(source, options, spirv, dxcMessage);
            // 成功时这里是 DXC 的警告，透传但不判失败
            result.message = dxcMessage;
            if (!ok)
            {
                result.hasError = true;
                return result;
            }

            if (target.language == HLSLTarget::kSpirV)
            {
                result.isText = false;
                result.target = std::move(spirv);
                return result;
            }

            String translated;
            String xcMessage;
            if (!SpirvCrossDriver::translate(spirv, source.stage, source.entryPoint, target, translated, xcMessage))
            {
                result.hasError = true;
                appendMessage(result.message, xcMessage);
                return result;
            }

            if (target.language == HLSLTarget::kHlsl)
            {
                fixSpirVCrossForHLSLSemantics(translated);
            }

            result.isText = true;
            result.target.assign(translated.begin(), translated.end());
            return result;
        }
    }

    //--------------------------------------------------------------------------

    HLSLCrossResult HLSLCrossCompiler::compile(const HLSLCrossSource &source,
        const HLSLCrossOptions &options, const HLSLCrossTarget &target)
    {
        // 动态库边界：任何异常（bad_alloc、SPIRV-Cross 解析阶段的 CompilerError 等）都不能越过
        try
        {
            return compileImpl(source, options, target);
        }
        catch (const std::exception &e)
        {
            HLSLCrossResult result;
            result.hasError = true;
            result.message = String("HLSLCross internal error: ") + e.what();
            return result;
        }
        catch (...)
        {
            HLSLCrossResult result;
            result.hasError = true;
            result.message = "HLSLCross internal error: unknown exception.";
            return result;
        }
    }

    //--------------------------------------------------------------------------

    bool HLSLCrossCompiler::isAvailable(String &message)
    {
        try
        {
            return DxcDriver::isAvailable(message);
        }
        catch (...)
        {
            message = "HLSLCross internal error while probing dxcompiler.";
            return false;
        }
    }
}
