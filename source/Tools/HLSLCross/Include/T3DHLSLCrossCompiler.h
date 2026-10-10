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

#ifndef __T3D_HLSL_CROSS_COMPILER_H__
#define __T3D_HLSL_CROSS_COMPILER_H__


// 只取 String / TArray，不要包 <Tiny3D.h>：本库不依赖 T3DCore。
#include <T3DType.h>
#include "T3DHLSLCrossPrerequisites.h"


namespace Tiny3D
{
    /// 着色器阶段。取值顺序与 ShaderConductor::ShaderStage 一致
    enum class HLSLStage : uint32_t
    {
        kVertex = 0,
        kPixel,
        kGeometry,
        kHull,
        kDomain,
        kCompute,
    };

    /// HLSLStage 的枚举项个数
    constexpr uint32_t kHLSLStageCount = 6;

    /// 输出目标语言。不含 DXIL
    enum class HLSLTarget : uint32_t
    {
        kSpirV = 0,
        kHlsl,
        kGlsl,
        kEssl,
        kMslMacOS,
        kMslIOS,
    };

    /// 宏定义。value 原样传给 DXC：空串得到 "NAME="（已定义、值为空），与 ShaderConductor 一致
    struct HLSLMacroDefine
    {
        String  name;
        String  value;
    };

    struct HLSLCrossSource
    {
        /// HLSL 源码（完整文本，非路径）
        String                      source;
        /// 源文件路径。用于 #include 的相对解析与编译错误定位
        String                      fileName;
        /// 入口函数名，空串按 "main" 处理
        String                      entryPoint;
        HLSLStage                   stage           {HLSLStage::kVertex};
        TArray<HLSLMacroDefine>     defines;
        /// #include 搜索路径
        TArray<String>              includeDirs;
    };

    struct HLSLCrossOptions
    {
        /// 沿用 ShaderConductor（2019）的语义：false 传 -Zpr，true 传 -Zpc。
        /// HLSL 矩阵在 SPIR-V 里是转置表示的，所以 DXC 开关与字段名看上去相反
        bool        packMatricesInRowMajor  {false};
        /// 输出调试信息（-Zi）
        bool        enableDebugInfo         {false};
        /// 0 到 3，映射为 -O0 ~ -O3
        uint32_t    optimizationLevel       {3};
        /// 启用真 16 位标量类型（-enable-16bit-types），要求 shader model >= 6.2。
        /// 打开后 half 与 min16* 的语义会变，见 doc/todo/ShaderConductor-Replacement-todo.md §6.1.5
        bool        enable16bitTypes        {false};
        /// HLSL 前端 profile 的 shader model，与 HLSLCrossTarget::version 是两个独立概念
        uint8_t     shaderModelMajor        {6};
        uint8_t     shaderModelMinor        {0};
    };

    struct HLSLCrossTarget
    {
        HLSLTarget  language {HLSLTarget::kSpirV};
        /// 后端方言版本号字符串，空串表示用 SPIRV-Cross 默认值：
        ///   GLSL "330"/"450"  ESSL "300"/"310"  HLSL "50"（= SM 5.0）  MSL 直接作为 msl_version
        ///   SpirV 忽略此字段
        String      version;
    };

    struct HLSLCrossResult
    {
        /// 编译是否失败。message 非空不代表失败，可能只是警告
        bool                hasError {false};
        /// target 是文本还是二进制。仅 kSpirV 为 false
        bool                isText   {true};
        /// 编译产物
        TArray<uint8_t>     target;
        /// 错误与警告合并后的诊断文本
        String              message;

        String toString() const
        {
            return String(reinterpret_cast<const char*>(target.data()), target.size());
        }
    };

    class T3D_HLSLCROSS_API HLSLCrossCompiler
    {
    public:
        /**
         * @brief   把 HLSL 交叉编译到目标语言
         * @param [in] source : 源码与编译上下文
         * @param [in] options : 前端选项
         * @param [in] target : 目标语言与方言版本
         * @return  编译结果。检查 hasError 判成败，message 恒需检查并打印
         */
        static HLSLCrossResult compile(const HLSLCrossSource &source,
                                       const HLSLCrossOptions &options,
                                       const HLSLCrossTarget &target);

        /**
         * @brief   探测后端是否可用（dxcompiler 动态库能否加载）
         * @param [out] message : 不可用时的原因描述
         * @return  true 表示可用
         */
        static bool isAvailable(String &message);
    };
}


#endif  /*__T3D_HLSL_CROSS_COMPILER_H__*/
