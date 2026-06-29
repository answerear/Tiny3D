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


#ifndef __T3D_BUNDLE_MANIFEST_H__
#define __T3D_BUNDLE_MANIFEST_H__


#include "T3DBundleFSArchivePrerequisites.h"


namespace Tiny3D
{
    /**
     * @brief Bundle 清单，维护 [相对路径 -> UUID] 的内存映射。
     * @remarks
     *  - 清单为纯文本格式，由原生 I/O 读出后调用 parse() 解析，不依赖其它插件。
     *  - 文本格式：第一行为魔数版本头，其后每行一个条目，格式为
     *    "<UUID 32位十六进制> <类型整数> <相对路径>"，相对路径位于行尾，
     *    可包含空格。
     *  - 运行时仅用于把按名字读取的请求转换为 UUID，再由 BundleFSArchive
     *    直接按 UUID 读取散列文件。
     */
    class BundleManifest
    {
    public:
        /// 清单文件魔数版本头
        static const char * const MAGIC;

        /**
         * @brief 从数据流解析清单内容。
         * @param [in] stream : 清单文本数据流
         * @return 解析成功返回 true
         */
        bool parse(DataStream &stream);

        /**
         * @brief 根据相对路径查找对应的 UUID。
         * @param [in] path : 相对路径（分隔符不敏感）
         * @param [out] uuid : 命中时返回对应 UUID
         * @return 命中返回 true，未命中返回 false
         */
        bool getUUIDByPath(const String &path, UUID &uuid) const;

        /**
         * @brief 判断相对路径是否存在于清单中。
         */
        bool exists(const String &path) const;

        /**
         * @brief 解析 UUID 重定向（别名）。
         * @remarks
         *  用于 ShaderLab 这类「逻辑 UUID」到编译产物 UUID 的映射：材质持久化的是
         *  逻辑 shader（ShaderLab）的 UUID，而 bundle 内真正存在的散列文件以编译后
         *  的 .tshader（ShaderUUID）命名。运行时据此把逻辑 UUID 重定向到物理 UUID，
         *  对齐 MetaFileSystem 中 lab -> ShaderUUID 的重定向语义。
         * @param [in] uuid : 逻辑 UUID
         * @param [out] target : 命中时返回重定向后的目标 UUID
         * @return 命中返回 true，未命中返回 false
         */
        bool getRedirect(const UUID &uuid, UUID &target) const;

        /**
         * @brief 清空清单。
         */
        void clear();

        /**
         * @brief 清单条目数量。
         */
        size_t size() const { return mPathToUUID.size(); }

    protected:
        /// 把路径分隔符统一为 '/' 并去掉开头的 "./"
        static String normalize(const String &path);

        /// 取路径最后一段（文件名）
        static String basename(const String &path);

        /// 添加一个条目
        void addEntry(const String &path, const UUID &uuid);

        /// 添加一个 UUID 重定向（别名）：逻辑 UUID -> 目标 UUID
        void addAlias(const UUID &from, const UUID &to);

    protected:
        /// 归一化相对路径 -> UUID
        TUnorderedMap<String, UUID> mPathToUUID {};
        /// 文件名 -> UUID（仅在唯一时有效，用于按文件名的回退查找）
        TUnorderedMap<String, UUID> mNameToUUID {};
        /// 记录发生过冲突的文件名，冲突的文件名不参与回退查找
        TUnorderedMap<String, bool> mAmbiguousNames {};
        /// 逻辑 UUID(字符串) -> 目标 UUID 的重定向表（如 ShaderLab -> 编译后 .tshader）
        TUnorderedMap<String, UUID> mAliasByUUID {};
    };
}


#endif  /*__T3D_BUNDLE_MANIFEST_H__*/
