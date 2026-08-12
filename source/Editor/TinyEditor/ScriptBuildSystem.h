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


namespace Tiny3D
{
    NS_BEGIN(Editor)

    /**
     * @brief 业务代码（Game Plugin）的编译与影子拷贝
     * @remarks
     *   编辑器只调 cmake，由 cmake 去驱动平台本地工具链（MSBuild / Ninja / Make /
     *   xcodebuild），所以这里没有任何平台专属的构建器命令。工具链的选择读自编辑器
     *   构建时导出的 Tiny3DSDK.cmake，保证业务 DLL 与编辑器 ABI 一致。
     *
     *   编译产物落在 Library/ScriptAssemblies 下，但编辑器从不直接加载它——
     *   Windows 上 LoadLibrary 会锁住文件，锁住了就没法重新编译。加载前先拷一份到
     *   Temp/ShadowAssemblies 再加载那一份，原始产物始终可写。
     */
    class ScriptBuildSystem
        : public Allocator
        , public Singleton<ScriptBuildSystem>
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

        ScriptBuildSystem() = default;

        ~ScriptBuildSystem() override = default;

        /**
         * @brief 绑定到已打开的工程
         * @param [in] projectPath : 工程根目录
         * @param [in] pluginName : 业务插件名，决定产物文件名
         * @param [in] scriptsRelativePath : 业务源码目录，相对工程根
         */
        void attachProject(const String &projectPath, const String &pluginName,
            const String &scriptsRelativePath);

        /// 解绑工程，清掉所有路径状态
        void detachProject();

        /// 是否已绑定工程且业务源码目录确实存在
        bool hasScripts() const;

        /**
         * @brief 判断产物是否已过期，需要重新编译
         * @param [in] variant : 构建变体
         * @return 产物不存在，或任一源码 / CMake 文件比产物新时返回 true
         * @remarks 比较的是原始产物而不是影子副本，影子副本的时间戳没有意义
         */
        bool needsBuild(Variant variant) const;

        /**
         * @brief 编译业务插件
         * @param [in] variant : 构建变体
         * @param [out] output : cmake 的完整输出，供失败时展示给用户
         * @return 成功返回 T3D_OK
         * @remarks 首次或 CMakeLists 变更时会先 configure 再 build，否则直接 build
         */
        TResult build(Variant variant, String &output);

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

        /// 上一次 build 的完整输出
        const String &getLastOutput() const { return mLastOutput; }

    protected:
        /// 业务 C++ 源码目录（默认 Assets/Source，可由 ScriptsRelativePath 覆盖）
        String getScriptsDir() const { return mScriptsDir; }

        /// 某个变体的 CMake 源码目录（如 Assets/Source/Editor）
        String getCMakeSourceDir(Variant variant) const;

        /// 某个变体的 CMake 构建目录
        String getCMakeBuildDir(Variant variant) const;

        /**
         * @brief 执行 cmake configure
         * @remarks 工具链参数（-G / -A / -T / CMAKE_CXX_COMPILER）取自 SDK 导出的
         *          Tiny3DSDK.cmake，编辑器侧不硬编码任何生成器名字
         */
        TResult configure(Variant variant, String &output);

        /// 从 SDK 的 Tiny3DSDK.cmake 里读出工具链配置，只读一次并缓存
        bool loadSDKConfig();

        /// 从已读入的 Tiny3DSDK.cmake 文本里取出某个 set(NAME "value") 的值
        String parseSDKValue(const String &name) const;

        /// 拼出当前平台的动态库文件名，规则与 Dylib::onLoad 保持一致
        static String platformLibFileName(const String &name);

        /// 拼出当前平台的调试符号文件名，没有伴随符号文件时返回空串
        static String platformSymbolFileName(const String &name);

        /**
         * @brief 执行外部命令并捕获它的输出
         * @param [in] cmdLine : 完整命令行
         * @param [in] workDir : 工作目录，为空则继承当前进程
         * @param [out] output : 合并后的 stdout + stderr
         * @return 命令退出码为 0 时返回 T3D_OK
         */
        static TResult runCommand(const String &cmdLine, const String &workDir,
            String &output);

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
        /// 业务源码目录
        String mScriptsDir {};
        /// 影子目录
        String mShadowDir {};

        /// Tiny3DSDK.cmake 的原文，首次使用时读入
        String mSDKConfig {};
        /// 是否已尝试读取过 SDK 配置
        bool mSDKConfigLoaded {false};

        /// 上一次 build 的完整输出
        String mLastOutput {};
    };

    #define SCRIPT_BUILD_SYS (ScriptBuildSystem::getInstance())

    NS_END
}
