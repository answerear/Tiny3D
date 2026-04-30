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
            String pchPath = detectAndGeneratePCH(path, args);
            if (!pchPath.empty())
            {
                // 注入 -include-pch 参数（libclang 直接接收 cc1 参数，不需要 -Xclang）
                mArgs.push_back("-include-pch");
                args.push_back(mArgs.back().c_str());
                mArgs.push_back(pchPath);
                args.push_back(mArgs.back().c_str());
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

            // 生成源码文件（rebuild 模式会先删除再重建 Generated 目录）
            ret = generateSource(path, opts.IsRebuild, opts.DumpAST);

            // 在 generateSource 之后写入 .deps 文件（确保不会被 rebuild 的目录删除覆盖）
            for (const auto &pf : pendingFiles)
            {
                if (pf.processed)
                {
                    const StringList &deps = mGenerator->getFileDependencies(pf.fileTitle);
                    writeDepsFile(pf.depsFile, pf.filePath, deps);
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
            kExpectGeneratedPath = 0x00000800
        };

        uint32_t mState;
    };

    const std::string JsonHandler::kTagIncludePath = "IncludePath";
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
            else
            {
#ifdef T3D_RP_DEBUG
                mArgs.push_back("-IC:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Professional\\VC\\Tools\\MSVC\\14.29.30133\\include");
                args.push_back(mArgs.back().c_str());
                mArgs.push_back("-IC:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.19041.0\\ucrt");
                args.push_back(mArgs.back().c_str());
#endif
            }
            
            fs.close();
        }
        else
        {
            RP_LOG_ERROR("The file %s did not exist !", filename.c_str());
        }
        
        return args;
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

    String ReflectionPreprocessor::detectAndGeneratePCH(const String &generatedPath, const ClangArgs &args)
    {
        if (mPrerequisitesHeader.empty())
        {
            RP_LOG_INFO("[PCH] No prerequisites header found, skipping PCH generation.");
            return String();
        }

        // PCH 文件放在 generatedPath 下
        String pchPath = generatedPath + Dir::getNativeSeparator() + "prereq.pch";

        // 检查 PCH 是否已存在且比头文件新
        long_t pchTime = Dir::getLastWriteTime(pchPath);
        long_t hdrTime = Dir::getLastWriteTime(mPrerequisitesHeader);
        if (pchTime > 0 && pchTime >= hdrTime)
        {
            RP_LOG_INFO("[PCH] Reusing existing PCH: %s", pchPath.c_str());
            return pchPath;
        }

        TResult ret = ReflectionGenerator::generatePCH(mPrerequisitesHeader, pchPath, args);
        if (T3D_FAILED(ret))
        {
            RP_LOG_WARNING("[PCH] PCH generation failed, will proceed without PCH.");
            return String();
        }

        return pchPath;
    }

    //-------------------------------------------------------------------------
}
