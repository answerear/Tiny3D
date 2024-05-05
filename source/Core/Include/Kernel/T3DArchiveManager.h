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
    using ArchiveCreatorNew = TFunction<ArchivePtr(const String&, Archive::AccessMode)>;

    /**
     * @brief 档案系统资源管理器
     */
    class T3D_ENGINE_API ArchiveManager final
        : public Singleton<ArchiveManager>
        , public Object
    {
    public:
        /**
         * @brief 创建 ArchiveManager 对象
         */
        static ArchiveManagerPtr create();
        
        /**
         * 析构函数
         */
        ~ArchiveManager() override;
        
        /**
         * @brief 加载档案对象
         * @param [in] name : 档案文件完整路径名.
         * @param [in] archiveType : 档案系统类型.
         * @param [in] accessMode : 档案系统访问模式
         * @return 返回加载的档案对象.
         */
        ArchivePtr loadArchive(const String &name, const String &archiveType, Archive::AccessMode accessMode);

        /**
         * @brief 卸载档案对象
         * @param [in] archive : 要卸载的档案对象.
         * @return 调用成功返回 T3D_OK.
         */
        TResult unloadArchive(ArchivePtr archive);

        /**
         * @brief 卸载指定档案类型的所有档案对象
         * @param archiveType : 档案类型
         * @return 调用成功返回 T3D_OK
         */
        TResult unloadArchive(const String &archiveType);

        /**
         * @brief 卸载所有档案系统
         * @return 调用成功返回 T3D_OK
         */
        TResult unloadAllArchives();

        /**
         * @brief 添加指定档案类型的档案对象创建器
         * @param [in] archiveType : 档案类型
         * @param [in] creator : 创建 lambda 函数.
         * @return 调用成功返回 T3D_OK.
         */
        TResult addArchiveCreator(const String &archiveType, const ArchiveCreatorNew &creator);

        /**
         * @brief 删除指定档案类型的档案对象创建器
         * @param [in] archiveType : 档案类型.
         * @return 调用成功返回 T3D_OK.
         */
        TResult removeArchiveCreator(const String &archiveType);

        /**
         * @brief 删掉所有档案对象创建器
         * @return 调用成功返回 T3D_OK.
         */
        TResult removeAllArchiveCreator();

        /**
         * @brief 根据名称和访问模式获取档案对象
         * @param [in] name : 档案名字.
         * @param [in] mode : 档案访问模式.
         * @return 存在对应的档案系统对象则返回对象地址，不存在则返回 nullptr.
         */
        Archive *getArchive(const String &name, Archive::AccessMode mode) const;

    protected:
        /**
         * 构造函数
         */
        ArchiveManager() = default;

        /**
         * 创建档案对象
         * @param name : 档案名称 
         * @param archiveType : 档案类型
         * @param mode : 档案访问模式
         * @return 调用成功返回档案对象
         */
        ArchivePtr create(const String &name, const String &archiveType, Archive::AccessMode mode);

    protected:
        /**
         * 键值，用于查找档案对象
         */
        struct Key
        {
            /// 档案名称
            String name {};
            /// 档案访问模式
            Archive::AccessMode access {Archive::AccessMode::kNone};

            /// 重载 operator < ，用于放入 TMap 等关联容器中
            bool operator <(const Key& other) const
            {
                return (name < other.name || access < other.access);
            }
        };

        using Creators = TMap<String, ArchiveCreatorNew>;

        using Archives = TMap<Key, ArchivePtr>;

        /// 档案对象创建器映射表
        Creators    mCreators {};
        /// 缓存的档案对象
        Archives    mArchives {};
    };

    #define T3D_ARCHIVE_MGR     ArchiveManager::getInstance()
}


#endif  /*__T3D_ARCHIVE_MANAGER_NEW_H__*/
