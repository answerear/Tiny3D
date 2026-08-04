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


#ifndef __T3D_ARCHIVE_MANAGER_NEW_H__
#define __T3D_ARCHIVE_MANAGER_NEW_H__


#include "Kernel/T3DArchive.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /// 按名称与访问模式创建指定类型档案的工厂回调
    using ArchiveCreatorNew = TFunction<ArchivePtr(const String&, Archive::AccessMode)>;

    /**
     * \brief 档案管理器单例：注册类型创建器，并按类型+名称+访问模式缓存已加载档案
     */
    class T3D_ENGINE_API ArchiveManager final
        : public Singleton<ArchiveManager>
        , public Object
    {
    public:
        /**
         * \brief 创建 ArchiveManager 实例
         * \return 新管理器智能指针
         */
        static ArchiveManagerPtr create();

        /**
         * \brief 析构时卸载全部已缓存档案
         */
        ~ArchiveManager() override;

        /**
         * \brief 加载（或从缓存取回）指定类型与访问模式的档案
         * \param [in] name : 档案名称 / 路径
         * \param [in] archiveType : 档案类型字符串（须已注册创建器）
         * \param [in] accessMode : 访问模式
         * \return 已缓存或新创建的档案；无对应创建器时返回 nullptr
         */
        ArchivePtr loadArchive(const String &name, const String &archiveType, Archive::AccessMode accessMode);

        /**
         * \brief 从缓存移除指定档案（不销毁对象本身，仅 erase 缓存项）
         * \param [in] archive : 要卸载的档案
         * \return 恒返回 T3D_OK
         */
        TResult unloadArchive(ArchivePtr archive);

        /**
         * \brief 卸载缓存中所有指定类型的档案
         * \param [in] archiveType : 档案类型
         * \return 恒返回 T3D_OK
         */
        TResult unloadArchive(const String &archiveType);

        /**
         * \brief 清空全部档案缓存
         * \return 恒返回 T3D_OK
         */
        TResult unloadAllArchives();

        /**
         * \brief 注册档案类型创建器
         * \param [in] archiveType : 类型名
         * \param [in] creator : 创建回调
         * \return 恒返回 T3D_OK
         */
        TResult addArchiveCreator(const String &archiveType, const ArchiveCreatorNew &creator);

        /**
         * \brief 移除指定类型的创建器
         * \param [in] archiveType : 类型名
         * \return 恒返回 T3D_OK
         * \note 实现直接 erase(find 结果)，类型不存在时行为依赖容器 erase(end)
         */
        TResult removeArchiveCreator(const String &archiveType);

        /**
         * \brief 清空全部创建器
         * \return 恒返回 T3D_OK
         */
        TResult removeAllArchiveCreator();

        /**
         * \brief 按类型、名称、访问模式查找已缓存档案
         * \param [in] arhicveType : 档案类型（参数名与实现一致，拼写为 arhicve）
         * \param [in] name : 档案名称
         * \param [in] mode : 访问模式
         * \return 命中返回裸指针；未缓存返回 nullptr
         */
        Archive *getArchive(const String &arhicveType, const String &name, Archive::AccessMode mode) const;

    protected:
        /**
         * \brief 缓存查找键：类型#名称 + 访问模式
         */
        struct Key
        {
            String name {};                                         ///< 由 makeKey 生成：archiveType + "#" + name
            Archive::AccessMode access {Archive::AccessMode::kNone}; ///< 访问模式

            /// 先比 name，再比 access，供 TMap 排序
            bool operator <(const Key& other) const
            {
                return name < other.name || (name == other.name && access < other.access);
            }
        };

        ArchiveManager() = default;

        /**
         * \brief 用已注册创建器构造档案（不入缓存）
         * \param [in] name : 档案名称
         * \param [in] archiveType : 类型名
         * \param [in] mode : 访问模式
         * \return 创建成功返回档案；无创建器返回 nullptr
         */
        ArchivePtr create(const String &name, const String &archiveType, Archive::AccessMode mode);

        /**
         * \brief 填充缓存 Key：name = archiveType + "#" + name，access = mode
         * \param [in] archiveType : 类型名
         * \param [in] name : 档案名称
         * \param [in] mode : 访问模式
         * \param [out] key : 输出键
         */
        void makeKey(const String &archiveType, const String &name, Archive::AccessMode mode, Key &key) const;

    protected:
        using Creators = TMap<String, ArchiveCreatorNew>;
        using Archives = TMap<Key, ArchivePtr>;

        Creators    mCreators {};   ///< 类型 → 创建器
        Archives    mArchives {};   ///< Key → 已缓存档案
    };

    #define T3D_ARCHIVE_MGR     ArchiveManager::getInstance()

    T3D_EXTERN_SINGLETON(ArchiveManager)
}


#endif  /*__T3D_ARCHIVE_MANAGER_NEW_H__*/
