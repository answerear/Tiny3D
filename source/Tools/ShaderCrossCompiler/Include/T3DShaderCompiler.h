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

#ifndef __T3D_SHADER_COMPILER_H__
#define __T3D_SHADER_COMPILER_H__


#include "T3DSCCPrerequisites.h"
// #include "ShaderScriptObject.pb.h"


namespace Tiny3D
{
    class ShaderCompiler;

    T3D_DECLARE_SMART_PTR(ShaderCompiler);

    struct Args
    {
        enum Options
        {
            /// 输出版本号
            OPT_PRINT_VERSION = (1 << 0),
            /// 输出帮助信息
            OPT_PRINT_HELP = (1 << 1),
            /// 转换到目标平台着色器语言时是否输出调试信息
            OPT_ENABLE_DEBUG_INFO = (1 << 2),
            /// 是否输出二进制文件，还是 json 文件
            OPT_BINARY_FILE = (1 << 3),
            /// 输入的是否 native shader，也就是是原生的 hlsl、glsl 之类的，而非 shader lab
            OPT_NATIVE_SHADER = (1 << 4),
        };

        bool hasOptions(uint32_t flags) const
        {
            return (flags & options) == flags;
        }

        /// 选项
        uint32_t    options {0};
        /// 优化级别，0 to 3
        uint32_t    optimizeLevel {3};
        /// 包含的头文件目录路径
        String      include {};
        /// 输出目标平台
        String      target {};
        /// 输出文件的基本名称
        String      baseName {};
        /// 生成 Shader 的 UUID
        UUID        uuid {UUID::INVALID};
    };
    
    class ShaderCompiler : public Object
    {
    public:
        static ShaderCompilerPtr create();

        virtual ~ShaderCompiler();

        // bool compile(Script::ShaderSystem::Shader *source, const String &inputPath, const String &outputDir, const Args args);

        bool compile(const String &code, PassPtr pass, const String& inputPath, const String& outputDir, const Args& args);

        bool compile(const String &code, const String &inputPath, const String &outputDir, const Args& args);
        
    protected:
        static const String kVertex;
        static const String kFragment;
        static const String kCompute;
        static const String kGeometry;
        static const String kHull;
        static const String kDomain;

        static const uint32_t kVertexShader;
        static const uint32_t kFragmentShader;
        static const uint32_t kGeometryShader;
        static const uint32_t kComputeShader;
        static const uint32_t kHullShader;
        static const uint32_t kDomainShader;
        static const uint32_t kStageCount;

        struct PragmaParam
        {
            String          option;
            TArray<String>  values;
        };

        typedef TMap<String, PragmaParam> PragmaParamsMap;

        struct MultiCompileParam
        {
            MultiCompileParam(const TArray<String>& inKeywords)
                : keywords(inKeywords)
            {

            }

            TArray<String>	keywords;
        };

        typedef TMap<String, String> EntryMap;

        struct ProgramParameters
        {
            ProgramParameters()
                : shaderModel("20")
            {

            }

            void setPragmaParams(const TArray<PragmaParam>& params);

            bool hasProgram() const
            {
                return !entriesName.empty();
            }

            bool hasProgram(const String& p) const
            {
                return entriesName.find(p) != entriesName.end();
            }

            String                      shaderModel;
            TArray<MultiCompileParam>   keywords;
            PragmaParamsMap             paramsMap;
            EntryMap                    entriesName;
        };

        struct MacroDefine
        {
            String  name;
            String  value;
        };

        typedef TArray<MacroDefine> MacroDefines;

        struct ShaderSnippet
        {
            ShaderSnippet(const String& src) 
                : source(src)
            {

            }

            const String&       source;
            String              entry;
            String              stage;
            String              model;
            uint32_t            language;
            MacroDefines        defines;
            PragmaParamsMap     paramsMap;
        };

        typedef TArray<ShaderSnippet> ShaderSnippets;

        ShaderCompiler();

        // bool compilePass(const Script::ShaderSystem::Pass& pass);

        bool parsePragmaArgs(const String& str, const String& pragma, TArray<PragmaParam>& outParams);

        void enumerateKeywords(const ProgramParameters& params, int32_t depth, StringArray& result, TArray<StringArray>& results);

        void generateShaderSnippets(const String& source, const ProgramParameters& params, ShaderSnippets& snippets);

        bool compileShaderSnippet(const ShaderSnippet &snippet, PassPtr pass);

        bool compileShaderSnippet(const ShaderSnippet &snippet);

        using CompilePostProcessor = TFunction<void(const String&, ShaderKeyword &&, SHADER_STAGE)>;
        
        bool compileShaderSnippet(const ShaderSnippet &snippet, const CompilePostProcessor &postProcessor);

        void fixSpirVCrossForHLSLSemantics(String& content);

    protected:
        Args    mArgs;
        String  mInputPath;
        String  mOutputDir;
    };
}


#endif  /*__T3D_SHADER_COMPILER_H__*/
