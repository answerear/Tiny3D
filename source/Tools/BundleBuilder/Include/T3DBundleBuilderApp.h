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

#pragma once


#include "T3DBundleBuilderPrerequisites.h"


namespace Tiny3D
{
    /**
     * @brief Bundle 构建工具（仅桌面）。
     *
     * 扫描工程资源目录中的 .meta 文件，得到每个资源的 UUID/类型，将资源字节
     * 拷贝为以 UUID 命名的散列文件，并生成 bundle.manifest（相对路径 -> UUID）。
     * 不打包 Tiny3D.cfg。运行时由 BundleFSArchive 读取该产物。
     *
     * 用法：
     *   bundlebuilder --assets <资源目录> [--assets <附加目录> ...] --out <输出目录>
     *   （可多次传入 --assets，例如把编译后的 Temp/shaders 一并纳入）
     */
    class BundleBuilderApp : public ConsoleApplication
    {
    public:
        BundleBuilderApp();

        ~BundleBuilderApp() override;

        TResult applicationDidFinishLaunching(int32_t argc, char *argv[]) override;

        void applicationDidEnterBackground() override;

        void applicationWillEnterForeground() override;

        void applicationWillTerminate() override;

        void applicationLowMemory() override;

    protected:
        /// 解析命令行参数
        bool parseArgs(int32_t argc, char *argv[]);

        /// 打印用法
        void printUsage() const;

        /// 递归创建多级目录
        static bool createDirectories(const String &path);

        /// 递归扫描目录并导出资源
        TResult scanDir(const String &root, const String &dir, DataStream &manifest);

        /// 处理单个文件：读取其 meta，导出散列文件并写清单
        TResult processFile(const String &root, const String &filePath, DataStream &manifest);

        /// 把绝对路径转换为相对 root 的、以 '/' 为分隔符的相对路径
        static String makeRelative(const String &root, const String &filePath);

        /// 写一条清单记录
        void writeManifestEntry(DataStream &manifest, const UUID &uuid,
            int32_t type, const String &relativePath);

        /// 写一条 UUID 重定向（别名）记录："ALIAS <逻辑UUID> <目标UUID>"
        /// 用于把材质引用的 ShaderLab 逻辑 UUID 重定向到编译后 .tshader 的 UUID。
        void writeAliasEntry(DataStream &manifest, const UUID &from, const UUID &to);

        /// 解析 --keep-languages 的逗号分隔列表到 mKeepLanguages
        bool parseKeepLanguages(const String &csv);

        /// scc/命令行语言字符串 -> SHADER_LANGUAGE（未识别返回 kUnknown）
        static SHADER_LANGUAGE languageFromString(const String &s);

        /// SHADER_LANGUAGE -> 命令行语言字符串（用于日志）
        static const char *languageToString(SHADER_LANGUAGE lang);

        /// 反序列化 .tshader，按 mKeepLanguages 裁剪语言变体并重新序列化导出。
        /// 同时收集裁剪前出现过的语言集合，供覆盖自检使用。
        TResult exportShader(const UUID &uuid, const String &filePath,
            const String &relativePath, DataStream &manifest);

        /// 遍历 Shader 的全部 (technique, pass, stage, keyword) 变体集合，
        /// 移除不在 mKeepLanguages 中的语言；present 返回裁剪前出现过的语言并集。
        void pruneShaderLanguages(const ShaderPtr &shader,
            TSet<SHADER_LANGUAGE> &present, bool &hasEmptySet);

    protected:
        /// 资源根目录列表
        TArray<String>  mAssetRoots {};
        /// 输出目录
        String          mOutDir {};
        /// 需要保留的语言白名单（空 = 全部保留，不裁剪）
        TSet<SHADER_LANGUAGE>   mKeepLanguages {};
        /// 已导出资源计数
        size_t          mExportedCount {0};
        /// 经过语言裁剪导出的 shader 计数
        size_t          mPrunedShaderCount {0};
        /// 写入清单的条目数
        size_t          mEntryCount {0};
    };
}
