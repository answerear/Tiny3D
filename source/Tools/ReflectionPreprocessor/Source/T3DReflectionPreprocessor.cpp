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

#include "T3DReflectionPreprocessor.h"
#include "T3DPreprocessorCommand.h"
#include "T3DPreprocessorOptions.h"
#include "T3DReflectionGenerator.h"
#include "T3DRPErrorCode.h"

#include <fstream>
#include <sstream>
#include <thread>
#include <future>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstdio>


namespace  Tiny3D
{
    //-------------------------------------------------------------------------

    const String ReflectionPreprocessor::kReflectionSettingsFile = "ReflectionSettings.json";
    
    //-------------------------------------------------------------------------
    
    ReflectionPreprocessor::ReflectionPreprocessor()
        : mGenerator(T3D_NEW ReflectionGenerator())
    {
        
    }

    //-------------------------------------------------------------------------
    
    ReflectionPreprocessor::~ReflectionPreprocessor()
    {
        delete mGenerator;
        mGenerator = nullptr;
    }

    //-------------------------------------------------------------------------

    TResult ReflectionPreprocessor::execute(int32_t argc, char* argv[])
    {
        TResult ret = T3D_OK;

        do
        {
            PreprocessorCommand cmd;
            PreprocessorOptions opts;

            // 解析命令行参数
            if (!cmd.parse(argc, argv, opts))
            {
                ret = T3D_ERR_FAIL;
                break;
            }

            // 从配置文件获取编译参数
            ClangArgs args = parseSettingsFile(opts.SettingsPath);
            if (args.empty())
            {
                ret = T3D_ERR_FILE_NOT_EXIST;
                break;
            }

            // 系统头搜索路径要在解析之前就确定下来，缺了它后面每个文件都会以
            // 同样的 "file not found" 失败，白跑几分钟才看得出问题
            ret = verifySystemIncludes();
            if (T3D_FAILED(ret))
            {
                break;
            }

            RP_LOG_INFO("Starting reflection [%s] ...", opts.SourcePath.c_str());

            // 设置自动反射类
            StringList whitelist;
            whitelist.push_back("std::vector");
            whitelist.push_back("std::list");
            whitelist.push_back("std::deque");
            whitelist.push_back("std::queue");
            whitelist.push_back("std::stack");
            whitelist.push_back("std::priority_queue");
            whitelist.push_back("std::set");
            whitelist.push_back("std::multiset");
            whitelist.push_back("std::map");
            whitelist.push_back("std::multimap");
            whitelist.push_back("std::unordered_set");
            whitelist.push_back("std::unordered_multiset");
            whitelist.push_back("std::unordered_map");
            whitelist.push_back("std::unordered_multimap");
            for (const auto klass : opts.WhiteList)
            {
                whitelist.push_back(klass);
            }
            mGenerator->setBuiltinClass(whitelist);

            // 设置工程路径
            mGenerator->setProjectPath(opts.SourcePath);

            RP_LOG_INFO("Parse project header path !");

            // 分析头文件包含路径并记录
            mGenerator->parseProjectHeaderPath(args);

            RP_LOG_INFO("Collect project headers !");

            // 收集项目头文件信息
            ret = collectProjectHeaders(opts.SourcePath);
            if (T3D_FAILED(ret))
            {
                RP_LOG_ERROR("Collect project headers failed ! ERROR [%d]", ret);
                break;
            }

            String path = opts.SourcePath + Dir::getNativeSeparator() + mGeneratedPath;
            
            // 确保 Generated 目录存在（PCH 文件需要写入此目录）
            Dir::makeDir(path);

            // 确保 Generated/.deps/ 子目录存在（增量构建依赖文件存放于此）
            String depsPath = path + Dir::getNativeSeparator() + ".deps";
            Dir::makeDir(depsPath);

            RP_LOG_INFO("Generating AST [%s] ...", path.c_str());

            // 尝试生成 PCH 加速后续解析
            String pchPath = detectAndGeneratePCH(path, args, opts.IsRebuild);
            if (!pchPath.empty())
            {
                // 注入 -include-pch 参数（libclang 直接接收 cc1 参数，不需要 -Xclang）
                mArgs.push_back("-include-pch");
                mArgs.push_back(pchPath);
                syncClangArgs(args);
            }

            // 增量模式先验一下缓存完整性。缓存缺失时增量产物和全量不等价，
            // 与其生成一份错的，不如整体退回全量重新生成
            if (!opts.IsRebuild)
            {
                // 源文件删掉、或者头文件不再带反射宏之后，遗留的孤儿缓存会让下面
                // 的校验永远判定不一致，从此每次构建都被迫退回全量
                pruneOrphanCacheFiles(path, opts.SourcePath);

                String reason;
                if (!checkIncrementalCache(path, opts.DumpAST, reason))
                {
                    RP_LOG_WARNING("Incremental cache unusable : %s. "
                        "Fallback to full rebuild.", reason.c_str());
                    opts.IsRebuild = true;
                }
            }

            // 收集待处理的源文件
            std::vector<PendingFile> pendingFiles;
            collectSourceFiles(opts.SourcePath, path, opts.IsRebuild, pendingFiles);

            // 解析 AST
            ret = generateAST(args, pendingFiles, opts.NumThreads);
            if (T3D_FAILED(ret))
            {
                RP_LOG_ERROR("Generating AST failed ! ERROR [%d]", ret);
                break;
            }

            RP_LOG_INFO("Generating source files ...");

            uint32_t processedCount = 0;
            for (const auto &pf : pendingFiles)
            {
                if (pf.processed)
                {
                    ++processedCount;
                }
            }
            // 解析失败的文件不能悄悄放过：产物会停留在上一轮的状态，而退出码是 0，
            // 构建照常通过，等到运行期反射对不上才发现，届时已经无从追溯
            const uint32_t totalCount = (uint32_t)pendingFiles.size();
            if (processedCount != totalCount)
            {
                RP_LOG_ERROR("%u of %u source file(s) failed to parse, "
                    "keep existing generated sources.",
                    totalCount - processedCount, totalCount);
                ret = T3D_ERR_RP_PARSE_SOURCE;
                break;
            }

            // 增量模式下，加载跳过文件的 .tpl 模板实例化信息注入 mGenerator
            if (!opts.IsRebuild)
            {
                String depsDir = path + Dir::getNativeSeparator() + ".deps";
                loadAllTemplateFiles(depsDir, pendingFiles);
            }

            // 生成源码文件（rebuild 模式会先删除再重建 Generated 目录）
            ret = generateSource(path, opts.IsRebuild, opts.DumpAST);

            // 在 generateSource 之后写入 .deps 和 .tpl 文件（确保不会被 rebuild 的目录删除覆盖）
            for (const auto &pf : pendingFiles)
            {
                if (pf.processed)
                {
                    const StringList &deps = mGenerator->getFileDependencies(pf.fileTitle);
                    writeDepsFile(pf.depsFile, pf.filePath, deps);

                    // 写入 .tpl 模板实例化持久化文件
                    String tplFile = path + Dir::getNativeSeparator() + ".deps" + Dir::getNativeSeparator() + pf.fileTitle + ".tpl";
                    writeTemplateFile(tplFile, pf.fileTitle);
                }
            }

            RP_LOG_INFO("Completed reflection [%s] ! ERROR [%d]", opts.SourcePath.c_str(), ret);
        } while (false);
        
        return ret;
    }
    
    //-------------------------------------------------------------------------

    class JsonHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, JsonHandler> 
    {
    public:
        static const std::string kTagIncludePath;
        static const std::string kTagSystemIncludePath;
        static const std::string kTagMacroDefinition;
        static const std::string kTagOtherFlags;
        static const std::string kTagGeneratedPath;
        
        JsonHandler(const std::string &path, ClangArgs &clangArgs, StringList &args, std::string &generatedPath)
            : mPath(path)
            , mClangArgs(clangArgs)
            , mArgs(args)
            , mGeneratedPath(generatedPath)
            , mState(kExpectObjectStart)
        {}

        bool Default()
        {
            return false;
        }
        
        bool String(const Ch* str, rapidjson::SizeType length, bool copy)
        {
            if (mState & kExpectNameOrObjectEnd)
            {
                mState = kExpectValue;
                
                std::string name(str);
                if (name == kTagIncludePath)
                {
                    mState |= kExpectIncludePath;
                }
                else if (name == kTagSystemIncludePath)
                {
                    mState |= kExpectSystemIncludePath;
                }
                else if (name == kTagMacroDefinition)
                {
                    mState |= kExpectMacroDefinition;
                }
                else if (name == kTagOtherFlags)
                {
                    mState |= kExpectOtherFlags;
                }
                else if (name == kTagGeneratedPath)
                {
                    mState |= kExpectGeneratedPath;
                }
            }
            else if (mState & kExpectValue)
            {
                if (mState & kExpectIncludePath)
                {
                    // 头文件路径
                    std::string arg(str);
                    // arg = mPath + Dir::getNativeSeparator() + arg;
                    arg = Dir::formatPath(arg);
                    arg = "-I" + arg;
                    mArgs.push_back(std::move(arg));
                    mClangArgs.push_back(mArgs.back().c_str());
                }
                else if (mState & kExpectSystemIncludePath)
                {
                    // 工具链自带的头文件路径。用 -isystem 而不是 -I，既排在项目
                    // 路径之后，也不会把系统头自身的警告刷进日志
                    std::string arg(str);
                    mArgs.push_back("-isystem");
                    mClangArgs.push_back(mArgs.back().c_str());
                    mArgs.push_back(std::move(arg));
                    mClangArgs.push_back(mArgs.back().c_str());
                }
                else if (mState & kExpectMacroDefinition)
                {
                    // 宏定义
                    std::string arg(str);
                    arg = "-D" + arg;
                    mArgs.push_back(std::move(arg));
                    mClangArgs.push_back(mArgs.back().c_str());
                }
                else if (mState & kExpectOtherFlags)
                {
                    // 其他标记
                    std::string arg(str);
                    mArgs.push_back(std::move(arg));
                    mClangArgs.push_back(mArgs.back().c_str());
                }
                else if (mState & kExpectGeneratedPath)
                {
                    // 生成路径
                    mGeneratedPath = str;
                }
            }
            
            return true;
        }
        
        bool StartObject()
        {
            bool ret = false;
            if (mState & kExpectObjectStart)
            {
                mState = kExpectNameOrObjectEnd;
                ret = true;
            }
            return ret;
        }
        
        bool EndObject(rapidjson::SizeType memberCount)
        {
            mState = kExpectObjectStart;
            return true;			
        }
        
        bool StartArray()
        {
            bool ret = mState & kExpectValue;
            return ret;
        }
        
        bool EndArray(rapidjson::SizeType elementCount)
        {
            mState = kExpectNameOrObjectEnd;
            return true;
        }

    protected:
        const std::string &mPath;  
        ClangArgs &mClangArgs;
        StringList &mArgs;
        std::string &mGeneratedPath;

        enum State : uint32_t
        {
            kExpectObjectStart = 0x000000001,
            kExpectNameOrObjectEnd = 0x00000002,
            kExpectValue = 0x00000004,
            kExpectIncludePath = 0x00000100,
            kExpectMacroDefinition = 0x00000200,
            kExpectOtherFlags = 0x00000400,
            kExpectGeneratedPath = 0x00000800,
            kExpectSystemIncludePath = 0x00001000
        };

        uint32_t mState;
    };

    const std::string JsonHandler::kTagIncludePath = "IncludePath";
    const std::string JsonHandler::kTagSystemIncludePath = "SystemIncludePath";
    const std::string JsonHandler::kTagMacroDefinition = "MacroDefinition";
    const std::string JsonHandler::kTagOtherFlags = "OtherFlags";
    const std::string JsonHandler::kTagGeneratedPath = "GeneratedPath";

    ClangArgs ReflectionPreprocessor:: parseSettingsFile(const String& path)
    {
        ClangArgs args;
        
        String filename = path + Dir::getNativeSeparator() + kReflectionSettingsFile;
        FileDataStream fs;
        
        if (fs.open(filename.c_str(), FileDataStream::E_MODE_READ_ONLY))
        {
            JsonStream stream(fs);
            JsonHandler handler(path, args, mArgs, mGeneratedPath);
            rapidjson::Reader reader;

            if (!reader.Parse(stream, handler))
            {
                rapidjson::ParseErrorCode e = reader.GetParseErrorCode();
                size_t o = reader.GetErrorOffset();
                std::stringstream ss;
                ss << "Error: " << rapidjson::GetParseError_En(e) << std::endl;;
                ss << " at offset " << o << std::endl;
                RP_LOG_ERROR("Parse json failed ! %s", ss.str().c_str());
                args.clear();
            }
            
            fs.close();
        }
        else
        {
            RP_LOG_ERROR("The file %s did not exist !", filename.c_str());
        }

        if (!args.empty())
        {
            appendSysroot(args);
            appendMSVCIncludes(args);
            appendResourceDir(args);
            syncClangArgs(args);
        }

        return args;
    }

    //-------------------------------------------------------------------------

    void ReflectionPreprocessor::appendSysroot(ClangArgs &args)
    {
        for (const auto &s : mArgs)
        {
            if (s == "-isysroot" || StringUtil::startsWith(s, "-isysroot", false))
            {
                return;
            }
        }

#if defined(T3D_OS_OSX)
        String sdk;
        if (const char *env = std::getenv("SDKROOT"))
        {
            sdk = env;
        }

        if (sdk.empty())
        {
            FILE *pipe = popen("xcrun --sdk macosx --show-sdk-path 2>/dev/null", "r");
            if (pipe != nullptr)
            {
                char buf[1024] = {0};
                if (fgets(buf, sizeof(buf), pipe) != nullptr)
                {
                    sdk = buf;
                    while (!sdk.empty()
                        && (sdk.back() == '\n' || sdk.back() == '\r'))
                    {
                        sdk.pop_back();
                    }
                }
                pclose(pipe);
            }
        }

        if (sdk.empty() || !Dir::exists(sdk))
        {
            RP_LOG_WARNING("No -isysroot in reflection settings and SDK lookup "
                "failed; libclang may not find C++ standard headers.");
            return;
        }

        RP_LOG_INFO("Using macOS sysroot: %s", sdk.c_str());
        mArgs.push_back("-isysroot");
        mArgs.push_back(sdk);
        mArgs.push_back("-stdlib=libc++");
        syncClangArgs(args);
#endif
    }

    //-------------------------------------------------------------------------

    String ReflectionPreprocessor::findVCToolsInclude()
    {
#if defined(T3D_OS_WINDOWS)
        // vcvarsall 起的环境里直接就有，不必再猜
        if (const char *env = std::getenv("VCToolsInstallDir"))
        {
            String dir = env;
            while (!dir.empty() && (dir.back() == '\\' || dir.back() == '/'))
            {
                dir.pop_back();
            }

            String include = dir + "\\include";
            if (!dir.empty() && Dir::exists(include))
            {
                return include;
            }
        }

        // 只扫默认安装位置。装到别处的话没法靠猜，那种环境应该用配置里的
        // SystemIncludePath，别指望这个兜底
        StringList vsRoots;
        static const char *kProgramFilesEnvs[] = {
            "ProgramFiles(x86)", "ProgramFiles"
        };
        for (const auto *name : kProgramFilesEnvs)
        {
            if (const char *env = std::getenv(name))
            {
                if (env[0] != '\0')
                {
                    vsRoots.push_back(String(env) + "\\Microsoft Visual Studio");
                }
            }
        }

        // T3D_RPP_VS_YEAR 可以指定用哪一代，比如 "2019"，不设就挑最新的
        String wantedYear;
        if (const char *env = std::getenv("T3D_RPP_VS_YEAR"))
        {
            wantedYear = env;
        }

        String best;
        auto pickNewer = [&best](const String &candidate)
        {
            // 目录名形如 14.29.30133，位数一致，字典序即版本序
            if (candidate > best)
            {
                best = candidate;
            }
        };

        auto forEachSubDir = [](const String &parent,
            const std::function<void(const String &)> &visit)
        {
            Dir dir;
            bool working = dir.findFile(parent + "\\*");
            while (working)
            {
                if (!dir.isDots() && dir.isDirectory())
                {
                    visit(dir.getFilePath());
                }
                working = dir.findNextFile();
            }
            dir.close();
        };

        for (const auto &vsRoot : vsRoots)
        {
            if (!Dir::exists(vsRoot))
            {
                continue;
            }

            forEachSubDir(vsRoot, [&](const String &yearDir)
            {
                String parent, year, ext;
                Dir::parsePath(yearDir + "\\.", parent, year, ext);
                if (!wantedYear.empty() && year != wantedYear)
                {
                    return;
                }

                forEachSubDir(yearDir, [&](const String &editionDir)
                {
                    const String toolsRoot = editionDir + "\\VC\\Tools\\MSVC";
                    if (!Dir::exists(toolsRoot))
                    {
                        return;
                    }

                    forEachSubDir(toolsRoot, [&](const String &toolsetDir)
                    {
                        const String include = toolsetDir + "\\include";
                        if (Dir::exists(include))
                        {
                            pickNewer(include);
                        }
                    });
                });
            });
        }

        return best;
#else
        return String();
#endif
    }

    //-------------------------------------------------------------------------

    String ReflectionPreprocessor::findWindowsSDKIncludeRoot()
    {
#if defined(T3D_OS_WINDOWS)
        // vcvarsall 起的环境里直接就有
        const char *sdkDir = std::getenv("WindowsSdkDir");
        const char *sdkVer = std::getenv("WindowsSDKVersion");
        if (sdkDir != nullptr && sdkVer != nullptr)
        {
            String version = sdkVer;
            while (!version.empty()
                && (version.back() == '\\' || version.back() == '/'))
            {
                version.pop_back();
            }

            String root = String(sdkDir) + "Include\\" + version;
            if (Dir::exists(root))
            {
                return root;
            }
        }

        String kitsRoot;
        if (const char *env = std::getenv("ProgramFiles(x86)"))
        {
            kitsRoot = String(env) + "\\Windows Kits\\10";
        }

        if (kitsRoot.empty() || !Dir::exists(kitsRoot))
        {
            return String();
        }

        // 挑版本号最大、且 um 和 ucrt 都齐的那一套
        String best;
        Dir dir;
        bool working = dir.findFile(kitsRoot + "\\Include\\*");
        while (working)
        {
            if (!dir.isDots() && dir.isDirectory())
            {
                String candidate = dir.getFilePath();
                if (Dir::exists(candidate + "\\um")
                    && Dir::exists(candidate + "\\ucrt")
                    && candidate > best)
                {
                    best = candidate;
                }
            }
            working = dir.findNextFile();
        }
        dir.close();

        return best;
#else
        return String();
#endif
    }

    //-------------------------------------------------------------------------

    void ReflectionPreprocessor::appendMSVCIncludes(ClangArgs &args)
    {
#if defined(T3D_OS_WINDOWS)
        for (const auto &s : mArgs)
        {
            if (s == "-isystem")
            {
                RP_LOG_INFO("Reflection settings provide system include paths.");
                return;
            }
        }

        if (const char *env = std::getenv("INCLUDE"))
        {
            if (env[0] != '\0')
            {
                RP_LOG_INFO("Using system include paths from the INCLUDE "
                    "environment variable.");
                return;
            }
        }

        const String vcInclude = findVCToolsInclude();
        const String sdkRoot = findWindowsSDKIncludeRoot();

        if (vcInclude.empty() || sdkRoot.empty())
        {
            // 这里不报错，交给 verifySystemIncludes 统一裁决
            RP_LOG_WARNING("MSVC toolchain auto-detection incomplete "
                "[VC=%s] [SDK=%s].",
                vcInclude.empty() ? "<not found>" : vcInclude.c_str(),
                sdkRoot.empty() ? "<not found>" : sdkRoot.c_str());
            return;
        }

        RP_LOG_WARNING("No system include path in reflection settings, "
            "falling back to auto-detected toolchain "
            "[VC=%s] [SDK=%s]. It may differ from the toolset that compiles "
            "the project; re-run the generate script to pin it down.",
            vcInclude.c_str(), sdkRoot.c_str());

        auto appendDir = [this](const String &dir)
        {
            if (!Dir::exists(dir))
            {
                return;
            }
            mArgs.push_back("-isystem");
            mArgs.push_back(dir);
        };

        appendDir(vcInclude);

        // ucrt 给 C 运行库，um 给 Win32 API（WinSock.h 在这儿），shared 给两者
        // 共用的定义，winrt / cppwinrt 少数头会用到
        static const char *kSDKSubDirs[] = {
            "ucrt", "um", "shared", "winrt", "cppwinrt"
        };
        for (const auto *sub : kSDKSubDirs)
        {
            appendDir(sdkRoot + "\\" + sub);
        }

        syncClangArgs(args);
#endif
    }

    //-------------------------------------------------------------------------

    TResult ReflectionPreprocessor::verifySystemIncludes() const
    {
#if defined(T3D_OS_WINDOWS)
        for (const auto &s : mArgs)
        {
            if (s == "-isystem")
            {
                return T3D_OK;
            }
        }

        if (const char *env = std::getenv("INCLUDE"))
        {
            if (env[0] != '\0')
            {
                return T3D_OK;
            }
        }

        RP_LOG_ERROR("No MSVC / Windows SDK include path available: "
            "'SystemIncludePath' is missing from %s, the INCLUDE environment "
            "variable is empty and toolchain auto-detection failed. Every "
            "translation unit would fail with \"file not found\", so stop here. "
            "Re-run the engine generate script to refresh %s, or build from a "
            "Visual Studio developer prompt.",
            kReflectionSettingsFile.c_str(), kReflectionSettingsFile.c_str());

        return T3D_ERR_RP_NO_SYSTEM_INCLUDE;
#else
        return T3D_OK;
#endif
    }

    //-------------------------------------------------------------------------

    void ReflectionPreprocessor::appendResourceDir(ClangArgs &args)
    {
        // libclang 靠自己所在的目录反推 LLVM 安装布局，才能找到编译器内建头
        // (stdarg.h、stddef.h 等)。这里的 libclang 是单独拷到 rpp 旁边的，那套
        // 推断必然落空，所以显式指定，也免得依赖 clang 内部的布局约定。
        // Windows 上这些内建头由 MSVC 的 UCRT 提供，不随包，目录不存在即跳过。
        const char sep = Dir::getNativeSeparator();
        const String appPath = Dir::getAppPath();
        String resourceDir = appPath + sep + "clang-resource";
        if (!Dir::exists(resourceDir))
        {
            // macOS bundle：头文件在 Contents/Resources，不能放进 Contents/MacOS
            resourceDir = appPath + sep + ".." + sep + "Resources" + sep
                + "clang-resource";
        }

        if (!Dir::exists(resourceDir))
        {
#if defined(T3D_OS_OSX)
            FILE *pipe = popen("xcrun clang -print-resource-dir 2>/dev/null", "r");
            if (pipe != nullptr)
            {
                char buf[1024] = {0};
                if (fgets(buf, sizeof(buf), pipe) != nullptr)
                {
                    resourceDir = buf;
                    while (!resourceDir.empty()
                        && (resourceDir.back() == '\n' || resourceDir.back() == '\r'))
                    {
                        resourceDir.pop_back();
                    }
                }
                pclose(pipe);
            }
#endif
        }

        if (!Dir::exists(resourceDir))
        {
            RP_LOG_WARNING("clang resource dir not found next to rpp, libclang may fail to parse.");
            return;
        }

        RP_LOG_INFO("Using clang resource dir: %s", resourceDir.c_str());
        mArgs.push_back("-resource-dir");
        mArgs.push_back(resourceDir);
        syncClangArgs(args);
    }

    //-------------------------------------------------------------------------

    void ReflectionPreprocessor::syncClangArgs(ClangArgs &args)
    {
        args.clear();
        args.reserve(mArgs.size());
        for (const auto &s : mArgs)
        {
            args.push_back(s.c_str());
        }
    }

    //-------------------------------------------------------------------------

    TResult ReflectionPreprocessor::collectProjectHeaders(const String& path)
    {
        TResult ret = T3D_OK;

        String searchPath = path + Dir::getNativeSeparator() + "*.*";

        Dir dir;

        // 收集工程头文件信息
        bool working = dir.findFile(searchPath);

        while (working)
        {
            if (dir.isDots())
            {
                // . or ..
            }
            else if (dir.isDirectory())
            {
                // directory
                collectProjectHeaders(dir.getFilePath());
            }
            else
            {
                // file
                const String filePath = dir.getFilePath();
                String fileDir, fileTitle, fileExt;
                Dir::parsePath(filePath, fileDir, fileTitle, fileExt);
                if (fileExt == "h" || fileExt == "hpp")
                {
                    mGenerator->collectProjectHeaders(filePath);

                    // 预扫描：检查头文件是否包含反射宏
                    if (hasReflectionMacros(filePath))
                    {
                        mReflectionHeaders.insert(fileTitle);
                        RP_LOG_INFO(">>> [prescan] %s contains reflection macros.", fileTitle.c_str());
                    }

                    // 检测 Prerequisites 头文件作为 PCH 候选
                    if (mPrerequisitesHeader.empty()
                        && (fileTitle.find("Prerequisites") != String::npos
                            || fileTitle.find("Prereq") != String::npos))
                    {
                        mPrerequisitesHeader = filePath;
                        RP_LOG_INFO(">>> [PCH] Found prerequisites header: %s", filePath.c_str());
                    }
                }
            }

            working = dir.findNextFile();
        }

        dir.close();
        
        RP_LOG_INFO("Prescan complete: %u header(s) with reflection macros.", (uint32_t)mReflectionHeaders.size());

        return ret;
    }
    
    //-------------------------------------------------------------------------

    void ReflectionPreprocessor::collectSourceFiles(const String &path, const String &generatedPath, bool rebuild,
                                                    std::vector<PendingFile> &pendingFiles)
    {
        String searchPath = path + Dir::getNativeSeparator() + "*.*";

        Dir dir;
        bool working = dir.findFile(searchPath);

        while (working)
        {
            if (dir.isDots())
            {
                // . or ..
            }
            else if (dir.isDirectory())
            {
                collectSourceFiles(dir.getFilePath(), generatedPath, rebuild, pendingFiles);
            }
            else
            {
                const String filePath = dir.getFilePath();
                String fileDir, fileTitle, fileExt;
                Dir::parsePath(filePath, fileDir, fileTitle, fileExt);
                if (fileExt == "cpp" || fileExt == "cxx")
                {
                    // 预扫描过滤：如果同名 .h 不含反射宏，跳过该 .cpp
                    if (mReflectionHeaders.find(fileTitle) == mReflectionHeaders.end())
                    {
                        RP_LOG_INFO(">>> [prescan] %s skipped (no reflection macros in header).", filePath.c_str());
                        working = dir.findNextFile();
                        continue;
                    }

                    PendingFile pf;
                    pf.filePath = filePath;
                    pf.fileTitle = fileTitle;
                    pf.generatedFile = generatedPath + Dir::getNativeSeparator() + fileTitle + ".generated.cpp";
                    pf.depsFile = generatedPath + Dir::getNativeSeparator() + ".deps" + Dir::getNativeSeparator() + fileTitle + ".deps";

                    if (rebuild || needsRebuild(pf.depsFile, pf.filePath, pf.generatedFile))
                    {
                        pendingFiles.push_back(std::move(pf));
                    }
                    else
                    {
                        RP_LOG_INFO(">>> %s up-to-date, skipped.", filePath.c_str());
                    }
                }
            }

            working = dir.findNextFile();
        }

        dir.close();
    }

    //-------------------------------------------------------------------------

    TResult ReflectionPreprocessor::generateAST(const ClangArgs &args, std::vector<PendingFile> &pendingFiles, int32_t numThreads)
    {
        TResult ret = T3D_OK;

        if (pendingFiles.empty())
        {
            RP_LOG_INFO("All files are up-to-date, nothing to do.");
            return ret;
        }

        RP_LOG_INFO("Found %u file(s) to process.", (uint32_t)pendingFiles.size());

        auto totalStart = std::chrono::steady_clock::now();

        if (numThreads <= 1 || pendingFiles.size() <= 1)
        {
            // 单线程模式
            for (auto &pf : pendingFiles)
            {
                auto fileStart = std::chrono::steady_clock::now();

                mGenerator->clearCurrentDependencies();
                TResult parseRet = mGenerator->generateAST(pf.filePath, args);

                auto fileEnd = std::chrono::steady_clock::now();
                auto fileMs = std::chrono::duration_cast<std::chrono::milliseconds>(fileEnd - fileStart).count();
                RP_LOG_INFO("[timing] %s : %lld ms", pf.fileTitle.c_str(), (long long)fileMs);

                if (!T3D_FAILED(parseRet) || parseRet == T3D_ERR_RP_COMPILE_WARNING)
                {
                    pf.processed = true;
                }
                else if (T3D_RP_FATAL(parseRet))
                {
                    // 致命诊断基本都是头文件找不到，剩下的文件用的是同一套编译
                    // 参数，继续跑只会刷出几百条一样的报错
                    RP_LOG_ERROR("Fatal diagnostic on [%s], abort the whole run.",
                        pf.filePath.c_str());
                    return parseRet;
                }
            }
        }
        else
        {
            // 多线程模式：并行 parse → 串行 visit
            int32_t actualThreads = std::min(numThreads, (int32_t)pendingFiles.size());
            RP_LOG_INFO("Using %d thread(s) for parallel parsing.", actualThreads);

            // 创建 CXIndex 池
            std::vector<CXIndex> indexPool(actualThreads, nullptr);
            for (int32_t t = 0; t < actualThreads; ++t)
            {
                indexPool[t] = clang_createIndex(0, 0);
                if (indexPool[t] == nullptr)
                {
                    RP_LOG_ERROR("Failed to create CXIndex for thread slot %d !", t);
                }
            }

            std::vector<std::future<ReflectionGenerator::ParsedUnit>> futures;
            futures.reserve(pendingFiles.size());

            TResult fatalRet = T3D_OK;
            String fatalFile;
            size_t idx = 0;
            while (idx < pendingFiles.size())
            {
                size_t batchEnd = std::min(idx + (size_t)actualThreads, pendingFiles.size());
                futures.clear();

                auto batchStart = std::chrono::steady_clock::now();

                for (size_t i = idx; i < batchEnd; ++i)
                {
                    const auto &pf = pendingFiles[i];
                    CXIndex slotIndex = indexPool[i - idx];
                    futures.push_back(std::async(std::launch::async,
                        [&pf, &args, slotIndex]()
                        {
                            return ReflectionGenerator::parseOnly(pf.filePath, args, slotIndex);
                        }));
                }

                for (size_t i = 0; i < futures.size(); ++i)
                {
                    auto unit = futures[i].get();
                    auto &pf = pendingFiles[idx + i];

                    if (unit.cxUnit != nullptr)
                    {
                        TResult visitRet = mGenerator->visitParsedUnit(unit);

                        if (!T3D_FAILED(visitRet) || visitRet == T3D_ERR_RP_COMPILE_WARNING)
                        {
                            pf.processed = true;
                        }
                        else if (T3D_RP_FATAL(visitRet) && fatalRet == T3D_OK)
                        {
                            fatalRet = visitRet;
                            fatalFile = pf.filePath;
                        }
                    }
                    else
                    {
                        RP_LOG_ERROR("Parse source file [%s] failed !", pf.filePath.c_str());
                    }
                }

                auto batchEnd_ = std::chrono::steady_clock::now();
                auto batchMs = std::chrono::duration_cast<std::chrono::milliseconds>(batchEnd_ - batchStart).count();
                RP_LOG_INFO("[timing] batch [%u..%u] : %lld ms", (uint32_t)idx, (uint32_t)(batchEnd - 1), (long long)batchMs);

                idx = batchEnd;

                if (fatalRet != T3D_OK)
                {
                    break;
                }
            }

            // 销毁 CXIndex 池
            for (auto &cxIdx : indexPool)
            {
                if (cxIdx != nullptr)
                {
                    clang_disposeIndex(cxIdx);
                    cxIdx = nullptr;
                }
            }

            if (fatalRet != T3D_OK)
            {
                // 致命诊断基本都是头文件找不到，剩下的文件用的是同一套编译参数，
                // 继续跑只会刷出几百条一样的报错
                RP_LOG_ERROR("Fatal diagnostic on [%s], abort the whole run.",
                    fatalFile.c_str());
                return fatalRet;
            }
        }

        auto totalEnd = std::chrono::steady_clock::now();
        auto totalMs = std::chrono::duration_cast<std::chrono::milliseconds>(totalEnd - totalStart).count();
        RP_LOG_INFO("[timing] Total generateAST : %lld ms (%u files)", (long long)totalMs, (uint32_t)pendingFiles.size());

        return ret;
    }

    //-------------------------------------------------------------------------

    TResult ReflectionPreprocessor::generateSource(const String& path, bool rebuild, bool dumpAST)
    {
        if (rebuild)
        {
            Dir::removeDir(path, true);
            Dir::makeDir(path);
            // 重建 .deps 子目录
            Dir::makeDir(path + Dir::getNativeSeparator() + ".deps");
        }
        
        // 输出 AST 到文件，仅在 -d 开关下启用
        if (dumpAST)
        {
            String dumpPath = path + Dir::getNativeSeparator() + "ast.json";
            mGenerator->dumpReflectionInfo(dumpPath);
        }

        return mGenerator->generateSource(path);
    }

    //-------------------------------------------------------------------------

    bool ReflectionPreprocessor::checkIncrementalCache(const String &generatedPath,
        bool dumpAST, String &reason) const
    {
        const String sep(1, Dir::getNativeSeparator());
        const String depsDir = generatedPath + sep + ".deps";

        auto countFiles = [](const String &pattern, StringList &titles)
        {
            Dir dir;
            bool working = dir.findFile(pattern);

            while (working)
            {
                if (!dir.isDots() && !dir.isDirectory())
                {
                    String fileDir, title, ext;
                    Dir::parsePath(dir.getFilePath(), fileDir, title, ext);
                    titles.push_back(title);
                }

                working = dir.findNextFile();
            }

            dir.close();
        };

        // 一个产物都没有，说明是首次生成，没有增量可言
        StringList generatedTitles;
        countFiles(generatedPath + sep + "*.generated.cpp", generatedTitles);

        if (generatedTitles.empty())
        {
            reason = "no generated source file found";
            return false;
        }

        // 注意不能拿产物去比对 .deps：模板实例（TVector3、std::vector 等）的
        // 产物以模板所在头文件命名，没有对应的 .cpp，本来就不会有 .deps
        StringList depsTitles;
        countFiles(depsDir + sep + "*.deps", depsTitles);

        if (depsTitles.empty())
        {
            reason = "dependency cache is empty";
            return false;
        }

        // .deps 与 .tpl 在同一轮循环里成对写出，有 .deps 却缺 .tpl 是致命的：
        // 该文件被跳过时它贡献的模板实例会丢失。反过来多出的 .tpl 不要紧，
        // 对应文件因为没有 .deps 必然会被重新处理，两者都会被重写一遍
        StringList tplTitles;
        countFiles(depsDir + sep + "*.tpl", tplTitles);

        std::unordered_set<std::string> tplSet(tplTitles.begin(), tplTitles.end());

        for (const auto &title : depsTitles)
        {
            if (tplSet.find(title) == tplSet.end())
            {
                reason = "missing template instantiation file ["
                    + depsDir + sep + title + ".tpl]";
                return false;
            }
        }

        // 开了 -d 就应该有上一轮 dump 出来的 ast.json，缺了同样说明目录被动过
        if (dumpAST && !Dir::exists(generatedPath + sep + "ast.json"))
        {
            reason = "missing ast.json";
            return false;
        }

        return true;
    }

    //-------------------------------------------------------------------------

    void ReflectionPreprocessor::collectExpectedSourceTitles(const String &path,
        std::unordered_set<std::string> &titles) const
    {
        Dir dir;
        bool working = dir.findFile(path + Dir::getNativeSeparator() + "*.*");

        while (working)
        {
            if (dir.isDots())
            {
                // . or ..
            }
            else if (dir.isDirectory())
            {
                collectExpectedSourceTitles(dir.getFilePath(), titles);
            }
            else
            {
                String fileDir, fileTitle, fileExt;
                Dir::parsePath(dir.getFilePath(), fileDir, fileTitle, fileExt);
                if ((fileExt == "cpp" || fileExt == "cxx")
                    && mReflectionHeaders.find(fileTitle) != mReflectionHeaders.end())
                {
                    titles.insert(fileTitle);
                }
            }

            working = dir.findNextFile();
        }

        dir.close();
    }

    //-------------------------------------------------------------------------

    void ReflectionPreprocessor::pruneOrphanCacheFiles(const String &generatedPath,
        const String &sourcePath) const
    {
        const String sep(1, Dir::getNativeSeparator());
        const String depsDir = generatedPath + sep + ".deps";

        if (!Dir::exists(depsDir))
        {
            return;
        }

        std::unordered_set<std::string> expected;
        collectExpectedSourceTitles(sourcePath, expected);

        if (expected.empty())
        {
            // 一个都没收集到不像是真相，宁可什么都不删
            return;
        }

        StringList victims;
        static const char *kPatterns[] = { "*.deps", "*.tpl" };
        for (const auto *pattern : kPatterns)
        {
            Dir dir;
            bool working = dir.findFile(depsDir + sep + pattern);

            // 一边遍历一边删同一个目录不靠谱，先收集完再动手
            while (working)
            {
                if (!dir.isDots() && !dir.isDirectory())
                {
                    String fileDir, title, ext;
                    Dir::parsePath(dir.getFilePath(), fileDir, title, ext);
                    if (expected.find(title) == expected.end())
                    {
                        victims.push_back(dir.getFilePath());
                    }
                }

                working = dir.findNextFile();
            }

            dir.close();
        }

        for (const auto &victim : victims)
        {
            RP_LOG_INFO("Pruning orphan cache file [%s].", victim.c_str());
            Dir::remove(victim);
        }
    }

    //-------------------------------------------------------------------------

    bool ReflectionPreprocessor::needsRebuild(const String &depsFile, const String &srcFile, const String &generatedFile)
    {
        // 如果生成文件不存在，必须重建
        long_t genLastWTime = Dir::getLastWriteTime(generatedFile);
        if (genLastWTime == 0)
        {
            return true;
        }

        // 读取 .deps 文件
        std::ifstream ifs(depsFile.c_str());
        if (!ifs.is_open())
        {
            // .deps 文件不存在，需要重建
            return true;
        }

        // 解析 .deps 文件，格式: <timestamp> <hash> <filepath>
        struct DepEntry
        {
            long_t savedTimestamp {0};
            uint64_t savedHash {0};
            std::string path {};
            long_t currentTimestamp {0};
        };

        std::vector<DepEntry> entries;
        bool needsTimestampUpdate = false;

        std::string line;
        while (std::getline(ifs, line))
        {
            if (line.empty())
                continue;

            std::istringstream iss(line);
            DepEntry entry;
            iss >> entry.savedTimestamp >> entry.savedHash;
            std::getline(iss >> std::ws, entry.path);

            if (entry.path.empty())
                continue;

            entry.currentTimestamp = Dir::getLastWriteTime(entry.path.c_str());

            if (entry.currentTimestamp == entry.savedTimestamp)
            {
                // 时间戳相同，该依赖 OK
            }
            else
            {
                // 时间戳不同，计算当前文件内容哈希进行二次确认
                uint64_t currentHash = computeFileHash(entry.path.c_str());
                if (currentHash != entry.savedHash)
                {
                    // 哈希不同，文件真的变了，需要重建
                    RP_LOG_INFO(">>> Dependency changed: %s", entry.path.c_str());
                    ifs.close();
                    return true;
                }

                // 哈希相同，内容未变（仅时间戳变了），标记需要更新 .deps
                needsTimestampUpdate = true;
            }

            entries.push_back(std::move(entry));
        }

        ifs.close();

        // 所有依赖都 OK，如果有时间戳需要更新则重写 .deps
        if (needsTimestampUpdate)
        {
            std::ofstream ofs(depsFile.c_str(), std::ios::trunc);
            if (ofs.is_open())
            {
                for (const auto &entry : entries)
                {
                    ofs << entry.currentTimestamp << " " << entry.savedHash << " " << entry.path << "\n";
                }
                ofs.close();
            }
        }

        return false;
    }

    //-------------------------------------------------------------------------

    void ReflectionPreprocessor::writeDepsFile(const String &depsFile, const String &srcFile, const StringList &deps) const
    {
        std::ofstream ofs(depsFile.c_str(), std::ios::trunc);
        if (!ofs.is_open())
        {
            RP_LOG_WARNING("Failed to write deps file: %s", depsFile.c_str());
            return;
        }

        // 写入源文件自身：<timestamp> <hash> <filepath>
        long_t srcTimestamp = Dir::getLastWriteTime(srcFile);
        uint64_t srcHash = computeFileHash(srcFile);
        ofs << srcTimestamp << " " << srcHash << " " << srcFile << "\n";

        // 写入所有头文件依赖：<timestamp> <hash> <filepath>
        for (const auto &dep : deps)
        {
            long_t depTimestamp = Dir::getLastWriteTime(dep);
            uint64_t depHash = computeFileHash(dep);
            ofs << depTimestamp << " " << depHash << " " << dep << "\n";
        }

        ofs.close();
    }

    //-------------------------------------------------------------------------

    void ReflectionPreprocessor::writeTemplateFile(const String &tplFile, const String &srcTitle) const
    {
        const auto &instList = mGenerator->getTemplateInstantiations(srcTitle);
        
        std::ofstream ofs(tplFile.c_str(), std::ios::trunc);
        if (!ofs.is_open())
        {
            RP_LOG_WARNING("Failed to write tpl file: %s", tplFile.c_str());
            return;
        }

        // 格式：每行 <sourceFilePath>|<hierarchyName>|<header1>;<header2>;...
        for (const auto &info : instList)
        {
            ofs << info.sourceFilePath << "|" << info.hierarchyName << "|";
            bool first = true;
            for (const auto &h : info.headerPaths)
            {
                if (!first) ofs << ";";
                ofs << h;
                first = false;
            }
            ofs << "\n";
        }

        ofs.close();
    }

    //-------------------------------------------------------------------------

    void ReflectionPreprocessor::loadAllTemplateFiles(const String &depsDir, const std::vector<PendingFile> &pendingFiles)
    {
        // 收集已被重新处理的文件 title 集合
        std::unordered_set<std::string> processedTitles;
        for (const auto &pf : pendingFiles)
        {
            if (pf.processed)
            {
                processedTitles.insert(pf.fileTitle);
            }
        }

        // 遍历 depsDir 下所有 .tpl 文件
        String searchPath = depsDir + Dir::getNativeSeparator() + "*.tpl";
        Dir dir;
        bool working = dir.findFile(searchPath);

        while (working)
        {
            if (!dir.isDots() && !dir.isDirectory())
            {
                const String filePath = dir.getFilePath();
                String fileDir, fileTitle, fileExt;
                Dir::parsePath(filePath, fileDir, fileTitle, fileExt);

                // 只加载未被重新处理的文件的 .tpl
                if (processedTitles.find(fileTitle) == processedTitles.end())
                {
                    // 读取 .tpl 文件并注入
                    std::ifstream ifs(filePath.c_str());
                    if (ifs.is_open())
                    {
                        ReflectionGenerator::TemplateInstList instList;
                        std::string line;
                        while (std::getline(ifs, line))
                        {
                            if (line.empty()) continue;

                            // 解析格式：<sourceFilePath>|<hierarchyName>|<header1>;<header2>;...
                            auto pos1 = line.find('|');
                            if (pos1 == std::string::npos) continue;
                            auto pos2 = line.find('|', pos1 + 1);
                            if (pos2 == std::string::npos) continue;

                            ReflectionGenerator::TemplateInstInfo info;
                            info.sourceFilePath = line.substr(0, pos1);
                            info.hierarchyName = line.substr(pos1 + 1, pos2 - pos1 - 1);

                            // 解析头文件列表
                            std::string headers = line.substr(pos2 + 1);
                            if (!headers.empty())
                            {
                                size_t start = 0;
                                size_t sep;
                                while ((sep = headers.find(';', start)) != std::string::npos)
                                {
                                    info.headerPaths.push_back(headers.substr(start, sep - start));
                                    start = sep + 1;
                                }
                                info.headerPaths.push_back(headers.substr(start));
                            }

                            instList.push_back(std::move(info));
                        }

                        ifs.close();

                        if (!instList.empty())
                        {
                            mGenerator->injectTemplateInstantiations(instList);
                        }
                    }
                }
            }

            working = dir.findNextFile();
        }

        dir.close();
    }

    //-------------------------------------------------------------------------

    uint64_t ReflectionPreprocessor::computeFileHash(const String &filePath)
    {
        std::ifstream ifs(filePath.c_str(), std::ios::binary);
        if (!ifs.is_open())
        {
            return 0;
        }

        // FNV-1a 64 位哈希
        const uint64_t FNV_OFFSET_BASIS = 0xcbf29ce484222325ULL;
        const uint64_t FNV_PRIME = 0x100000001b3ULL;

        uint64_t hash = FNV_OFFSET_BASIS;
        char buffer[4096];

        while (ifs.read(buffer, sizeof(buffer)) || ifs.gcount() > 0)
        {
            auto bytesRead = ifs.gcount();
            for (std::streamsize i = 0; i < bytesRead; ++i)
            {
                hash ^= static_cast<uint64_t>(static_cast<uint8_t>(buffer[i]));
                hash *= FNV_PRIME;
            }
        }

        ifs.close();
        return hash;
    }

    //-------------------------------------------------------------------------

    bool ReflectionPreprocessor::hasReflectionMacros(const String &filePath)
    {
        std::ifstream ifs(filePath.c_str(), std::ios::binary | std::ios::ate);
        if (!ifs.is_open())
        {
            return false;
        }

        auto fileSize = ifs.tellg();
        if (fileSize <= 0)
        {
            return false;
        }

        ifs.seekg(0, std::ios::beg);
        std::string content;
        content.resize(static_cast<size_t>(fileSize));
        ifs.read(&content[0], fileSize);
        ifs.close();

        // 检查反射宏关键字
        static const char* kReflectionMacros[] = {
            "TCLASS",
            "TSTRUCT",
            "TFUNCTION",
            "TPROPERTY",
            "TENUM",
            "TRTTI_ENABLE",
            "TRTTI_FRIEND"
        };

        for (const auto *macro : kReflectionMacros)
        {
            if (content.find(macro) != std::string::npos)
            {
                return true;
            }
        }

        return false;
    }

    //-------------------------------------------------------------------------

    String ReflectionPreprocessor::detectAndGeneratePCH(const String &generatedPath, const ClangArgs &args, bool rebuild)
    {
        if (mPrerequisitesHeader.empty())
        {
            RP_LOG_INFO("[PCH] No prerequisites header found, skipping PCH generation.");
            return String();
        }

        // PCH 文件放在 generatedPath 下。只有带 .ok 标记的才允许复用：
        // 以前缺 -isysroot 时仍会写出坏 PCH，复用会把 String 解析成 int。
        String pchPath = generatedPath + Dir::getNativeSeparator() + "prereq.pch";
        String okPath = pchPath + ".ok";

        if (!rebuild && Dir::exists(okPath) && Dir::exists(pchPath))
        {
            long_t pchTime = Dir::getLastWriteTime(pchPath);
            long_t hdrTime = Dir::getLastWriteTime(mPrerequisitesHeader);
            if (pchTime > 0 && pchTime >= hdrTime)
            {
                RP_LOG_INFO("[PCH] Reusing existing PCH: %s", pchPath.c_str());
                return pchPath;
            }
        }
        else if (rebuild)
        {
            RP_LOG_INFO("[PCH] Full rebuild, regenerating PCH.");
            Dir::remove(pchPath);
            Dir::remove(okPath);
        }

        TResult ret = ReflectionGenerator::generatePCH(mPrerequisitesHeader, pchPath, args);
        if (T3D_FAILED(ret))
        {
            Dir::remove(pchPath);
            Dir::remove(okPath);
            RP_LOG_WARNING("[PCH] PCH generation failed, will proceed without PCH.");
            return String();
        }

        FileDataStream marker;
        if (marker.open(okPath.c_str(),
            FileDataStream::E_MODE_WRITE_ONLY | FileDataStream::E_MODE_TRUNCATE))
        {
            marker.close();
        }

        return pchPath;
    }

    //-------------------------------------------------------------------------
}
