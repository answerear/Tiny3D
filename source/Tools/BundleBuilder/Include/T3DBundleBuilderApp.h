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

    protected:
        /// 资源根目录列表
        TArray<String>  mAssetRoots {};
        /// 输出目录
        String          mOutDir {};
        /// 已导出资源计数
        size_t          mExportedCount {0};
        /// 写入清单的条目数
        size_t          mEntryCount {0};
    };
}
