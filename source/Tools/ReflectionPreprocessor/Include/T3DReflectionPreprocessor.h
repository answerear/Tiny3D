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
        String detectAndGeneratePCH(const String &generatedPath, const ClangArgs &args);

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
