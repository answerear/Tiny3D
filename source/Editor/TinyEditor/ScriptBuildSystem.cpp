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


#include "ScriptBuildSystem.h"

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
    const char *ScriptBuildSystem::BUILD_CONFIG = "Release";
#else
    const char *ScriptBuildSystem::BUILD_CONFIG = "Debug";
#endif

    //--------------------------------------------------------------------------

    void ScriptBuildSystem::attachProject(const String &projectPath,
        const String &pluginName, const String &scriptsRelativePath)
    {
        const String sep(1, Dir::getNativeSeparator());

        mProjectPath = projectPath;
        mPluginName = pluginName;
        mScriptsDir = projectPath + sep
            + (scriptsRelativePath.empty() ? String("Scripts") : scriptsRelativePath);
        mShadowDir = projectPath + sep + "Temp" + sep + "ShadowAssemblies";
        mLastOutput.clear();
    }

    //--------------------------------------------------------------------------

    void ScriptBuildSystem::detachProject()
    {
        mProjectPath.clear();
        mPluginName.clear();
        mScriptsDir.clear();
        mShadowDir.clear();
        mLastOutput.clear();
    }

    //--------------------------------------------------------------------------

    bool ScriptBuildSystem::hasScripts() const
    {
        if (mScriptsDir.empty() || mPluginName.empty())
        {
            return false;
        }

        // 老工程可能是在支持业务插件之前建的，没有 Scripts 目录，这种情况下
        // 整条业务代码链路直接跳过，不影响纯资源工程的正常使用
        return Dir::exists(mScriptsDir);
    }

    //--------------------------------------------------------------------------

    String ScriptBuildSystem::getAssemblyName(Variant variant) const
    {
        return (variant == Variant::kEditor) ? (mPluginName + "Editor") : mPluginName;
    }

    //--------------------------------------------------------------------------

    String ScriptBuildSystem::getAssemblyDir(Variant variant) const
    {
        const String sep(1, Dir::getNativeSeparator());
        const String sub = (variant == Variant::kEditor) ? "Editor" : "Runtime";
        return mProjectPath + sep + "Library" + sep + "ScriptAssemblies" + sep + sub;
    }

    //--------------------------------------------------------------------------

    String ScriptBuildSystem::getAssemblyPath(Variant variant) const
    {
        return getAssemblyDir(variant) + Dir::getNativeSeparator()
            + platformLibFileName(getAssemblyName(variant));
    }

    //--------------------------------------------------------------------------

    String ScriptBuildSystem::getCMakeSourceDir(Variant variant) const
    {
        const String sub = (variant == Variant::kEditor) ? "Editor" : "Runtime";
        return mScriptsDir + Dir::getNativeSeparator() + sub;
    }

    //--------------------------------------------------------------------------

    String ScriptBuildSystem::getCMakeBuildDir(Variant variant) const
    {
        const String sep(1, Dir::getNativeSeparator());
        const String sub = (variant == Variant::kEditor) ? "Editor" : "Runtime";
        return mProjectPath + sep + "Temp" + sep + "ScriptBuild" + sep + sub;
    }

    //--------------------------------------------------------------------------

    String ScriptBuildSystem::platformLibFileName(const String &name)
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

    String ScriptBuildSystem::platformSymbolFileName(const String &name)
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

    String ScriptBuildSystem::quote(const String &value)
    {
        if (value.find(' ') == String::npos)
        {
            return value;
        }

        return "\"" + value + "\"";
    }

    //--------------------------------------------------------------------------

    long_t ScriptBuildSystem::getNewestSourceTime(const String &dir)
    {
        long_t newest = 0;

        Dir finder;
        String pattern = dir + Dir::getNativeSeparator() + "*";

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

    bool ScriptBuildSystem::needsBuild(Variant variant) const
    {
        if (!hasScripts())
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

        return getNewestSourceTime(mScriptsDir) > assemblyTime;
    }

    //--------------------------------------------------------------------------

    bool ScriptBuildSystem::loadSDKConfig()
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

    String ScriptBuildSystem::parseSDKValue(const String &name) const
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

    TResult ScriptBuildSystem::configure(Variant variant, String &output)
    {
        if (!loadSDKConfig())
        {
            return T3D_ERR_FILE_NOT_EXIST;
        }

        const String buildDir = getCMakeBuildDir(variant);

        if (!Dir::exists(buildDir) && !Dir::makeDirs(buildDir))
        {
            EDITOR_LOG_ERROR("Failed to create script build dir [%s] !", buildDir.c_str());
            return T3D_ERR_FAIL;
        }

        String cmd = "cmake";
        cmd += " -S " + quote(getCMakeSourceDir(variant));
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

        // 单配置生成器（Ninja / Unix Makefiles）只认 configure 期的 CMAKE_BUILD_TYPE
        // 且忽略 --config，多配置生成器（VS / Xcode）则相反。两个都给，任何生成器下
        // 都能拿到正确配置。
        cmd += String(" -DCMAKE_BUILD_TYPE=") + BUILD_CONFIG;

        EDITOR_LOG_INFO("Configuring game plugin : %s", cmd.c_str());

        return runCommand(cmd, mScriptsDir, output);
    }

    //--------------------------------------------------------------------------

    TResult ScriptBuildSystem::build(Variant variant, String &output)
    {
        output.clear();
        mLastOutput.clear();

        if (!hasScripts())
        {
            EDITOR_LOG_WARNING("No game plugin scripts to build.");
            return T3D_ERR_NOT_FOUND;
        }

        const String buildDir = getCMakeBuildDir(variant);

        // CMake 自己会在 build 时检测 CMakeLists 变更并重跑 configure，所以只有
        // 构建目录还不存在（首次构建、或用户清过 Temp）时才需要显式 configure
        const String cacheFile = buildDir + Dir::getNativeSeparator() + "CMakeCache.txt";

        if (!Dir::exists(cacheFile))
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
        }

        String cmd = "cmake --build " + quote(buildDir) + " --config " + BUILD_CONFIG;

        EDITOR_LOG_INFO("Building game plugin : %s", cmd.c_str());

        String buildOutput;
        TResult ret = runCommand(cmd, mScriptsDir, buildOutput);

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

    TResult ScriptBuildSystem::shadowCopy(String &shadowDir, String &shadowName)
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
        // PDB 的文件名而非绝对路径，调试器会在 DLL 同目录也就是影子目录里找到它。
        const String symbol = platformSymbolFileName(getAssemblyName(Variant::kEditor));

        if (!symbol.empty())
        {
            const String srcSymbol = getAssemblyDir(Variant::kEditor)
                + Dir::getNativeSeparator() + symbol;
            const String dstSymbol = mShadowDir + Dir::getNativeSeparator()
                + platformSymbolFileName(name);

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

    void ScriptBuildSystem::cleanShadowAssemblies(const String &keepName)
    {
        if (mShadowDir.empty() || !Dir::exists(mShadowDir))
        {
            return;
        }

        const String keepLib = keepName.empty() ? String() : platformLibFileName(keepName);
        const String keepSymbol = keepName.empty() ? String() : platformSymbolFileName(keepName);

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

#if defined (T3D_OS_WINDOWS)

    TResult ScriptBuildSystem::runCommand(const String &cmdLine, const String &workDir,
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

    TResult ScriptBuildSystem::runCommand(const String &cmdLine, const String &workDir,
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
