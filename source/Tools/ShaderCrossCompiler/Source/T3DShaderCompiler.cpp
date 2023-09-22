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


#include "T3DShaderCompiler.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    void ShaderCompiler::ProgramParameters::setPragmaParams(
        const TArray<PragmaParam>& params)
    {
        for (size_t i = 0; i < params.size(); ++i)
        {
            const PragmaParam& pragma = params[i];
            if (pragma.values.size() < 1)
            {
                continue;
            }

            if (pragma.option == "multi_compile")
            {
                keywords.push_back(pragma.values);
            }
            else if (pragma.option == "vertex")
            {
                entriesName[kVertex] = pragma.values[0];
            }
            else if (pragma.option == "fragment")
            {
                entriesName[kFragment] = pragma.values[0];
            }
            else if (pragma.option == "compute")
            {
                entriesName[kCompute] = pragma.values[0];
            }
            else if (pragma.option == "geometry")
            {
                entriesName[kGeometry] = pragma.values[0];
            }
            else if (pragma.option == "hull")
            {
                entriesName[kHull] = pragma.values[0];
            }
            else if (pragma.option == "domain")
            {
                entriesName[kDomain] = pragma.values[0];
            }
            else if (pragma.option == "target")
            {
                shaderModel = pragma.values[0];
                StringUtil::replaceAll(shaderModel, ".", "");
            }
            else
            {
                paramsMap[pragma.option] = pragma;
            }
        }
    }

    //--------------------------------------------------------------------------

    const String ShaderCompiler::kVertex = "vertex";
    const String ShaderCompiler::kFragment = "fragment";
    const String ShaderCompiler::kCompute = "compute";
    const String ShaderCompiler::kGeometry = "geometry";
    const String ShaderCompiler::kHull = "hull";
    const String ShaderCompiler::kDomain = "domain";

    const uint32_t ShaderCompiler::kVertexShader = (uint32_t)ShaderConductor::ShaderStage::VertexShader;
    const uint32_t ShaderCompiler::kFragmentShader = (uint32_t)ShaderConductor::ShaderStage::PixelShader;
    const uint32_t ShaderCompiler::kComputeShader = (uint32_t)ShaderConductor::ShaderStage::ComputeShader;
    const uint32_t ShaderCompiler::kGeometryShader = (uint32_t)ShaderConductor::ShaderStage::GeometryShader;
    const uint32_t ShaderCompiler::kHullShader = (uint32_t)ShaderConductor::ShaderStage::HullShader;
    const uint32_t ShaderCompiler::kDomainShader = (uint32_t)ShaderConductor::ShaderStage::DomainShader;

    const uint32_t ShaderCompiler::kStageCount = (uint32_t)ShaderConductor::ShaderStage::NumShaderStages;

    //--------------------------------------------------------------------------

    ShaderCompilerPtr ShaderCompiler::create()
    {
        ShaderCompilerPtr compiler = new ShaderCompiler();
        // compiler->release();
        return compiler;
    }

    //--------------------------------------------------------------------------

    ShaderCompiler::ShaderCompiler()
    {

    }

    //--------------------------------------------------------------------------

    ShaderCompiler::~ShaderCompiler()
    {

    }

    //--------------------------------------------------------------------------

#if 0
    bool ShaderCompiler::compile(Script::ShaderSystem::Shader* source, 
        const String &inputPath, const String &outputDir, const Args args)
    {
        bool ret = true;

        mArgs = args;
        mInputPath = inputPath;
        mOutputDir = outputDir;

        for (int32_t i = 0; i < source->subshaders_size(); i++)
        {
            auto subshader = source->subshaders(i);

            for (int32_t j = 0; j < subshader.passes_size(); j++)
            {
                auto pass = subshader.passes(j);
                ret = ret && compilePass(pass);
            }
        }

        return ret;
    }
#else
    bool ShaderCompiler::compile(const String &code, PassPtr pass)
    {
        bool ret = true;

        // shader code
        const String &source = code;

        // parse pragma
        TArray<PragmaParam> pragmaParams;
        parsePragmaArgs(source, "#pragma ", pragmaParams);

        // program params
        ProgramParameters programParams;
        programParams.setPragmaParams(pragmaParams);

        // generate snippet
        ShaderSnippets snippets;
        generateShaderSnippets(source, programParams, snippets);

        // String outputPath = mOutputDir + Dir::getNativeSeparator() + mArgs.baseName;

        for (size_t i = 0; i < snippets.size(); i++)
        {
            ShaderSnippet snippet = snippets[i];
            ret = ret && compileShaderSnippet(snippet, pass);
        }
        
        return ret;
    }

#endif

    //--------------------------------------------------------------------------

#if 0
    bool ShaderCompiler::compilePass(const Script::ShaderSystem::Pass& pass)
    {
        bool ret = true;

        if (pass.has_program())
        {
            // shader code
            auto type = pass.program().source().type();
            const String &source = pass.program().source().code();

            // parse pragma
            TArray<PragmaParam> pragmaParams;
            parsePragmaArgs(source, "#pragma ", pragmaParams);

            // program params
            ProgramParameters programParams;
            programParams.setPragmaParams(pragmaParams);

            // generate snippet
            ShaderSnippets snippets;
            generateShaderSnippets(source, programParams, snippets);

            String outputPath = mOutputDir + Dir::getNativeSeparator() + mArgs.baseName;

            for (size_t i = 0; i < snippets.size(); i++)
            {
                ShaderSnippet snippet = snippets[i];
                ret = ret && compileShaderSnippet(snippet, outputPath);
            }
        }

        return ret;
    }
#endif

    //--------------------------------------------------------------------------

    bool ShaderCompiler::parsePragmaArgs(const String& str, const String& pragma, TArray<PragmaParam>& outParams)
    {
        size_t pos = str.find(pragma, 0);
        while (pos != String::npos)
        {
            pos += pragma.length();
            size_t lineEnd = StringUtil::findLine(str, pos);
            size_t comment = 0;
            if (comment < lineEnd)
            {
                comment = str.find("//", pos);
                if (comment < lineEnd)
                    lineEnd = comment;
            }

            size_t start = pos;

            outParams.emplace_back();
            PragmaParam& params = outParams.back();

            while (start < lineEnd)
            {
                start = str.find_first_not_of(" ", start);
                size_t end = str.find_first_of(" ", start);
                if (end >= lineEnd)
                    end = lineEnd;
                String token = str.substr(start, end-start);
                if (token.empty())
                {
                    break;
                }
                params.values.push_back(token);
                start = end + 1;
            }

            params.option = params.values[0];
            params.values.erase(params.values.begin());

            pos = str.find(pragma, lineEnd+1);
        }

        return true;
    }

    //--------------------------------------------------------------------------

    void ShaderCompiler::enumerateKeywords(const ProgramParameters& params, 
        int32_t depth, StringArray& result, TArray<StringArray>& results)
    {
        if (params.keywords.size() == 0)
        {
            results.emplace_back();
            return;
        }

        for (int32_t i = 0; i < params.keywords[depth].keywords.size(); ++i)
        {
            result[depth] = params.keywords[depth].keywords[i];
            if (depth != params.keywords.size() - 1)
            {
                enumerateKeywords(params, depth + 1, result, results);
            }
            else
            {
                results.push_back(result);
            }
        }
    }

    //--------------------------------------------------------------------------

    void ShaderCompiler::generateShaderSnippets(const String& source, 
        const ProgramParameters& params, ShaderSnippets& snippets)
    {
        // variants
        std::vector<std::string> temp;
        temp.resize(params.keywords.size());
        std::vector<std::vector<std::string>> variants;
        enumerateKeywords(params, 0, temp, variants);

        static const String kStages[] =
        {
            kVertex, kFragment, kGeometry, kHull, kDomain, kCompute
        };

        // snippets
        for (int32_t variantIndex = 0; variantIndex < variants.size(); ++variantIndex)
        {
            std::vector<MacroDefine> defines;
            defines.resize(variants[variantIndex].size());
            for (int32_t defineIndex = 0; defineIndex < defines.size(); ++defineIndex)
            {
                defines[defineIndex].name = variants[variantIndex][defineIndex].c_str();
            }

            for (int32_t programIndex = 0; programIndex < kStageCount; ++programIndex)
            {
                const String& stage = kStages[programIndex];
                if (!params.hasProgram(stage))
                {
                    continue;
                }

                ShaderSnippet snippet(source);
                snippet.entry = params.entriesName.at(stage);
                snippet.defines = defines;
                snippet.paramsMap = params.paramsMap;
                snippet.stage = stage;
                snippet.model = params.shaderModel;
                snippets.push_back(snippet);
            }
        }
    }

    //--------------------------------------------------------------------------

    bool ShaderCompiler::compileShaderSnippet(const ShaderSnippet &snippet, PassPtr pass)
    {
        bool ret = true;

        do 
        {
            using namespace ShaderConductor;

            Compiler::SourceDesc sourceDesc{};
            Compiler::TargetDesc targetDesc{};

            auto getShaderStage = [](const String &stage, SHADER_STAGE &type) -> ShaderStage
            {
                if (stage == kVertex)
                {
                    type = SHADER_STAGE::kVertex;
                    return ShaderStage::VertexShader;
                }
                else if (stage == kFragment)
                {
                    type = SHADER_STAGE::kPixel;
                    return ShaderStage::PixelShader;
                }
                else if (stage == kGeometry)
                {
                    type = SHADER_STAGE::kGeometry;
                    return ShaderStage::GeometryShader;
                }
                else if (stage == kHull)
                {
                    type = SHADER_STAGE::kHull;
                    return ShaderStage::HullShader;
                }
                else if (stage == kDomain)
                {
                    type = SHADER_STAGE::kDomain;
                    return ShaderStage::DomainShader;
                }
                else if (stage == kCompute)
                {
                    type = SHADER_STAGE::kCompute;
                    return ShaderStage::ComputeShader;
                } 
                else
                {
                    type = SHADER_STAGE::kVertex;
                    return ShaderStage::VertexShader;
                }
            };

            // String path;

            auto generateDefinesAndPath = [](const ShaderSnippet& snippet, ShaderConductor::MacroDefine* defines, ShaderKeyword &keyword)
            {
                // String name;
                for (size_t i = 0; i < snippet.defines.size(); i++)
                {
                    const MacroDefine& define = snippet.defines[i];
                    defines[i].name = define.name.c_str();
                    defines[i].value = define.value.c_str();
                    keyword.addKeyword(defines[i].name);
                    // name = name + "_" + define.name;
                }

                // path = outPath + name;
            };

            ShaderConductor::MacroDefine* defines = new ShaderConductor::MacroDefine[snippet.defines.size()];
            ShaderKeyword keyword;
            generateDefinesAndPath(snippet, defines, keyword);
            keyword.generate();

            SHADER_STAGE shaderType;
            sourceDesc.source = snippet.source.c_str();
            sourceDesc.stage = getShaderStage(snippet.stage, shaderType);
            sourceDesc.entryPoint = snippet.entry.c_str();
            sourceDesc.fileName = mInputPath.c_str();
            sourceDesc.defines = defines;
            sourceDesc.numDefines = snippet.defines.size();
            //sourceDesc.loadIncludeCallback = nullptr;

            auto getShadingLanguage = [](const String& str) -> ShadingLanguage
            {
                if (str == "glsl")
                    return ShadingLanguage::Glsl;
                else if (str == "hlsl")
                    return ShadingLanguage::Hlsl;
                else if (str == "essl")
                    return ShadingLanguage::Essl;
                else if (str == "dxil")
                    return ShadingLanguage::Dxil;
                else if (str == "spirv")
                    return ShadingLanguage::SpirV;
                else if (str == "msl_macos")
                    return ShadingLanguage::Msl_macOS;
                else if (str == "msl_ios")
                    return ShadingLanguage::Msl_iOS;
                else
                    return ShadingLanguage::Hlsl;
            };

            targetDesc.version = snippet.model.c_str();
            targetDesc.language = getShadingLanguage(mArgs.target);

            Compiler::Options opt;
            opt.packMatricesInRowMajor = false;
            opt.optimizationLevel = mArgs.optimizeLevel;
            opt.enableDebugInfo = mArgs.hasOptions(Args::OPT_ENABLE_DEBUG_INFO);

            const auto result = Compiler::Compile(sourceDesc, opt, targetDesc);

            if (result.errorWarningMsg != nullptr)
            {
                const char* msg = reinterpret_cast<const char*>(result.errorWarningMsg->Data());
                SCC_LOG_ERROR("Error or warning from shader compiler: %s", String(msg, msg + result.errorWarningMsg->Size()).c_str());
                DestroyBlob(result.errorWarningMsg);
                DestroyBlob(result.target);
                T3D_SAFE_DELETE_ARRAY(defines);
                ret = false;
                break;
            }

            if (result.target != nullptr)
            {
                String content((const char*)result.target->Data(), result.target->Size());
                if (targetDesc.language == ShadingLanguage::Hlsl)
                {
                    // ShaderConductor 有 bug，没有把 hlsl 的 Semantic 记录下来 写回去，
                    // 所以这里做一次替换，以修复转出来的 hlsl 错误的 Semantic 修饰
                    fixSpirVCrossForHLSLSemantics(content);
                }
                ShaderVariantPtr shaderVariant = ShaderVariant::create(std::move(keyword), content);
                shaderVariant->setShaderStage(shaderType);
                pass->addShaderVariant(shaderVariant->getShaderKeyword(), shaderVariant);
            }

            DestroyBlob(result.errorWarningMsg);
            DestroyBlob(result.target);
            T3D_SAFE_DELETE_ARRAY(defines);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void ShaderCompiler::fixSpirVCrossForHLSLSemantics(String& content)
    {
        String::size_type p0 = 0;
        while (1)
        {
            String::size_type startPos = content.find(" : TEXCOORD", p0);
            if (startPos == String::npos)
                break;

            String::size_type endPos = content.find_first_of(';', startPos);
            if (endPos == String::npos)
                break;

            String::size_type p1 = content.rfind('_', startPos);
            if (p1 == String::npos)
                break;

            String semantic = content.substr(p1 + 1, startPos - p1 - 1);
            content.replace(startPos + 3, endPos - startPos - 3, semantic);
            p0 = endPos + 1;
        }
    }
}

