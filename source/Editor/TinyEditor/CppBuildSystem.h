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

#pragma once


#include "EditorPrerequisites.h"

#include <atomic>
#include <mutex>
#include <thread>


namespace Tiny3D
{
    NS_BEGIN(Editor)

    /**
     * @brief 业务 C++ 代码（Game Plugin）的编译与影子拷贝
     * @remarks
     *   编辑器只调 cmake，由 cmake 去驱动平台本地工具链（MSBuild / Ninja / Make /
     *   xcodebuild），所以这里没有任何平台专属的构建器命令。工具链的选择读自编辑器
     *   构建时导出的 Tiny3DSDK.cmake，保证业务 DLL 与编辑器 ABI 一致。
     *
     *   编辑器编译与 IDE 编译共用 Temp/CppBuild 这一棵构建树。曾经是两棵——编辑器
     *   一棵、VS 解决方案另一棵——但两边 target 同名、产物又都落在
     *   Library/CppAssemblies 下，各自的 obj / tlog / 反射缓存互不可见，谁编过都会把
     *   对方的产物判成过期，于是每次换个入口就全量重编一遍。
     *
     *   编译产物落在 Library/CppAssemblies 下，但编辑器从不直接加载它——
     *   Windows 上 LoadLibrary 会锁住文件，锁住了就没法重新编译。加载前先拷一份到
     *   Temp/ShadowAssemblies 再加载那一份，原始产物始终可写。
     */
    class CppBuildSystem
        : public Allocator
        , public Singleton<CppBuildSystem>
    {
    public:
        /**
         * @brief 业务插件的两个构建变体
         * @remarks Editor 链 T3DCoreEditor 供编辑器加载，Runtime 链 T3DCore 供发布
         *          出去的游戏加载。两者 ABI 不兼容，产物目录严格分开，不做回退查找
         */
        enum class Variant
        {
            kEditor,
            kRuntime
        };

        /**
         * @brief 后台构建的生命周期
         * @remarks kSucceeded / kFailed 是等着被取走的终态，pollBuildState() 取过一次
         *          就回到 kIdle
         */
        enum class BuildState
        {
            kIdle,
            kRunning,
            kSucceeded,
            kFailed
        };

        CppBuildSystem() = default;

        ~CppBuildSystem() override;

        /**
         * @brief 绑定到已打开的工程
         * @param [in] projectPath : 工程根目录
         * @param [in] pluginName : 业务插件名，决定产物文件名
         * @param [in] cppSourceRelativePath : 业务 C++ 源码目录，相对工程根
         */
        void attachProject(const String &projectPath, const String &pluginName,
            const String &cppSourceRelativePath);

        /// 解绑工程，清掉所有路径状态
        void detachProject();

        /// 是否已绑定工程且业务 C++ 源码目录确实存在
        bool hasCppSources() const;

        /**
         * @brief 构建目录的独占锁
         * @remarks 编辑器有四条会编译的路径（打开工程 / Play / 编译菜单 / 校验 Runtime），
         *          共用一棵构建树之后它们绝不能并发跑 MSBuild，否则 tlog 与中间产物会互相
         *          写坏。锁只约束编辑器自己：VS 里的 MSBuild 不认这个锁，那种冲突靠
         *          isBusyOutputError() 事后识别
         */
        class BuildLock
        {
        public:
            /// @param [in] path : 锁文件路径，父目录不存在会被创建
            explicit BuildLock(const String &path);

            ~BuildLock();

            BuildLock(const BuildLock &) = delete;
            BuildLock &operator =(const BuildLock &) = delete;

            /// 是否真的拿到了锁
            bool isLocked() const { return mHandle != -1; }

        private:
            /// Windows 存 HANDLE，类 Unix 存 fd；没拿到锁时是 -1
            intptr_t mHandle {-1};
        };

        /**
         * @brief cmake 的输出是否在抱怨产物被别的进程占着
         * @remarks 命中时几乎都是 VS 正在编同一棵树，或者产物被调试器 / 杀毒软件按住。
         *          这类失败给一句人能看懂的话，比甩一屏 MSBuild 日志有用
         */
        static bool isBusyOutputError(const String &output);

        /**
         * @brief 判断产物是否已过期，需要重新编译
         * @param [in] variant : 构建变体
         * @return 产物不存在，或任一源码 / CMake 文件比产物新时返回 true
         * @remarks 比较的是原始产物而不是影子副本，影子副本的时间戳没有意义
         */
        bool needsBuild(Variant variant) const;

        /**
         * @brief 编译业务 C++ 插件
         * @param [in] variant : 构建变体
         * @param [out] output : cmake 的完整输出，供失败时展示给用户
         * @return 成功返回 T3D_OK
         * @remarks 只编指定变体那一个 target，同一棵构建树里的其它变体不受影响。
         *          首次、CMakeCache 缺失、或源码 / 反射产物的文件集合变化时会先
         *          configure 再 build：*.generated.cpp 只能靠 configure 期的
         *          file(GLOB) 进入 target。
         */
        TResult build(Variant variant, String &output);

        /**
         * @brief 在后台线程编译业务 C++ 插件
         * @param [in] variant : 构建变体
         * @return 成功投递返回 T3D_OK；已有后台构建在跑返回 T3D_ERR_FAIL
         * @remarks cmake 一趟能跑几十秒，同步等它就是把整个编辑器主循环按住。这里只负责
         *          起线程，调用方每帧 pollBuildState()，拿到终态再去做热重载
         */
        TResult buildAsync(Variant variant);

        /// 后台构建是否还在跑
        bool isBuildInFlight() const;

        /**
         * @brief 取后台构建的状态，终态只会返回一次
         * @remarks 只能由主线程调用。返回终态时线程已经 join，getLastOutput() 可以读了
         */
        BuildState pollBuildState();

        /// 后台构建当前在做什么，给进度提示显示；没有后台构建时返回空串
        String getBuildStage() const;

        /// 后台构建的变体，只在 isBuildInFlight() 为真时有意义
        Variant getBuildingVariant() const { return mBuildingVariant; }

        /**
         * @brief 把 Editor 变体的产物拷到影子目录
         * @param [out] shadowDir : 影子目录的绝对路径，供 loadPluginFromPath 使用
         * @param [out] shadowName : 影子副本的逻辑名（不含平台前后缀）
         * @return 成功返回 T3D_OK；产物不存在返回 T3D_ERR_FILE_NOT_EXIST
         * @remarks 影子名带产物的最后修改时间，同一份产物重复加载会复用已有副本
         */
        TResult shadowCopy(String &shadowDir, String &shadowName);

        /**
         * @brief 删除影子目录下的残留副本
         * @param [in] keepName : 要保留的影子逻辑名，为空表示全部删除
         * @remarks 删除失败只记日志——文件可能还被调试器或杀毒软件占着，留给下次清理。
         *          正常情况下影子目录稳态只有当前加载的那一份
         */
        void cleanShadowAssemblies(const String &keepName);

        /// 产物的逻辑名（不含平台前后缀），Editor 变体会带 Editor 后缀
        String getAssemblyName(Variant variant) const;

        /// 产物所在目录
        String getAssemblyDir(Variant variant) const;

        /// 产物的完整路径，含平台前后缀
        String getAssemblyPath(Variant variant) const;

        /// 影子目录
        const String &getShadowDir() const { return mShadowDir; }

        /// 上一次 build 的完整输出（后台线程也会写它，所以按值返回）
        String getLastOutput() const;

        /// 唯一的 CMake 构建目录（{Project}/Temp/CppBuild），编辑器与 IDE 共用
        String getBuildDir() const;

        /// 反射产物目录（{Project}/Temp/CppBuild/Generated），与 CMake 侧约定一致
        String getGeneratedDir() const;

        /**
         * @brief 确保游戏工程顶层 C++ solution 已生成
         * @param [out] slnPath : 找到的 .sln 绝对路径
         * @param [out] output : cmake configure 的完整输出
         * @return 成功返回 T3D_OK；没有源码 / 没有 sln / configure 失败分别返回对应错误
         * @remarks 与 build() 走同一条 configure 路径、同一个构建目录，因此在 VS 里编过
         *          之后编辑器不会再重编一遍，反之亦然
         */
        TResult ensureIDESolution(String &slnPath, String &output);

        /**
         * @brief 执行外部命令并捕获它的输出
         * @param [in] cmdLine : 完整命令行
         * @param [in] workDir : 工作目录，为空则继承当前进程
         * @param [out] output : 合并后的 stdout + stderr
         * @return 命令退出码为 0 时返回 T3D_OK
         */
        static TResult runCommand(const String &cmdLine, const String &workDir,
            String &output);

    protected:
        /// 业务 C++ 源码目录（默认 Assets/Source，可由 CppSourceRelativePath 覆盖）
        String getCppSourceDir() const { return mCppSourceDir; }

        /**
         * @brief 需要时执行 cmake configure，顺带把引擎托管的 CMake 模板同步过去
         * @param [out] output : cmake 的完整输出
         * @param [in] verifyGeneratedProjects : 是否顺带检查生成的 vcxproj 有没有被写坏
         * @remarks build() 与 ensureIDESolution() 都走这里，两个入口的 configure 参数
         *          必须完全一致，否则同一个构建目录会被反复重新生成
         */
        TResult ensureConfigured(String &output,
            bool verifyGeneratedProjects = false);

        /**
         * @brief 无条件执行一次 cmake configure，成功后刷新文件集合 stamp
         * @remarks 工具链参数（-G / -A / -T / CMAKE_CXX_COMPILER）取自 SDK 导出的
         *          Tiny3DSDK.cmake，编辑器侧不硬编码任何生成器名字
         */
        TResult doConfigure(String &output);

        /// 执行一次 cmake --build，只编指定变体那一个 target
        TResult runBuild(Variant variant, String &output);

        /// CMakeCache 缺失、工程文件损坏、或文件集合相对上次 configure 有变化
        bool needsReconfigure() const;

        /// 参与 configure 判定的文件集合：源码文件名 + 反射产物文件名
        String collectStamp() const;

        /// 上次 configure 时记下的文件集合
        String getCppFileStampPath() const;

        /// 构建目录的锁文件
        String getBuildLockPath() const;

        /// 记录后台构建当前阶段，供 getBuildStage() 读
        void setBuildStage(const String &stage);

        /// 记录本次 build 的完整输出
        void setLastOutput(const String &output);

        /// 后台构建线程跑完就 join，没有则立即返回
        void joinBuildThread();

        /// 把当前文件集合写到构建目录，供下次比较
        void writeCppFileStamp(const String &list) const;

        /// 清掉旧布局（编辑器与 IDE 各一棵树）留在磁盘上的构建目录
        void removeLegacyBuildDirs() const;

        /// 从 SDK 的 Tiny3DSDK.cmake 里读出工具链配置，只读一次并缓存
        bool loadSDKConfig();

        /// 从已读入的 Tiny3DSDK.cmake 文本里取出某个 set(NAME "value") 的值
        String parseSDKValue(const String &name) const;

        /// 拼出当前平台的动态库文件名，规则与 Dylib::onLoad 保持一致
        static String platformLibFileName(const String &name);

        /// 拼出当前平台的调试符号文件名，没有伴随符号文件时返回空串
        static String platformSymbolFileName(const String &name);

        /// 拼出 cmake configure 命令（工具链参数来自 Tiny3DSDK.cmake）
        String buildCMakeConfigureCommand(const String &sourceDir,
            const String &buildDir) const;

        /// 从编辑器模板同步引擎托管的 CMake（GamePluginCommon / Player）
        /// @return 有文件被写入时返回 true，供调用方决定是否重新 configure
        bool syncCMakeFromTemplate() const;

        /// 把模板相对路径同步到业务 C++ 源码目录，并替换 {ProjectName}
        bool syncTemplateFile(const String &relativePath) const;

        /// cmake 生成 sln 后，把 vcxproj 里 LocalDebugger* 路径改成本机分隔符
        void fixGeneratedVsDebuggerPaths(const String &buildDir) const;

        /// 递归收集目录下指定后缀的文件
        static void collectFilesBySuffix(const String &dir, const String &suffix,
            TArray<String> &out);

        /// 把一段文本里的路径分隔符改成本机形式（Windows \，其它 /）
        static void toNativePathSeparators(String &text);

        /// 只改 LocalDebugger* 标签内的路径分隔符，整文件有改动返回 true
        static bool rewriteVsDebuggerPathSlashes(String &xml);

        /// xml[pos] 是否为标签名的合法结束字符，用于避免前缀标签误匹配
        static bool isTagNameEnd(const String &xml, size_t pos);

        /// 生成的工程文件是否被旧版分隔符改写逻辑破坏（出现 "<\" 这种非法 XML）
        static bool hasCorruptedGeneratedProject(const String &buildDir);

        static bool readTextFile(const String &path, String &text);
        static bool writeTextFile(const String &path, const String &text);

        /// 目录存在则整棵删掉，失败只记日志
        static void removeDirIfExists(const String &dir);

        /**
         * @brief 收集目录下匹配任一后缀的文件名（不含路径），已排序、每行一个
         * @remarks 比对的是后缀而不是扩展名，".generated.cpp" 这种复合后缀才能区分出来
         */
        static String collectFileNames(const String &dir,
            const char *const *suffixes);

        /// 在构建目录根下找第一个 .sln
        static bool findSolutionFile(const String &buildDir, String &slnPath);

        /// 路径含空格时给 cmake 命令行参数加引号
        static String quote(const String &value);

        /// 取目录树下所有源码与 CMake 文件的最新修改时间
        static long_t getNewestSourceTime(const String &dir);

    protected:
        /// 编辑器自身的构建配置，业务插件必须用同一个
        static const char *BUILD_CONFIG;

        /// 工程根目录
        String mProjectPath {};
        /// 业务插件名
        String mPluginName {};
        /// 业务 C++ 源码目录
        String mCppSourceDir {};
        /// 影子目录
        String mShadowDir {};

        /// Tiny3DSDK.cmake 的原文，首次使用时读入
        String mSDKConfig {};
        /// 是否已尝试读取过 SDK 配置
        bool mSDKConfigLoaded {false};

        /// 上一次 build 的完整输出
        String mLastOutput {};

        /// 后台构建线程，跑完由 pollBuildState() 回收
        std::thread mBuildThread {};
        /// 后台构建状态
        std::atomic<BuildState> mBuildState {BuildState::kIdle};
        /// 后台构建当前阶段的可读描述
        String mBuildStage {};
        /// 保护 mLastOutput 与 mBuildStage：这两个由后台线程写、主线程读
        mutable std::mutex mBuildMutex {};
        /// 后台构建的变体，起线程之前写好，之后只读
        Variant mBuildingVariant {Variant::kEditor};
    };

    #define CPP_BUILD_SYS (CppBuildSystem::getInstance())

    NS_END
}
