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


#include "CppBuildSystem.h"

#include <algorithm>
#include <cstdio>

#if defined (T3D_OS_WINDOWS)
    #include <windows.h>
#endif


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------

    // 业务插件必须和编辑器用同一个配置，Debug 版编辑器加载 Release 版业务 DLL
    // 会因为运行时库不同而崩在跨 DLL 的内存释放上。
#if defined (NDEBUG)
    const char *CppBuildSystem::BUILD_CONFIG = "Release";
#else
    const char *CppBuildSystem::BUILD_CONFIG = "Debug";
#endif

    //--------------------------------------------------------------------------

    void CppBuildSystem::attachProject(const String &projectPath,
        const String &pluginName, const String &cppSourceRelativePath)
    {
        const String sep(1, Dir::getNativeSeparator());
        const String relative = cppSourceRelativePath.empty()
            ? (String("Assets") + sep + "Source")
            : cppSourceRelativePath;

        mProjectPath = Dir::formatPath(projectPath);
        mPluginName = pluginName;
        // 设置里的相对路径可能用 '/'，formatPath 统一成本机分隔符
        mCppSourceDir = Dir::formatPath(mProjectPath + sep + relative);
        mShadowDir = Dir::formatPath(mProjectPath + sep + "Temp" + sep + "ShadowAssemblies");
        mLastOutput.clear();
    }

    //--------------------------------------------------------------------------

    void CppBuildSystem::detachProject()
    {
        mProjectPath.clear();
        mPluginName.clear();
        mCppSourceDir.clear();
        mShadowDir.clear();
        mLastOutput.clear();
    }

    //--------------------------------------------------------------------------

    bool CppBuildSystem::hasCppSources() const
    {
        if (mCppSourceDir.empty() || mPluginName.empty())
        {
            return false;
        }

        // 纯资源工程可以没有业务 C++ 源码目录，这种情况下整条业务代码链路
        // 直接跳过，不影响工程的正常使用
        return Dir::exists(mCppSourceDir);
    }

    //--------------------------------------------------------------------------

    String CppBuildSystem::getAssemblyName(Variant variant) const
    {
        return (variant == Variant::kEditor) ? (mPluginName + "Editor") : mPluginName;
    }

    //--------------------------------------------------------------------------

    String CppBuildSystem::getAssemblyDir(Variant variant) const
    {
        const String sep(1, Dir::getNativeSeparator());
        const String sub = (variant == Variant::kEditor) ? "Editor" : "Runtime";
        return mProjectPath + sep + "Library" + sep + "CppAssemblies" + sep + sub;
    }

    //--------------------------------------------------------------------------

    String CppBuildSystem::getAssemblyPath(Variant variant) const
    {
        return getAssemblyDir(variant) + Dir::getNativeSeparator()
            + platformLibFileName(getAssemblyName(variant));
    }

    //--------------------------------------------------------------------------

    String CppBuildSystem::getCMakeSourceDir(Variant variant) const
    {
        const String sub = (variant == Variant::kEditor) ? "Editor" : "Runtime";
        return mCppSourceDir + Dir::getNativeSeparator() + sub;
    }

    //--------------------------------------------------------------------------

    String CppBuildSystem::getCMakeBuildDir(Variant variant) const
    {
        const String sep(1, Dir::getNativeSeparator());
        const String sub = (variant == Variant::kEditor) ? "Editor" : "Runtime";
        return mProjectPath + sep + "Temp" + sep + "CppBuild" + sep + sub;
    }

    //--------------------------------------------------------------------------

    String CppBuildSystem::platformLibFileName(const String &name)
    {
        // 与 Dylib::onLoad 的拼接规则保持一致，否则影子副本加载不到
#if defined (T3D_OS_WINDOWS)
        return name + ".dll";
#elif defined (T3D_OS_LINUX) || defined (T3D_OS_ANDROID)
        return "lib" + name + ".so";
#else
        return "lib" + name + ".dylib";
#endif
    }

    //--------------------------------------------------------------------------

    String CppBuildSystem::platformSymbolFileName(const String &name)
    {
        // Linux 的调试信息内嵌在 .so 里，macOS 的 dSYM 是目录且默认不生成，
        // 只有 MSVC 的 PDB 需要跟着影子副本一起搬
#if defined (T3D_OS_WINDOWS)
        return name + ".pdb";
#else
        return String();
#endif
    }

    //--------------------------------------------------------------------------

    String CppBuildSystem::quote(const String &value)
    {
        if (value.find(' ') == String::npos)
        {
            return value;
        }

        return "\"" + value + "\"";
    }

    //--------------------------------------------------------------------------

    long_t CppBuildSystem::getNewestSourceTime(const String &dir)
    {
        long_t newest = 0;

        String normalized = Dir::formatPath(dir);
        while (!normalized.empty()
            && (normalized.back() == '/' || normalized.back() == '\\'))
        {
            normalized.pop_back();
        }

        Dir finder;
        String pattern = normalized + Dir::getNativeSeparator() + "*";

        if (!finder.findFile(pattern))
        {
            return newest;
        }

        while (finder.findNextFile())
        {
            if (finder.isDots())
            {
                continue;
            }

            if (finder.isDirectory())
            {
                newest = std::max(newest, getNewestSourceTime(finder.getFilePath()));
                continue;
            }

            const String name = finder.getFileName();
            const size_t dot = name.rfind('.');
            const String ext = (dot == String::npos) ? String() : name.substr(dot);

            // CMakeLists.txt 没有可判别的扩展名，单独认一下
            if (ext == ".h" || ext == ".hpp" || ext == ".cpp" || ext == ".cc"
                || ext == ".inl" || ext == ".cmake" || name == "CMakeLists.txt")
            {
                newest = std::max(newest, finder.getLastWriteTime());
            }
        }

        finder.close();

        return newest;
    }

    //--------------------------------------------------------------------------

    bool CppBuildSystem::needsBuild(Variant variant) const
    {
        if (!hasCppSources())
        {
            return false;
        }

        const String assembly = getAssemblyPath(variant);

        if (!Dir::exists(assembly))
        {
            return true;
        }

        // 比的是原始产物，不是影子副本——影子副本每次加载都会重拷，时间戳没有参考意义
        const long_t assemblyTime = Dir::getLastWriteTime(assembly);

        return getNewestSourceTime(mCppSourceDir) > assemblyTime;
    }

    //--------------------------------------------------------------------------

    String CppBuildSystem::getCppFileStampPath(Variant variant) const
    {
        return getCMakeBuildDir(variant) + Dir::getNativeSeparator()
            + ".cpp_files.stamp";
    }

    //--------------------------------------------------------------------------

    String CppBuildSystem::collectCppFileList() const
    {
        TArray<String> names;
        const String sep(1, Dir::getNativeSeparator());
        static const char *kExts[] =
        {
            ".h", ".hpp", ".hh", ".cpp", ".cc", ".cxx", nullptr
        };

        if (!Dir::exists(mCppSourceDir))
        {
            return String();
        }

        Dir finder;
        const String pattern = mCppSourceDir + sep + "*";
        bool working = finder.findFile(pattern);
        while (working)
        {
            if (!finder.isDots() && !finder.isDirectory())
            {
                const String name = finder.getFileName();
                const size_t dot = name.rfind('.');
                const String ext = (dot == String::npos)
                    ? String() : name.substr(dot);
                for (int i = 0; kExts[i] != nullptr; ++i)
                {
                    if (ext == kExts[i])
                    {
                        names.push_back(name);
                        break;
                    }
                }
            }

            working = finder.findNextFile();
        }
        finder.close();

        std::sort(names.begin(), names.end());

        String list;
        for (const auto &n : names)
        {
            list += n;
            list += '\n';
        }
        return list;
    }

    //--------------------------------------------------------------------------

    bool CppBuildSystem::needsReconfigure(Variant variant) const
    {
        const String cacheFile = getCMakeBuildDir(variant)
            + Dir::getNativeSeparator() + "CMakeCache.txt";
        if (!Dir::exists(cacheFile))
        {
            return true;
        }

        const String stampPath = getCppFileStampPath(variant);
        if (!Dir::exists(stampPath))
        {
            return true;
        }

        FileDataStream fs;
        if (!fs.open(stampPath.c_str(), FileDataStream::E_MODE_READ_ONLY))
        {
            return true;
        }

        const size_t size = static_cast<size_t>(fs.size());
        String previous;
        previous.resize(size);
        if (size > 0)
        {
            fs.read(&previous[0], size);
        }
        fs.close();

        return previous != collectCppFileList();
    }

    //--------------------------------------------------------------------------

    void CppBuildSystem::writeCppFileStamp(Variant variant,
        const String &list) const
    {
        const String stampPath = getCppFileStampPath(variant);
        FileDataStream fs;
        if (!fs.open(stampPath.c_str(),
            FileDataStream::E_MODE_TRUNCATE | FileDataStream::E_MODE_WRITE_ONLY))
        {
            EDITOR_LOG_WARNING("Failed to write C++ file stamp [%s] !",
                stampPath.c_str());
            return;
        }

        if (!list.empty())
        {
            fs.write((void*)list.data(), list.size());
        }
        fs.close();
    }

    //--------------------------------------------------------------------------

    bool CppBuildSystem::loadSDKConfig()
    {
        if (mSDKConfigLoaded)
        {
            return !mSDKConfig.empty();
        }

        mSDKConfigLoaded = true;

        const String path = Dir::getAppPath() + Dir::getNativeSeparator() + "Tiny3DSDK.cmake";

        if (!Dir::exists(path))
        {
            EDITOR_LOG_ERROR("Tiny3DSDK.cmake not found at [%s]. Game plugin cannot be "
                "built without it, because the toolchain must match the editor's.",
                path.c_str());
            return false;
        }

        FileDataStream fs;
        if (!fs.open(path.c_str(), FileDataStream::E_MODE_READ_ONLY))
        {
            EDITOR_LOG_ERROR("Failed to open [%s] !", path.c_str());
            return false;
        }

        const size_t size = static_cast<size_t>(fs.size());
        mSDKConfig.resize(size);
        fs.read(&mSDKConfig[0], size);
        fs.close();

        return !mSDKConfig.empty();
    }

    //--------------------------------------------------------------------------

    String CppBuildSystem::parseSDKValue(const String &name) const
    {
        // 文件是生成出来的，格式固定为 set(NAME "value")，用不着完整的 CMake 解析
        const String key = "set(" + name;

        size_t pos = mSDKConfig.find(key);
        if (pos == String::npos)
        {
            return String();
        }

        const size_t open = mSDKConfig.find('"', pos);
        if (open == String::npos)
        {
            return String();
        }

        const size_t close = mSDKConfig.find('"', open + 1);
        if (close == String::npos)
        {
            return String();
        }

        return mSDKConfig.substr(open + 1, close - open - 1);
    }

    //--------------------------------------------------------------------------

    String CppBuildSystem::buildCMakeConfigureCommand(const String &sourceDir,
        const String &buildDir) const
    {
        String cmd = "cmake";
        cmd += " -S " + quote(sourceDir);
        cmd += " -B " + quote(buildDir);

        // 工具链三件套。这几项没法在 CMake 内部设置，只能走命令行，而它们的值来自
        // 编辑器构建时导出的 SDK 配置，所以这里读到什么就传什么，不作平台判断。
        const String generator = parseSDKValue("TINY3D_SDK_GENERATOR");
        const String platform = parseSDKValue("TINY3D_SDK_GENERATOR_PLATFORM");
        const String toolset = parseSDKValue("TINY3D_SDK_GENERATOR_TOOLSET");
        const String compiler = parseSDKValue("TINY3D_SDK_CXX_COMPILER");

        if (!generator.empty())
        {
            cmd += " -G " + quote(generator);
        }

        if (!platform.empty())
        {
            cmd += " -A " + quote(platform);
        }

        if (!toolset.empty())
        {
            cmd += " -T " + quote(toolset);
        }

        if (!compiler.empty())
        {
            cmd += " -DCMAKE_CXX_COMPILER=" + quote(compiler);
        }

        cmd += " -DTINY3D_SDK_ROOT=" + quote(Dir::getAppPath());
        // 源码可能在 Assets/Source，不能靠目录上一级推工程根；显式传入
        cmd += " -DGAME_PROJECT_ROOT=" + quote(mProjectPath);

        // 单配置生成器（Ninja / Unix Makefiles）只认 configure 期的 CMAKE_BUILD_TYPE
        // 且忽略 --config，多配置生成器（VS / Xcode）则相反。两个都给，任何生成器下
        // 都能拿到正确配置。
        cmd += String(" -DCMAKE_BUILD_TYPE=") + BUILD_CONFIG;

        return cmd;
    }

    //--------------------------------------------------------------------------

    TResult CppBuildSystem::configure(Variant variant, String &output)
    {
        if (!loadSDKConfig())
        {
            return T3D_ERR_FILE_NOT_EXIST;
        }

        const String buildDir = getCMakeBuildDir(variant);

        if (!Dir::exists(buildDir) && !Dir::makeDirs(buildDir))
        {
            EDITOR_LOG_ERROR("Failed to create C++ build dir [%s] !", buildDir.c_str());
            return T3D_ERR_FAIL;
        }

        const String cmd = buildCMakeConfigureCommand(getCMakeSourceDir(variant), buildDir);

        EDITOR_LOG_INFO("Configuring game plugin : %s", cmd.c_str());

        return runCommand(cmd, mCppSourceDir, output);
    }

    //--------------------------------------------------------------------------

    TResult CppBuildSystem::build(Variant variant, String &output)
    {
        output.clear();
        mLastOutput.clear();

        if (!hasCppSources())
        {
            EDITOR_LOG_WARNING("No game plugin C++ sources to build.");
            return T3D_ERR_NOT_FOUND;
        }

        const String buildDir = getCMakeBuildDir(variant);

        // Assets/Source 根下的 .h / .cpp 集合变了必须重新 configure：file(GLOB) 和 rpp
        // 新产出的 *.generated.cpp 都要进工程。只改已有文件内容则走增量 rpp。
        if (needsReconfigure(variant))
        {
            String configureOutput;
            TResult ret = configure(variant, configureOutput);

            output += configureOutput;

            if (T3D_FAILED(ret))
            {
                mLastOutput = output;
                EDITOR_LOG_ERROR("Configure game plugin failed :\n%s", output.c_str());
                return ret;
            }

            writeCppFileStamp(variant, collectCppFileList());
        }

        String cmd = "cmake --build " + quote(buildDir) + " --config " + BUILD_CONFIG;

        EDITOR_LOG_INFO("Building game plugin : %s", cmd.c_str());

        String buildOutput;
        TResult ret = runCommand(cmd, mCppSourceDir, buildOutput);

        output += buildOutput;
        mLastOutput = output;

        if (T3D_FAILED(ret))
        {
            EDITOR_LOG_ERROR("Build game plugin failed :\n%s", output.c_str());
        }
        else
        {
            EDITOR_LOG_INFO("Build game plugin succeeded.");
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult CppBuildSystem::shadowCopy(String &shadowDir, String &shadowName)
    {
        const String assembly = getAssemblyPath(Variant::kEditor);

        if (!Dir::exists(assembly))
        {
            EDITOR_LOG_ERROR("Game plugin assembly [%s] does not exist !", assembly.c_str());
            return T3D_ERR_FILE_NOT_EXIST;
        }

        if (!Dir::exists(mShadowDir) && !Dir::makeDirs(mShadowDir))
        {
            EDITOR_LOG_ERROR("Failed to create shadow dir [%s] !", mShadowDir.c_str());
            return T3D_ERR_FAIL;
        }

        // 时间戳取产物的最后修改时间而不是当前时间：同一份产物重复打开工程时影子名
        // 相同，可以直接复用已有副本；产物一变名字就变，不会撞上还被锁着的旧副本。
        const long_t stamp = Dir::getLastWriteTime(assembly);
        const String name = getAssemblyName(Variant::kEditor) + "_" + std::to_string(stamp);
        const String dst = mShadowDir + Dir::getNativeSeparator() + platformLibFileName(name);

        if (!Dir::exists(dst) && !Dir::copy(assembly, dst, true))
        {
            EDITOR_LOG_ERROR("Failed to shadow copy [%s] -> [%s] !",
                assembly.c_str(), dst.c_str());
            return T3D_ERR_COPY_FILE;
        }

        // 符号文件不跟着走的话断点会失效。模板里配了 /PDBALTPATH，DLL 内记录的是
        // 原始 PDB 文件名（{Name}Editor.pdb）而不是绝对路径。影子 DLL 虽然改了名，
        // PDB 必须保持原名，调试器才能在影子目录里对上。
        const String symbol = platformSymbolFileName(getAssemblyName(Variant::kEditor));

        if (!symbol.empty())
        {
            const String srcSymbol = getAssemblyDir(Variant::kEditor)
                + Dir::getNativeSeparator() + symbol;
            const String dstSymbol = mShadowDir + Dir::getNativeSeparator() + symbol;

            if (Dir::exists(srcSymbol) && !Dir::exists(dstSymbol))
            {
                if (!Dir::copy(srcSymbol, dstSymbol, true))
                {
                    // 符号拷不过去只影响调试体验，不该拦住加载
                    EDITOR_LOG_WARNING("Failed to shadow copy debug symbol [%s].",
                        srcSymbol.c_str());
                }
            }
        }

        shadowDir = mShadowDir;
        shadowName = name;

        EDITOR_LOG_INFO("Shadow copied game plugin to [%s].", dst.c_str());

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void CppBuildSystem::cleanShadowAssemblies(const String &keepName)
    {
        if (mShadowDir.empty() || !Dir::exists(mShadowDir))
        {
            return;
        }

        const String keepLib = keepName.empty() ? String() : platformLibFileName(keepName);
        // PDB 按原始程序集名存放，不带影子 stamp
        const String keepSymbol = keepName.empty()
            ? String()
            : platformSymbolFileName(getAssemblyName(Variant::kEditor));

        // 边遍历边删可能干扰枚举，先收集再删
        TArray<String> victims;

        Dir finder;
        if (finder.findFile(mShadowDir + Dir::getNativeSeparator() + "*"))
        {
            while (finder.findNextFile())
            {
                if (finder.isDots() || finder.isDirectory())
                {
                    continue;
                }

                const String name = finder.getFileName();

                if (!keepLib.empty() && (name == keepLib || name == keepSymbol))
                {
                    continue;
                }

                victims.push_back(finder.getFilePath());
            }

            finder.close();
        }

        for (const String &victim : victims)
        {
            // 删不掉是常态而非异常：调试器可能还持有 PDB，杀毒软件可能正在扫描。
            // 影子名带时间戳就是为了让删不掉的旧副本也不挡住下一次加载。
            if (!Dir::remove(victim))
            {
                EDITOR_LOG_DEBUG("Shadow assembly [%s] is still locked, leaving it for "
                    "the next cleanup.", victim.c_str());
            }
        }
    }

    //--------------------------------------------------------------------------

    String CppBuildSystem::getIDEBuildDir() const
    {
        const String sep(1, Dir::getNativeSeparator());
        return mProjectPath + sep + "Temp" + sep + "CppIDE";
    }

    //--------------------------------------------------------------------------

    bool CppBuildSystem::findSolutionFile(const String &buildDir, String &slnPath)
    {
        slnPath.clear();

        Dir finder;
        const String pattern = buildDir + Dir::getNativeSeparator() + "*.sln";
        bool working = finder.findFile(pattern);
        while (working)
        {
            if (!finder.isDots() && !finder.isDirectory())
            {
                slnPath = finder.getFilePath();
                finder.close();
                return true;
            }
            working = finder.findNextFile();
        }
        finder.close();
        return false;
    }

    //--------------------------------------------------------------------------

    bool CppBuildSystem::needsIDEReconfigure() const
    {
        const String buildDir = getIDEBuildDir();
        const String cacheFile = buildDir + Dir::getNativeSeparator() + "CMakeCache.txt";
        if (!Dir::exists(cacheFile))
        {
            return true;
        }

        String slnPath;
        if (!findSolutionFile(buildDir, slnPath))
        {
            // 缓存在但没有 sln：可能是 Ninja 生成器，再 configure 一次也变不出 sln
            return false;
        }

        if (getNewestSourceTime(mCppSourceDir) > Dir::getLastWriteTime(cacheFile))
        {
            return true;
        }

        // 源码没动但工程文件坏了（例如被历史版本的路径改写逻辑写坏），
        // 让 cmake 重新生成一遍把它覆盖掉，否则 VS 永远打不开这个工程
        return hasCorruptedGeneratedProject(buildDir);
    }

    //--------------------------------------------------------------------------

    TResult CppBuildSystem::configureIDE(String &output)
    {
        if (!loadSDKConfig())
        {
            return T3D_ERR_FILE_NOT_EXIST;
        }

        const String buildDir = getIDEBuildDir();

        if (!Dir::exists(buildDir) && !Dir::makeDirs(buildDir))
        {
            EDITOR_LOG_ERROR("Failed to create IDE build dir [%s] !", buildDir.c_str());
            return T3D_ERR_FAIL;
        }

        const String cmd = buildCMakeConfigureCommand(mCppSourceDir, buildDir);

        EDITOR_LOG_INFO("Configuring C++ IDE solution : %s", cmd.c_str());

        return runCommand(cmd, mCppSourceDir, output);
    }

    //--------------------------------------------------------------------------

    TResult CppBuildSystem::ensureIDESolution(String &slnPath, String &output)
    {
        slnPath.clear();
        output.clear();
        mLastOutput.clear();

        if (!hasCppSources())
        {
            EDITOR_LOG_WARNING("No game plugin C++ sources to open in the IDE.");
            return T3D_ERR_NOT_FOUND;
        }

        const String cmakeLists = mCppSourceDir + Dir::getNativeSeparator() + "CMakeLists.txt";
        if (!Dir::exists(cmakeLists))
        {
            EDITOR_LOG_ERROR("Game plugin CMakeLists.txt not found at [%s] !",
                cmakeLists.c_str());
            return T3D_ERR_FILE_NOT_EXIST;
        }

        // 引擎托管的 CMake 从模板同步，不要去改已经生成的游戏工程。
        const bool templateSynced = syncIDECMakeFromTemplate();

        if (templateSynced || needsIDEReconfigure())
        {
            TResult ret = configureIDE(output);
            mLastOutput = output;
            if (T3D_FAILED(ret))
            {
                EDITOR_LOG_ERROR("Failed to configure C++ IDE solution !\n%s",
                    output.c_str());
                return ret;
            }
        }

        const String buildDir = getIDEBuildDir();
        if (!findSolutionFile(buildDir, slnPath))
        {
            EDITOR_LOG_ERROR("No .sln was generated in [%s]. The editor toolchain "
                "must use a Visual Studio CMake generator to open a C++ solution.",
                buildDir.c_str());
            return T3D_ERR_FILE_NOT_EXIST;
        }

        // cmake 内部路径是 /，VS 调试页在 Windows 上必须是 '\'
        fixGeneratedVsDebuggerPaths(buildDir);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    bool CppBuildSystem::readTextFile(const String &path, String &text)
    {
        FileDataStream fs;
        if (!fs.open(path.c_str(), FileDataStream::E_MODE_READ_ONLY))
        {
            return false;
        }

        const size_t size = static_cast<size_t>(fs.size());
        text.resize(size);
        if (size > 0)
        {
            fs.read(&text[0], size);
        }
        fs.close();
        return true;
    }

    //--------------------------------------------------------------------------

    bool CppBuildSystem::writeTextFile(const String &path, const String &text)
    {
        FileDataStream fs;
        if (!fs.open(path.c_str(), FileDataStream::E_MODE_TRUNCATE
            | FileDataStream::E_MODE_WRITE_ONLY))
        {
            return false;
        }

        if (!text.empty())
        {
            fs.write(const_cast<char *>(text.data()), text.size());
        }
        fs.close();
        return true;
    }

    //--------------------------------------------------------------------------

    bool CppBuildSystem::syncTemplateFile(const String &relativePath) const
    {
        const String sep(1, Dir::getNativeSeparator());
        const String src = Dir::getAppPath() + sep + "Editor" + sep + "templates"
            + sep + "GamePlugin" + sep + relativePath;
        const String dst = mCppSourceDir + sep + relativePath;

        if (!Dir::exists(src))
        {
            return false;
        }

        String content;
        if (!readTextFile(src, content))
        {
            EDITOR_LOG_WARNING("Failed to read CMake template [%s] !", src.c_str());
            return false;
        }

        static const String kPlaceholder = "{ProjectName}";
        size_t pos = content.find(kPlaceholder);
        while (pos != String::npos)
        {
            content.replace(pos, kPlaceholder.size(), mPluginName);
            pos = content.find(kPlaceholder, pos + mPluginName.size());
        }

        String existing;
        if (readTextFile(dst, existing) && existing == content)
        {
            return false;
        }

        const size_t slash = dst.find_last_of("/\\");
        if (slash != String::npos)
        {
            const String dir = dst.substr(0, slash);
            if (!Dir::exists(dir) && !Dir::makeDirs(dir))
            {
                EDITOR_LOG_WARNING("Failed to create [%s] for CMake template sync !",
                    dir.c_str());
                return false;
            }
        }

        if (!writeTextFile(dst, content))
        {
            EDITOR_LOG_WARNING("Failed to write CMake template [%s] !", dst.c_str());
            return false;
        }

        EDITOR_LOG_INFO("Synced CMake template [%s] -> [%s].", src.c_str(), dst.c_str());
        return true;
    }

    //--------------------------------------------------------------------------

    bool CppBuildSystem::syncIDECMakeFromTemplate() const
    {
        bool changed = syncTemplateFile("GamePluginCommon.cmake");
        changed = syncTemplateFile(String("Player") + Dir::getNativeSeparator()
            + "CMakeLists.txt") || changed;
        return changed;
    }

    //--------------------------------------------------------------------------

    void CppBuildSystem::toNativePathSeparators(String &text)
    {
        const char native = Dir::getNativeSeparator();
        const char foreign = (native == '\\') ? '/' : '\\';
        for (size_t i = 0; i < text.size(); ++i)
        {
            if (text[i] == foreign)
            {
                text[i] = native;
            }
        }
    }

    //--------------------------------------------------------------------------

    bool CppBuildSystem::isTagNameEnd(const String &xml, size_t pos)
    {
        if (pos >= xml.size())
        {
            return false;
        }

        const char ch = xml[pos];
        return ch == '>' || ch == '/' || ch == ' ' || ch == '\t'
            || ch == '\r' || ch == '\n';
    }

    //--------------------------------------------------------------------------

    bool CppBuildSystem::rewriteVsDebuggerPathSlashes(String &xml)
    {
        static const char *kTags[] = {
            "LocalDebuggerCommand",
            "LocalDebuggerCommandArguments",
            "LocalDebuggerWorkingDirectory",
            "LocalDebuggerEnvironment",
            nullptr
        };

        bool changed = false;

        for (int i = 0; kTags[i] != nullptr; ++i)
        {
            const String open = String("<") + kTags[i];
            const String close = String("</") + kTags[i] + ">";
            size_t pos = 0;

            while ((pos = xml.find(open, pos)) != String::npos)
            {
                // "<LocalDebuggerCommand" 也是 "<LocalDebuggerCommandArguments" 的前缀，
                // 不卡标签名边界就会把两个标签之间的整段 XML 当成路径值改掉。
                if (!isTagNameEnd(xml, pos + open.size()))
                {
                    pos += open.size();
                    continue;
                }

                const size_t gt = xml.find('>', pos);
                if (gt == String::npos)
                {
                    break;
                }

                const size_t end = xml.find(close, gt + 1);
                if (end == String::npos)
                {
                    break;
                }

                String value = xml.substr(gt + 1, end - (gt + 1));

                // 元素值里出现 '<' 说明匹配到的不是同一个标签的收尾，跳过以免破坏 XML
                if (value.find('<') != String::npos)
                {
                    pos = gt + 1;
                    continue;
                }

                const String before = value;
                toNativePathSeparators(value);
                if (value != before)
                {
                    xml.replace(gt + 1, before.size(), value);
                    changed = true;
                    pos = gt + 1 + value.size() + close.size();
                }
                else
                {
                    pos = end + close.size();
                }
            }
        }

        return changed;
    }

    //--------------------------------------------------------------------------

    void CppBuildSystem::collectFilesBySuffix(const String &dir,
        const String &suffix, TArray<String> &out)
    {
        Dir finder;
        if (!finder.findFile(dir + Dir::getNativeSeparator() + "*"))
        {
            return;
        }

        while (finder.findNextFile())
        {
            if (finder.isDots())
            {
                continue;
            }

            if (finder.isDirectory())
            {
                collectFilesBySuffix(finder.getFilePath(), suffix, out);
                continue;
            }

            const String name = finder.getFileName();
            if (name.size() >= suffix.size()
                && name.compare(name.size() - suffix.size(), suffix.size(), suffix) == 0)
            {
                out.push_back(finder.getFilePath());
            }
        }

        finder.close();
    }

    //--------------------------------------------------------------------------

    bool CppBuildSystem::hasCorruptedGeneratedProject(const String &buildDir)
    {
        TArray<String> files;
        collectFilesBySuffix(buildDir, ".vcxproj", files);
        collectFilesBySuffix(buildDir, ".vcxproj.user", files);

        for (const String &path : files)
        {
            String xml;
            if (!readTextFile(path, xml))
            {
                continue;
            }

            // 合法 XML 里 '<' 只能开始一个标签，"<\" 只可能是被改坏的结束标签
            if (xml.find("<\\") != String::npos)
            {
                EDITOR_LOG_WARNING("Generated project [%s] is malformed, forcing a "
                    "CMake regeneration.", path.c_str());
                return true;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------

    void CppBuildSystem::fixGeneratedVsDebuggerPaths(const String &buildDir) const
    {
        TArray<String> files;
        collectFilesBySuffix(buildDir, ".vcxproj", files);
        collectFilesBySuffix(buildDir, ".vcxproj.user", files);

        for (const String &path : files)
        {
            String xml;
            if (!readTextFile(path, xml))
            {
                continue;
            }

            if (!rewriteVsDebuggerPathSlashes(xml))
            {
                continue;
            }

            if (writeTextFile(path, xml))
            {
                EDITOR_LOG_INFO("Normalized VS debugger paths in [%s].", path.c_str());
            }
            else
            {
                EDITOR_LOG_WARNING("Failed to write normalized debugger paths [%s] !",
                    path.c_str());
            }
        }
    }

    //--------------------------------------------------------------------------

#if defined (T3D_OS_WINDOWS)

    TResult CppBuildSystem::runCommand(const String &cmdLine, const String &workDir,
        String &output)
    {
        SECURITY_ATTRIBUTES sa;
        memset(&sa, 0, sizeof(sa));
        sa.nLength = sizeof(sa);
        sa.bInheritHandle = TRUE;
        sa.lpSecurityDescriptor = nullptr;

        HANDLE readPipe = nullptr;
        HANDLE writePipe = nullptr;

        if (!::CreatePipe(&readPipe, &writePipe, &sa, 0))
        {
            EDITOR_LOG_ERROR("Failed to create pipe for external command !");
            return T3D_ERR_FAIL;
        }

        // 读端不能被子进程继承，否则子进程退出后管道不会关闭，下面的读循环会一直挂着
        ::SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0);

        STARTUPINFOA si;
        memset(&si, 0, sizeof(si));
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;
        si.hStdOutput = writePipe;
        si.hStdError = writePipe;
        si.hStdInput = nullptr;

        PROCESS_INFORMATION pi;
        memset(&pi, 0, sizeof(pi));

        const String ansiCmd = T3D_LOCALE.UTF8ToANSI(cmdLine);
        const String ansiDir = T3D_LOCALE.UTF8ToANSI(workDir);

        TArray<char> mutableCmd(ansiCmd.begin(), ansiCmd.end());
        mutableCmd.push_back('\0');

        // lpApplicationName 传 nullptr 才会按 PATH 查找 cmake；
        // CREATE_NO_WINDOW 避免编辑器界面上闪出控制台窗口
        BOOL ok = ::CreateProcessA(nullptr, mutableCmd.data(), nullptr, nullptr, TRUE,
            CREATE_NO_WINDOW, nullptr, ansiDir.empty() ? nullptr : ansiDir.c_str(),
            &si, &pi);

        ::CloseHandle(writePipe);

        if (!ok)
        {
            ::CloseHandle(readPipe);
            EDITOR_LOG_ERROR("Failed to launch external command [%s], error %lu. "
                "Is cmake installed and on PATH ?", cmdLine.c_str(), ::GetLastError());
            return T3D_ERR_FAIL;
        }

        String raw;
        char buffer[4096];
        DWORD read = 0;

        // 必须边跑边读，管道缓冲区写满时子进程会阻塞在写上，等它退出再读会死锁
        while (::ReadFile(readPipe, buffer, sizeof(buffer), &read, nullptr) && read > 0)
        {
            raw.append(buffer, read);
        }

        ::CloseHandle(readPipe);
        ::WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD exitCode = 1;
        ::GetExitCodeProcess(pi.hProcess, &exitCode);

        ::CloseHandle(pi.hThread);
        ::CloseHandle(pi.hProcess);

        // 编译器输出走的是系统 ANSI 代码页，日志系统要 UTF-8
        output = T3D_LOCALE.ANSIToUTF8(raw);

        return (exitCode == 0) ? T3D_OK : T3D_ERR_FAIL;
    }

#else

    TResult CppBuildSystem::runCommand(const String &cmdLine, const String &workDir,
        String &output)
    {
        String cmd;

        if (!workDir.empty())
        {
            cmd = "cd " + quote(workDir) + " && ";
        }

        // 合并 stderr，编译错误才不会漏掉
        cmd += cmdLine + " 2>&1";

        FILE *pipe = popen(cmd.c_str(), "r");

        if (pipe == nullptr)
        {
            EDITOR_LOG_ERROR("Failed to launch external command [%s]. "
                "Is cmake installed and on PATH ?", cmdLine.c_str());
            return T3D_ERR_FAIL;
        }

        char buffer[4096];

        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            output += buffer;
        }

        const int status = pclose(pipe);

        return (status == 0) ? T3D_OK : T3D_ERR_FAIL;
    }

#endif

    //--------------------------------------------------------------------------

    NS_END
}
