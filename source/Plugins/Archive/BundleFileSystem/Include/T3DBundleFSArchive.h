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


#ifndef __T3D_BUNDLE_FS_ARCHIVE_H__
#define __T3D_BUNDLE_FS_ARCHIVE_H__


#include "T3DBundleFSArchivePrerequisites.h"
#include "T3DBundleManifest.h"


namespace Tiny3D
{
    /**
     * @brief Bundle 文件系统档案结构类。
     *
     * 这是一个只读的虚拟文件系统：发布期把工程内所有资源导出为以其 UUID 命名的
     * 散列文件，并附带一份 bundle.manifest 记录 [相对路径 -> UUID] 映射。运行时：
     *  - read(uuid)：直接用原生 I/O 打开 bundleRoot/<uuid> 文件，零解析；
     *  - read(name)：先经 manifest 把名字转成 UUID，再按 UUID 读取；
     *               若 manifest 未命中则直接返回错误，不回退原生按名读取。
     *
     * 该类只使用原生功能读取字节（桌面 FileDataStream / Android AAssetManager），
     * 不依赖其它 archive 插件。
     */
    class BundleFSArchive : public Archive
    {
    public:
        /// 档案类型
        static const char * const ARCHIVE_TYPE;

        /// 清单文件名（位于 bundle 根目录下）
        static const char * const MANIFEST_NAME;

        /**
         * 创建对象
         */
        static BundleFSArchivePtr create(const String &name, AccessMode mode);

        /**
         * 析构函数
         */
        ~BundleFSArchive() override;

        /**
         * 重写 Archive::getArchiveType() 接口
         */
        String getArchiveType() const override;

    protected:
        /**
         * 重写 Archive::clone() 接口
         */
        ArchivePtr clone() const override;

        /**
         * 重写 Archive::getPath() 接口
         */
        String getPath() const override;

        /**
         * 重写 Archive::exists() 接口
         */
        bool exists(const String &name) const override;

        /**
         * 重写 Archive::read() 接口（按名字，经 manifest 转 UUID）
         */
        TResult read(const String &name, const ArchiveReadCallback &callback, void *userData) override;

        /**
         * 重写 Archive::write() 接口（只读档案，不支持）
         */
        TResult write(const String &name, const ArchiveWriteCallback &callback, void *userData) override;

        /**
         * 重写 Archive::read() 接口（按 UUID，直读散列文件）
         */
        TResult read(const UUID &uuid, const ArchiveReadCallback &callback, void *userData) override;

        /**
         * 重写 Archive::write() 接口（只读档案，不支持）
         */
        TResult write(const UUID &uuid, const ArchiveWriteCallback &callback, void *userData) override;

        /**
         * 构造函数
         */
        BundleFSArchive(const String &name, AccessMode mode);

        /**
         * 初始化：用原生 I/O 读出并解析 bundle.manifest。
         */
        bool init();

        /**
         * 用原生 I/O 读取 bundle 根目录下指定相对名字的文件，并执行回调。
         * @note 该方法是 manifest 之外的内部引导路径（也用于读取 manifest 本身）。
         */
        TResult readRaw(const String &relativeName, const ArchiveReadCallback &callback, void *userData);

    protected:
        /// 清单（相对路径 -> UUID）
        BundleManifest mManifest {};
        /// 清单是否成功加载
        bool mManifestLoaded {false};
    };
}


#endif  /*__T3D_BUNDLE_FS_ARCHIVE_H__*/
