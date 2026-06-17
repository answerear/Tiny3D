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
                // Some pragmas like skip_variants may have values, require may not
                if (pragma.option != "skip_variants" && pragma.option != "require")
                    continue;
            }

            // Helper lambda to parse keyword type/scope/stage from pragma option string
            auto parseKeywordPragma = [&](const String& option) -> bool
            {
                KeywordType type = KeywordType::kMultiCompile;
                KeywordScope scope = KeywordScope::kGlobal;
                KeywordStage stage = KeywordStage::kAll;

                String opt = option;

                // Determine type
                if (opt.find("shader_feature") == 0)
                {
                    type = KeywordType::kShaderFeature;
                    opt = opt.substr(14); // strlen("shader_feature")
                }
                else if (opt.find("multi_compile") == 0)
                {
                    type = KeywordType::kMultiCompile;
                    opt = opt.substr(13); // strlen("multi_compile")
                }
                else if (opt.find("dynamic_branch") == 0)
                {
                    type = KeywordType::kDynamicBranch;
                    opt = opt.substr(14); // strlen("dynamic_branch")
                }
                else
                {
                    return false;
                }

                // Determine scope (local suffix)
                if (opt.find("_local") == 0)
                {
                    scope = KeywordScope::kLocal;
                    opt = opt.substr(6); // strlen("_local")
                }

                // Determine stage-specific suffix
                if (opt.empty() || opt == "")
                {
                    stage = KeywordStage::kAll;
                }
                else if (opt == "_vertex")
                {
                    stage = KeywordStage::kVertex;
                }
                else if (opt == "_fragment")
                {
                    stage = KeywordStage::kFragment;
                }
                else if (opt == "_hull")
                {
                    stage = KeywordStage::kHull;
                }
                else if (opt == "_domain")
                {
                    stage = KeywordStage::kDomain;
                }
                else if (opt == "_geometry")
                {
                    stage = KeywordStage::kGeometry;
                }
                else if (opt == "_raytracing")
                {
                    stage = KeywordStage::kRaytracing;
                }
                else
                {
                    // Could be a builtin shortcut like _fwdbase, _fwdadd, _shadowcaster etc.
                    // Handle builtin shortcuts
                    if (type == KeywordType::kMultiCompile)
                    {
                        if (opt == "_fwdbase")
                        {
                            TArray<String> kws = {"DIRECTIONAL", "LIGHTMAP_ON", "DIRLIGHTMAP_COMBINED", "DYNAMICLIGHTMAP_ON", "SHADOWS_SCREEN", "SHADOWS_SHADOWMASK", "LIGHTMAP_SHADOW_MIXING", "LIGHTPROBE_SH"};
                            keywords.emplace_back(kws, type, scope, stage);
                            return true;
                        }
                        else if (opt == "_fwdadd")
                        {
                            TArray<String> kws = {"POINT", "DIRECTIONAL", "SPOT", "POINT_COOKIE", "DIRECTIONAL_COOKIE"};
                            keywords.emplace_back(kws, type, scope, stage);
                            return true;
                        }
                        else if (opt == "_fwdadd_fullshadows")
                        {
                            TArray<String> kws = {"POINT", "DIRECTIONAL", "SPOT", "POINT_COOKIE", "DIRECTIONAL_COOKIE", "SHADOWS_DEPTH", "SHADOWS_SCREEN", "SHADOWS_CUBE", "SHADOWS_SOFT", "SHADOWS_SHADOWMASK", "LIGHTMAP_SHADOW_MIXING"};
                            keywords.emplace_back(kws, type, scope, stage);
                            return true;
                        }
                        else if (opt == "_shadowcaster")
                        {
                            TArray<String> kws = {"SHADOWS_DEPTH", "SHADOWS_CUBE"};
                            keywords.emplace_back(kws, type, scope, stage);
                            return true;
                        }
                        else if (opt == "_fog")
                        {
                            TArray<String> kws = {"FOG_LINEAR", "FOG_EXP", "FOG_EXP2"};
                            keywords.emplace_back(kws, type, scope, stage);
                            return true;
                        }
                        else if (opt == "_instancing")
                        {
                            TArray<String> kws = {"INSTANCING_ON", "PROCEDURAL_ON"};
                            keywords.emplace_back(kws, type, scope, stage);
                            return true;
                        }
                        else if (opt == "_particles")
                        {
                            TArray<String> kws = {"SOFTPARTICLES_ON"};
                            keywords.emplace_back(kws, type, scope, stage);
                            return true;
                        }
                    }
                    // Unknown suffix, treat as generic
                    stage = KeywordStage::kAll;
                }

                keywords.emplace_back(pragma.values, type, scope, stage);
                return true;
            };

            if (parseKeywordPragma(pragma.option))
            {
                // Handled as keyword pragma
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
            else if (pragma.option == "skip_variants")
            {
                for (const auto& v : pragma.values)
                {
                    skipVariants.push_back(v);
                }
            }
            else if (pragma.option == "require")
            {
                for (const auto& v : pragma.values)
                {
                    requires.push_back(v);
                }
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
        ShaderCompilerPtr compiler = T3D_NEW ShaderCompiler();
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
    bool ShaderCompiler::compile(const String &code, PassPtr pass, const String &inputPath, const String &outputDir, const Args &args)
    {
        bool ret = true;

        mArgs = args;
        mInputPath = inputPath;
        mOutputDir = outputDir;

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

        // for (const ShaderSnippet &snippet : snippets)
        for (const auto &s : snippets)
        {
            SCC_LOG_INFO("Begin compiling shader variant [%s - %s] ...", s.first.stage.c_str(), s.first.defines.c_str());
            ret = ret && compileShaderSnippet(s.second, pass);
            SCC_LOG_INFO("Completed compiling shader variant ret = %d", ret);
        }
        
        return ret;
    }

#endif

    //--------------------------------------------------------------------------

    bool ShaderCompiler::compile(const String &code, const String &inputPath, const String &outputDir, const Args &args)
    {
        bool ret = true;

        mArgs = args;
        mInputPath = inputPath;
        mOutputDir = outputDir;

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

        // for (const ShaderSnippet &snippet : snippets)
        for (const auto &s : snippets)
        {
            SCC_LOG_INFO("Begin compiling shader variant [%s - %s] ...", s.first.stage.c_str(), s.first.defines.c_str());
            ret = ret && compileShaderSnippet(s.second);
            SCC_LOG_INFO("Completed compiling shader variant ret = %d", ret);
        }
        
        return ret;
    }

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
                results.emplace_back(result);
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
            String key = "";
            for (int32_t defineIndex = 0; defineIndex < defines.size(); ++defineIndex)
            {
                defines[defineIndex].name = variants[variantIndex][defineIndex].c_str();
                if (defines[defineIndex].name != "_")
                {
                    if (defineIndex > 0 && key != "")
                    {
                        key += "-";
                    }
                    key += defines[defineIndex].name;
                }
            }

            for (int32_t programIndex = 0; programIndex < kStageCount; ++programIndex)
            {
                const String& stage = kStages[programIndex];
                if (!params.hasProgram(stage))
                {
                    continue;
                }

                SnippetKey snippetKey;
                snippetKey.defines = key;
                snippetKey.stage = stage;
                const auto itr = snippets.find(snippetKey);
                if (itr != snippets.end())
                {
                    continue;
                }
                
                ShaderSnippet snippet(source);
                snippet.entry = params.entriesName.at(stage);
                snippet.defines = defines;
                snippet.paramsMap = params.paramsMap;
                snippet.stage = stage;
                snippet.model = params.shaderModel;
                // snippets.push_back(snippet);
                snippets.emplace(snippetKey, snippet);
            }
        }
    }

    //--------------------------------------------------------------------------

    bool ShaderCompiler::compileShaderSnippet(const ShaderSnippet &snippet, PassPtr pass)
    {
        return compileShaderSnippet(snippet, [&pass](const String &content, ShaderKeyword &&keyword, SHADER_STAGE shaderType)
            {
                ShaderVariantPtr shaderVariant = ShaderVariant::create(std::move(keyword), content);
                shaderVariant->setShaderStage(shaderType);
                pass->addShaderVariant(shaderVariant->getShaderKeyword(), shaderVariant);
            });
    }

    //--------------------------------------------------------------------------

    bool ShaderCompiler::compileShaderSnippet(const ShaderSnippet &snippet)
    {
        return compileShaderSnippet(snippet, [this, &snippet](const String &content, ShaderKeyword &&keyword, SHADER_STAGE shaderType)
            {
                do
                {
                    String outputPath = mOutputDir + Dir::getNativeSeparator() + mArgs.baseName;
                    if (!keyword.getKeys().empty())
                    {
                        outputPath = outputPath + "_" + keyword.getName() + "_" + snippet.stage + "." + mArgs.target;
                    }
                    else
                    {
                        outputPath = outputPath + "_" + snippet.stage + "." + mArgs.target;
                    }
                    
                    FileDataStream fs;
                    if (!fs.open(outputPath.c_str(), FileDataStream::EOpenMode::E_MODE_TRUNCATE|FileDataStream::EOpenMode::E_MODE_WRITE_ONLY))
                    {
                        SCC_LOG_ERROR("Failed to open file (%s) !", outputPath.c_str());
                        break;
                    }

                    fs.write((void*)content.data(), content.size());
                    fs.close();

                } while (false);
            });
    }

    //--------------------------------------------------------------------------

    bool ShaderCompiler::compileShaderSnippet(const ShaderSnippet &snippet, const CompilePostProcessor &postProcessor)
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

            size_t totalDefines = snippet.defines.size() + mArgs.defines.size();
            ShaderConductor::MacroDefine* defines = T3D_NEW ShaderConductor::MacroDefine[totalDefines];
            ShaderKeyword keyword;
            generateDefinesAndPath(snippet, defines, keyword);
            keyword.generate();

            // keyword 生成后，再追加命令行 -D 宏到 defines 数组（不影响 keyword 和文件名）
            for (size_t i = 0; i < mArgs.defines.size(); i++)
            {
                size_t idx = snippet.defines.size() + i;
                defines[idx].name = mArgs.defines[i].name.c_str();
                defines[idx].value = mArgs.defines[i].value.c_str();
            }

            SHADER_STAGE shaderType;
            sourceDesc.source = snippet.source.c_str();
            sourceDesc.stage = getShaderStage(snippet.stage, shaderType);
            sourceDesc.entryPoint = snippet.entry.c_str();
            sourceDesc.fileName = mInputPath.c_str();
            sourceDesc.defines = defines;
            sourceDesc.numDefines = totalDefines;
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

            targetDesc.language = getShadingLanguage(mArgs.target);

            // For GLSL/ESSL targets, convert HLSL shader model version to
            // the corresponding GLSL version string that SPIRV-Cross expects
            // (e.g. "40" -> "400", "50" -> "450", "30" -> "330").
            auto convertToGLSLVersion = [](const String &model) -> String
            {
                static const TMap<String, String> kModelToGLSL = {
                    {"20", "110"}, {"21", "120"}, {"30", "130"},
                    {"31", "140"}, {"32", "150"}, {"33", "330"},
                    {"40", "400"}, {"41", "410"}, {"42", "420"},
                    {"43", "430"}, {"44", "440"}, {"45", "450"},
                    {"50", "450"}, {"51", "450"}, {"60", "460"},
                    {"61", "460"}, {"62", "460"}, {"63", "460"},
                };
                auto it = kModelToGLSL.find(model);
                if (it != kModelToGLSL.end())
                    return it->second;
                return "450";
            };

            auto convertToESSLVersion = [](const String &model) -> String
            {
                static const TMap<String, String> kModelToESSL = {
                    {"20", "100"}, {"21", "100"}, {"30", "300"},
                    {"31", "300"}, {"32", "300"}, {"33", "300"},
                    {"40", "310"}, {"41", "310"}, {"42", "310"},
                    {"43", "310"}, {"44", "320"}, {"45", "320"},
                    {"50", "320"}, {"51", "320"}, {"60", "320"},
                    {"61", "320"}, {"62", "320"}, {"63", "320"},
                };
                auto it = kModelToESSL.find(model);
                if (it != kModelToESSL.end())
                    return it->second;
                return "310";
            };

            String glslVersion;
            if (targetDesc.language == ShadingLanguage::Essl)
            {
                glslVersion = convertToESSLVersion(snippet.model);
                targetDesc.version = glslVersion.c_str();
            }
            else if (targetDesc.language == ShadingLanguage::Glsl)
            {
                glslVersion = convertToGLSLVersion(snippet.model);
                targetDesc.version = glslVersion.c_str();
            }
            else
            {
                targetDesc.version = snippet.model.c_str();
            }

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
                if (postProcessor != nullptr)
                {
                    postProcessor(content, std::move(keyword), shaderType);
                }
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

    //--------------------------------------------------------------------------
}

