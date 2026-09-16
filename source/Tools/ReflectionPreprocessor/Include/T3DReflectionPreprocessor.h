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

#ifndef __T3D_REFLECTION_PREPROCESSOR_H__
#define __T3D_REFLECTION_PREPROCESSOR_H__


#include "T3DRPPrerequisites.h"
#include "T3DPreprocessorOptions.h"

#include <vector>
#include <unordered_set>


namespace Tiny3D
{
    class ReflectionGenerator;
    
    class ReflectionPreprocessor : public Allocator
    {
    public:
        ReflectionPreprocessor();

        virtual ~ReflectionPreprocessor();

        TResult execute(int32_t argc, char *argv[]);

    protected:
        ClangArgs parseSettingsFile(const String &path);

        /// 追加 -resource-dir，指向随 rpp 一起分发的 clang 内建头目录
        void appendResourceDir(ClangArgs &args);

        /// macOS：若配置里没有 -isysroot，补上 SDK 路径，否则找不到 <typeinfo>
        void appendSysroot(ClangArgs &args);

        /**
         * @brief Windows：配置里没给系统头路径时，探测 MSVC 与 Windows SDK 补上
         * @remarks 这里的 libclang 只认 %INCLUDE% 去找 MSVC / Windows SDK 头，注册表
         *      与 COM 那套自动探测在它身上不生效。从 Visual Studio 或 vcvarsall 起的
         *      构建自带这套环境变量，Android Studio 的 Gradle 进程没有，于是
         *      <WinSock.h> 之类的头会找不到、整个工程的 AST 全军覆没。
         *      优先级：配置里的 SystemIncludePath > %INCLUDE% > 这里的探测。
         *      探测到的版本可能与编译工程的工具集不一致，所以只当兜底用；引擎自己的
         *      generate 流程会把路径钉死写进 ReflectionSettings.json。
         *      环境变量 T3D_RPP_VS_YEAR 可以指定用哪一代，例如 "2019"。
         */
        void appendMSVCIncludes(ClangArgs &args);

        /**
         * @brief 解析前确认系统头搜索路径可用
         * @remarks 缺了这一步，每个 TU 都会以 "'xxx.h' file not found" 失败，而且要
         *      等全部文件跑完才看得出来，日志里还是几百条一模一样的报错。
         */
        TResult verifySystemIncludes() const;

        /// 定位 MSVC 工具链的 include 目录，找不到返回空串
        static String findVCToolsInclude();

        /// 定位 Windows 10/11 SDK 带版本号的 Include 目录，找不到返回空串
        static String findWindowsSDKIncludeRoot();

        /// 用 mArgs 重建 ClangArgs，避免 vector 扩容后 c_str 悬空
        void syncClangArgs(ClangArgs &args);

        TResult collectProjectHeaders(const String &path);
        
        /// 待处理的源文件信息
        struct PendingFile
        {
            String filePath {};
            String fileTitle {};
            String generatedFile {};
            String depsFile {};
            bool processed {false};     /// AST 解析是否成功
        };

        TResult generateAST(const ClangArgs &args, std::vector<PendingFile> &pendingFiles, int32_t numThreads);

        /// 递归收集待处理的 .cpp/.cxx 文件
        void collectSourceFiles(const String &path, const String &generatedPath, bool rebuild,
                                std::vector<PendingFile> &pendingFiles);

        TResult generateSource(const String &path, bool rebuild, bool dumpAST);

        /// 检查 .deps 依赖文件，判断是否需要重新生成（混合时间戳+哈希策略）
        bool needsRebuild(const String &depsFile, const String &srcFile, const String &generatedFile);

        /**
         * @brief 检查增量缓存是否完整可用
         * @param [in] generatedPath : Generated 目录
         * @param [in] dumpAST : 是否开启了 AST dump
         * @param [out] reason : 缓存不可用的原因，用于日志
         * @return 缓存完整返回 true，此时可以安全走增量
         * @remarks 增量依赖 .deps 的时间戳和 .tpl 里的模板实例化记录。前者决定
         *      单个文件是否重新解析，后者把跳过文件的模板实例合并回本次结果。
         *      .tpl 是跨文件的全局状态，一旦缺失，被跳过文件贡献的模板实例就会
         *      凭空消失或者与本次解析出的实例重复注册，产物与全量生成不等价。
         *      这种损坏没法靠逐文件的 needsRebuild 发现，只能整体降级到全量。
         */
        bool checkIncrementalCache(const String &generatedPath, bool dumpAST,
                                   String &reason) const;

        /**
         * @brief 删掉 .deps/.tpl 里已经没有对应源文件的孤儿缓存
         * @param [in] generatedPath : Generated 目录
         * @param [in] sourcePath : 源码根目录
         * @remarks 源文件被删掉、或者头文件不再带反射宏之后，留下的孤儿缓存会让
         *      checkIncrementalCache 永远判定不一致，从此每次构建都退回全量。
         *      孤儿本身对增量没有价值，清掉即可，不必因此放弃整个缓存。
         */
        void pruneOrphanCacheFiles(const String &generatedPath,
                                   const String &sourcePath) const;

        /// 递归收集本轮应当处理的源文件 title，口径必须与 collectSourceFiles 一致
        void collectExpectedSourceTitles(const String &path,
                                         std::unordered_set<std::string> &titles) const;

        /**
         * @brief 给还没有产物的源文件补一份只有注释头的产物
         * @param [in] generatedPath : Generated 目录
         * @param [in] sourcePath : 源码根目录
         * @remarks 构建系统要在 configure 期就把产物列表定下来，而它当时唯一能
         *      枚举的就是源文件。没有反射内容的源文件不落盘，这份列表就会声明出
         *      磁盘上不存在的文件，构建直接失败。所以口径反过来对齐：源码树里每
         *      个 .cpp/.cxx 都有一份同名产物，没有内容的是个空翻译单元。
         *      不覆盖已有产物 —— 增量模式下本轮没解析的文件，产物就是上一轮的。
         */
        void writeEmptyGeneratedSources(const String &generatedPath,
                                        const String &sourcePath) const;

        /// 写入 .deps 依赖文件
        void writeDepsFile(const String &depsFile, const String &srcFile, const StringList &deps) const;

        /// 写入 .tpl 模板实例化持久化文件
        void writeTemplateFile(const String &tplFile, const String &srcTitle) const;

        /// 加载所有 .tpl 文件，对跳过文件的模板实例化信息注入 mGenerator
        void loadAllTemplateFiles(const String &depsDir, const std::vector<PendingFile> &pendingFiles);

        /// 计算文件内容的 FNV-1a 64 位哈希值
        static uint64_t computeFileHash(const String &filePath);

        /// 检查头文件内容是否包含反射宏关键字
        static bool hasReflectionMacros(const String &filePath);

        /// 检测并生成 PCH 文件，返回 PCH 文件路径（空字符串表示未生成）
        /// @param rebuild 为 true 时不复用已有 PCH（-r 全量生成）
        String detectAndGeneratePCH(const String &generatedPath, const ClangArgs &args, bool rebuild);

    protected:
        static const String kReflectionSettingsFile;
        
        typedef TList<String> SettingsPathes;

        SettingsPathes mPathes {};
        ReflectionGenerator *mGenerator {nullptr};
        StringList mArgs {};
        String mGeneratedPath {};
        /// 包含反射宏的头文件 title 集合（预扫描结果）
        std::unordered_set<std::string> mReflectionHeaders {};
        /// 在 collectProjectHeaders 阶段发现的 Prerequisites 头文件路径
        String mPrerequisitesHeader {};
    };
}


#endif  /*__T3D_REFLECTION_PREPROCESSOR_H__*/
