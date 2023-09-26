/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef __T3D_SHADER_COMPILER_H__
#define __T3D_SHADER_COMPILER_H__


#include "T3DSCCPrerequisites.h"
// #include "ShaderScriptObject.pb.h"


namespace Tiny3D
{
    class ShaderCompiler;

    T3D_DECLARE_SMART_PTR(ShaderCompiler);

    class ShaderCompiler : public Object
    {
    public:
        static ShaderCompilerPtr create();

        virtual ~ShaderCompiler();

        // bool compile(Script::ShaderSystem::Shader *source, const String &inputPath, const String &outputDir, const Args args);

        bool compile(const String &code, PassPtr pass, const String& inputPath, const String& outputDir, const Args& args);
        
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

        void fixSpirVCrossForHLSLSemantics(String& content);

    protected:
        Args    mArgs;
        String  mInputPath;
        String  mOutputDir;
    };
}


#endif  /*__T3D_SHADER_COMPILER_H__*/
