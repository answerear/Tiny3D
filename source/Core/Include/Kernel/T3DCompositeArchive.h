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


#ifndef __T3D_COMPOSITE_ARCHIVE_H__
#define __T3D_COMPOSITE_ARCHIVE_H__


#include "Kernel/T3DArchive.h"


namespace Tiny3D
{
    /**
     * @brief 聚合档案，把多个子档案按优先级组织成一条搜索链
     * @remarks
     *   - 自身实现 Archive 接口，对上层（ResourceManager / 各资源 onLoad）完全透明。
     *   - read/exists 按优先级从高到低依次委托子档案，命中即返回，实现跨档案（跨包）
     *     的资源查找与依赖加载。
     *   - write 默认写入优先级最高（第一个）的子档案，用于编辑器工程资源写回；
     *     若最高优先级子档案不可写（如运行时只读包），则断言不支持。
     */
    class T3D_ENGINE_API CompositeArchive : public Archive
    {
    public:
        /// 聚合档案类型字符串
        static const char * const ARCHIVE_TYPE;

        /**
         * @brief 创建聚合档案对象
         * @param [in] name : 档案名称
         * @param [in] mode : 访问模式
         * @return 返回聚合档案对象
         */
        static CompositeArchivePtr create(const String &name = "Composite",
            AccessMode mode = AccessMode::kRead);

        /**
         * @brief 析构函数
         */
        ~CompositeArchive() override;

        /**
         * @brief 挂载一个子档案到搜索链
         * @param [in] archive : 要挂载的子档案对象
         * @param [in] priority : 优先级，数值越小越先被搜索
         */
        void mount(Archive *archive, int32_t priority);

        /**
         * @brief 从搜索链卸载指定子档案
         * @param [in] archive : 要卸载的子档案对象
         */
        void unmount(Archive *archive);

        /**
         * @brief 卸载搜索链上的所有子档案
         */
        void unmountAll();

        /**
         * @brief 获取搜索链上子档案数量
         */
        size_t getArchiveCount() const { return mEntries.size(); }

        /**
         * @brief 是否为空搜索链
         */
        bool isEmpty() const { return mEntries.empty(); }

        String getArchiveType() const override;

        String getPath() const override;

        ArchivePtr clone() const override;

        bool exists(const String &name) const override;

        TResult read(const String &name, const ArchiveReadCallback &callback,
            void *userData) override;

        TResult write(const String &name, const ArchiveWriteCallback &callback,
            void *userData) override;

        TResult read(const UUID &uuid, const ArchiveReadCallback &callback,
            void *userData) override;

        TResult write(const UUID &uuid, const ArchiveWriteCallback &callback,
            void *userData) override;

    protected:
        /**
         * @brief 构造函数
         * @param [in] name : 档案名称
         * @param [in] mode : 访问模式
         */
        CompositeArchive(const String &name, AccessMode mode);

        /**
         * @brief 判断子档案是否可写
         */
        static bool isWritable(const Archive *archive);

        /**
         * @brief 搜索链条目，记录子档案及其优先级
         */
        struct Entry
        {
            /// 子档案对象
            ArchivePtr  archive {nullptr};
            /// 优先级，数值越小越先被搜索
            int32_t     priority {0};
        };

        /// 按优先级升序排列的子档案列表
        TArray<Entry>   mEntries {};
    };
}


#endif  /*__T3D_COMPOSITE_ARCHIVE_H__*/
