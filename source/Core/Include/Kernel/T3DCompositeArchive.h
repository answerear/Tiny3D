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
     * \brief 聚合档案：将多个子档案按优先级组成搜索链，对上层呈单一 Archive 接口
     * \remarks
     *   - exists / read 按优先级升序（数值越小越先）依次委托子档案，命中即返回。
     *   - write 只写入优先级最高（列表首项）且可写的子档案；空链或首项只读时返回
     *     T3D_ERR_NOT_WRAITABLE_FILE 并断言。
     */
    class T3D_ENGINE_API CompositeArchive : public Archive
    {
    public:
        /// 档案类型字符串，值为 "Composite"
        static const char * const ARCHIVE_TYPE;

        /**
         * \brief 创建聚合档案
         * \param [in] name : 档案名称，默认 "Composite"
         * \param [in] mode : 访问模式，默认 kRead
         * \return 新聚合档案智能指针
         */
        static CompositeArchivePtr create(const String &name = "Composite",
            AccessMode mode = AccessMode::kRead);

        /**
         * \brief 析构时清空搜索链（unmountAll）
         */
        ~CompositeArchive() override;

        /**
         * \brief 按优先级插入子档案到搜索链
         * \param [in] archive : 子档案；为 nullptr 时打错误日志并忽略
         * \param [in] priority : 优先级，数值越小越先被搜索；同优先级时后挂载者排在后
         */
        void mount(Archive *archive, int32_t priority);

        /**
         * \brief 从搜索链移除指定子档案（按指针相等，只移除第一个匹配项）
         * \param [in] archive : 要卸载的子档案
         */
        void unmount(Archive *archive);

        /// 清空搜索链上全部子档案
        void unmountAll();

        /// 返回搜索链上子档案数量
        size_t getArchiveCount() const { return mEntries.size(); }

        /// 搜索链是否为空
        bool isEmpty() const { return mEntries.empty(); }

        /**
         * \brief 返回 ARCHIVE_TYPE（"Composite"）
         * \return 类型字符串
         */
        String getArchiveType() const override;

        /**
         * \brief 返回 getName() 作为路径
         * \return 档案名称
         */
        String getPath() const override;

        /**
         * \brief 克隆自身并按原优先级逐个克隆、挂载子档案
         * \return 新聚合档案
         */
        ArchivePtr clone() const override;

        /**
         * \brief 按优先级依次查询子档案，任一存在即返回 true
         * \param [in] name : 文件名称
         * \return 命中返回 true，否则返回 false
         */
        bool exists(const String &name) const override;

        /**
         * \brief 按优先级依次尝试读；首个返回 T3D_OK 的子档案即成功
         * \param [in] name : 文件名称
         * \param [in] callback : 读回调
         * \param [in] userData : 透传用户数据
         * \return 成功返回 T3D_OK；全部失败时返回最后一个子档案的错误码（默认 T3D_ERR_FILE_NOT_EXIST）
         */
        TResult read(const String &name, const ArchiveReadCallback &callback,
            void *userData) override;

        /**
         * \brief 写入优先级最高（首项）且可写的子档案
         * \param [in] name : 文件名称
         * \param [in] callback : 写回调
         * \param [in] userData : 透传用户数据
         * \return 成功返回 T3D_OK；空链或首项只读返回 T3D_ERR_NOT_WRAITABLE_FILE
         */
        TResult write(const String &name, const ArchiveWriteCallback &callback,
            void *userData) override;

        /**
         * \brief 按 UUID、优先级依次尝试读
         * \param [in] uuid : 资源 UUID
         * \param [in] callback : 读回调
         * \param [in] userData : 透传用户数据
         * \return 成功返回 T3D_OK；全部失败时返回最后一个错误码（默认 T3D_ERR_FILE_NOT_EXIST）
         */
        TResult read(const UUID &uuid, const ArchiveReadCallback &callback,
            void *userData) override;

        /**
         * \brief 按 UUID 写入优先级最高且可写的子档案
         * \param [in] uuid : 资源 UUID
         * \param [in] callback : 写回调
         * \param [in] userData : 透传用户数据
         * \return 成功返回 T3D_OK；空链或首项只读返回 T3D_ERR_NOT_WRAITABLE_FILE
         */
        TResult write(const UUID &uuid, const ArchiveWriteCallback &callback,
            void *userData) override;

    protected:
        /**
         * \brief 构造聚合档案
         * \param [in] name : 档案名称
         * \param [in] mode : 访问模式
         */
        CompositeArchive(const String &name, AccessMode mode);

        /**
         * \brief 判断子档案是否带写标志（kAppend 或 kTruncate）
         * \param [in] archive : 子档案；nullptr 返回 false
         * \return 可写返回 true
         */
        static bool isWritable(const Archive *archive);

        /**
         * \brief 搜索链条目：子档案及其优先级
         */
        struct Entry
        {
            ArchivePtr  archive {nullptr};  ///< 子档案
            int32_t     priority {0};       ///< 优先级，数值越小越先搜索
        };

        TArray<Entry>   mEntries {};        ///< 按优先级升序排列的子档案列表
    };
}


#endif  /*__T3D_COMPOSITE_ARCHIVE_H__*/
