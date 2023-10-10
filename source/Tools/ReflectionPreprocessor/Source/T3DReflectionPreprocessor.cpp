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

#include "T3DReflectionPreprocessor.h"
#include "T3DPreprocessorCommand.h"
#include "T3DPreprocessorOptions.h"
#include "T3DReflectionGenerator.h"


namespace  Tiny3D
{
    //-------------------------------------------------------------------------

    const String ReflectionPreprocessor::kReflectionSettingsFile = "ReflectionSettings.json";
    
    //-------------------------------------------------------------------------
    
    ReflectionPreprocessor::ReflectionPreprocessor()
        : mGenerator(new ReflectionGenerator())
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
            mGenerator->setBuiltinClass(whitelist);

            // 设置工程路径
            mGenerator->setProjectPath(opts.SourcePath);

            // 分析头文件包含路径并记录
            mGenerator->parseProjectHeaderPath(args);

            // 收集项目头文件信息
            ret = collectProjectHeaders(opts.SourcePath);
            if (T3D_FAILED(ret))
            {
                break;
            }

            String path = opts.SourcePath + Dir::getNativeSeparator() + mGeneratedPath;
            
            // 根据对应路径，遍历路径里的源文件，逐个产生抽象语法树
            ret = generateAST(opts.SourcePath, args, path, opts.IsRebuild);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 生成源码文件
            ret = generateSource(path, opts.IsRebuild);
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
                }
            }

            working = dir.findNextFile();
        }

        dir.close();
        
        return ret;
    }
    
    //-------------------------------------------------------------------------

    TResult ReflectionPreprocessor::generateAST(const String &path, const ClangArgs &args, const String &generatedPath, bool rebuild)
    {
        TResult ret = T3D_OK;

        String searchPath = path + Dir::getNativeSeparator() + "*.*";
        
        Dir dir;

        // 分析源码文件，生成 AST
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
                generateAST(dir.getFilePath(), args, generatedPath, rebuild);
            }
            else
            {
                // file
                const String filePath = dir.getFilePath();
                String fileDir, fileTitle, fileExt;
                Dir::parsePath(filePath, fileDir, fileTitle, fileExt);
                if (fileExt == "cpp" || fileExt == "cxx")
                {
                    String generatedFile = generatedPath + Dir::getNativeSeparator() + fileTitle + ".generated.cpp";
                    long_t srcLastWTime = Dir::getLastWriteTime(filePath);
                    long_t genLastWTime = Dir::getLastWriteTime(generatedFile);
                    if (rebuild || genLastWTime < srcLastWTime)
                    {
                        // 反射文件文件不存在，或者反射生成文件比源码文件还旧，则重新生成
                        mGenerator->generateAST(filePath, args);
                    }
                    else
                    {
                        RP_LOG_INFO(">>> %s updated !", filePath.c_str());
                    }
                }
            }

            working = dir.findNextFile();
        }

        dir.close();

        return ret;
    }

    //-------------------------------------------------------------------------

    TResult ReflectionPreprocessor::generateSource(const String& path, bool rebuild)
    {
        if (rebuild)
        {
            Dir::removeDir(path, true);
            Dir::makeDir(path);
        }
        
        // 输出 AST 到文件，方便 debug
        String dumpPath = path + Dir::getNativeSeparator() + "ast.json";
        mGenerator->dumpReflectionInfo(dumpPath);

        return mGenerator->generateSource(path);
    }

    //-------------------------------------------------------------------------
}
