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

    const uint32_t ShaderCompiler::kVertexShader = (uint32_t)HLSLStage::kVertex;
    const uint32_t ShaderCompiler::kFragmentShader = (uint32_t)HLSLStage::kPixel;
    const uint32_t ShaderCompiler::kComputeShader = (uint32_t)HLSLStage::kCompute;
    const uint32_t ShaderCompiler::kGeometryShader = (uint32_t)HLSLStage::kGeometry;
    const uint32_t ShaderCompiler::kHullShader = (uint32_t)HLSLStage::kHull;
    const uint32_t ShaderCompiler::kDomainShader = (uint32_t)HLSLStage::kDomain;

    const uint32_t ShaderCompiler::kStageCount = kHLSLStageCount;

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

    SHADER_LANGUAGE ShaderCompiler::toShaderLanguage(const String &target)
    {
        if (target == "hlsl" || target == "dxil")
        {
            return SHADER_LANGUAGE::kHLSL;
        }
        if (target == "glsl")
        {
            return SHADER_LANGUAGE::kGLSL;
        }
        if (target == "essl")
        {
            return SHADER_LANGUAGE::kESSL;
        }
        if (target == "spirv")
        {
            return SHADER_LANGUAGE::kSPIRV;
        }
        if (target == "msl" || target == "msl_macos" || target == "msl_ios")
        {
            return SHADER_LANGUAGE::kMSL;
        }
        return SHADER_LANGUAGE::kUnknown;
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

        // generate snippet（keyword × stage 枚举与目标语言无关，只需生成一次）
        ShaderSnippets snippets;
        generateShaderSnippets(source, programParams, snippets);

        // String outputPath = mOutputDir + Dir::getNativeSeparator() + mArgs.baseName;

        // 对每个目标语言各跑一遍 cross-compile，把各语言变体合并进同一个 pass
        for (const String &target : mArgs.targets)
        {
            mCurrentTarget = target;
            SCC_LOG_INFO("Begin compiling for target language [%s] ...", target.c_str());

            // for (const ShaderSnippet &snippet : snippets)
            for (const auto &s : snippets)
            {
                SCC_LOG_INFO("Begin compiling shader variant [%s - %s] ...", s.first.stage.c_str(), s.first.defines.c_str());
                ret = ret && compileShaderSnippet(s.second, pass);
                SCC_LOG_INFO("Completed compiling shader variant ret = %d", ret);
            }
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

        // generate snippet（keyword × stage 枚举与目标语言无关，只需生成一次）
        ShaderSnippets snippets;
        generateShaderSnippets(source, programParams, snippets);

        // String outputPath = mOutputDir + Dir::getNativeSeparator() + mArgs.baseName;

        // 对每个目标语言各跑一遍 cross-compile，分别输出散点调试文件
        for (const String &target : mArgs.targets)
        {
            mCurrentTarget = target;
            SCC_LOG_INFO("Begin compiling for target language [%s] ...", target.c_str());

            // for (const ShaderSnippet &snippet : snippets)
            for (const auto &s : snippets)
            {
                SCC_LOG_INFO("Begin compiling shader variant [%s - %s] ...", s.first.stage.c_str(), s.first.defines.c_str());
                ret = ret && compileShaderSnippet(s.second);
                SCC_LOG_INFO("Completed compiling shader variant ret = %d", ret);
            }
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
        return compileShaderSnippet(snippet, [this, &pass](const String &content, ShaderKeyword &&keyword, SHADER_STAGE shaderType)
            {
                ShaderVariantPtr shaderVariant = ShaderVariant::create(std::move(keyword), content);
                shaderVariant->setShaderStage(shaderType);
                // 标注当前目标语言，addShaderVariant 内部按语言合并进 ShaderVariantSet
                shaderVariant->setLanguage(toShaderLanguage(mCurrentTarget));
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
                        outputPath = outputPath + "_" + keyword.getName() + "_" + snippet.stage + "." + mCurrentTarget;
                    }
                    else
                    {
                        outputPath = outputPath + "_" + snippet.stage + "." + mCurrentTarget;
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
            auto getShaderStage = [](const String &stage, SHADER_STAGE &type) -> HLSLStage
            {
                if (stage == kVertex)
                {
                    type = SHADER_STAGE::kVertex;
                    return HLSLStage::kVertex;
                }
                else if (stage == kFragment)
                {
                    type = SHADER_STAGE::kPixel;
                    return HLSLStage::kPixel;
                }
                else if (stage == kGeometry)
                {
                    type = SHADER_STAGE::kGeometry;
                    return HLSLStage::kGeometry;
                }
                else if (stage == kHull)
                {
                    type = SHADER_STAGE::kHull;
                    return HLSLStage::kHull;
                }
                else if (stage == kDomain)
                {
                    type = SHADER_STAGE::kDomain;
                    return HLSLStage::kDomain;
                }
                else if (stage == kCompute)
                {
                    type = SHADER_STAGE::kCompute;
                    return HLSLStage::kCompute;
                } 
                else
                {
                    type = SHADER_STAGE::kVertex;
                    return HLSLStage::kVertex;
                }
            };

            // dxil / 裸 msl / 未知 target 过去都会静默编成 HLSL，现在直接报错
            auto getShadingLanguage = [](const String& str, String &error) -> HLSLTarget
            {
                if (str == "glsl")
                    return HLSLTarget::kGlsl;
                else if (str == "hlsl")
                    return HLSLTarget::kHlsl;
                else if (str == "essl")
                    return HLSLTarget::kEssl;
                else if (str == "spirv")
                    return HLSLTarget::kSpirV;
                else if (str == "msl_macos")
                    return HLSLTarget::kMslMacOS;
                else if (str == "msl_ios")
                    return HLSLTarget::kMslIOS;
                else if (str == "dxil")
                    error = "Target 'dxil' is not supported. Use 'hlsl' instead.";
                else if (str == "msl")
                    error = "Target 'msl' is ambiguous. Use 'msl_macos' or 'msl_ios'.";
                else
                    error = "Unknown target '" + str + "'. Supported: hlsl / glsl / essl / spirv / msl_macos / msl_ios.";
                return HLSLTarget::kHlsl;
            };

            HLSLCrossTarget tgt;
            String targetError;
            tgt.language = getShadingLanguage(mCurrentTarget, targetError);
            if (!targetError.empty())
            {
                SCC_LOG_ERROR("%s", targetError.c_str());
                ret = false;
                break;
            }

            TArray<HLSLMacroDefine> defines;
            defines.reserve(snippet.defines.size() + mArgs.defines.size());
            ShaderKeyword keyword;
            for (const MacroDefine &define : snippet.defines)
            {
                defines.push_back({ define.name, define.value });
                keyword.addKeyword(define.name);
            }
            keyword.generate();

            // keyword 生成后，再追加命令行 -D 宏（不影响 keyword 和文件名）
            for (const MacroDefine &define : mArgs.defines)
            {
                defines.push_back({ define.name, define.value });
            }

            SHADER_STAGE shaderType;
            HLSLCrossSource src;
            src.source = snippet.source;
            src.fileName = mInputPath;
            src.entryPoint = snippet.entry;
            src.stage = getShaderStage(snippet.stage, shaderType);
            src.defines = std::move(defines);
            if (!mArgs.include.empty())
            {
                src.includeDirs.push_back(mArgs.include);
            }

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

            if (tgt.language == HLSLTarget::kEssl)
            {
                tgt.version = convertToESSLVersion(snippet.model);
            }
            else if (tgt.language == HLSLTarget::kGlsl)
            {
                tgt.version = convertToGLSLVersion(snippet.model);
            }
            else
            {
                tgt.version = snippet.model;
            }

            HLSLCrossOptions opt;
            opt.packMatricesInRowMajor = false;
            opt.optimizationLevel = mArgs.optimizeLevel;
            opt.enableDebugInfo = mArgs.hasOptions(Args::OPT_ENABLE_DEBUG_INFO);
            // shader model 保持默认 6.0，与 ShaderConductor 一致

            const HLSLCrossResult result = HLSLCrossCompiler::compile(src, opt, tgt);

            if (!result.message.empty())
            {
                if (result.hasError)
                {
                    SCC_LOG_ERROR("Shader compile error: %s", result.message.c_str());
                }
                else
                {
                    SCC_LOG_WARNING("Shader compile warning: %s", result.message.c_str());
                }
            }

            if (result.hasError)
            {
                ret = false;
                break;
            }

            // HLSL 目标的语义修复已在 HLSLCrossCompiler 内部完成
            if (!result.target.empty() && postProcessor != nullptr)
            {
                postProcessor(result.toString(), std::move(keyword), shaderType);
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------
}

