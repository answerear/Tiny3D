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


#ifndef __T3D_ARCHIVE_MANAGER_H__
#define __T3D_ARCHIVE_MANAGER_H__


#include "Resource/T3DResourceManager.h"


namespace Tiny3D
{
    class ArchiveCreator;

    /**
     * @brief 檔案系統資源管理器
     */
    class T3D_ENGINE_API ArchiveManager 
        : public Singleton<ArchiveManager>
        , public ResourceManager
    {
    public:
        /** 創建 ArchiveManager 對象 */
        static ArchiveManagerPtr create();

        /** 析構函數 */
        virtual ~ArchiveManager();

        /**
         * @brief 記載檔案系統對象
         */
        virtual ArchivePtr loadArchive(const String &name, 
            const String &archiveType);

        /**
         * @brief 卸載檔案系統對象
         */
        virtual TResult unloadArchive(ArchivePtr archive);

        /**
         * @brief 重寫 ResourceManager::unloadAllResources() 接口
         */
        virtual TResult unloadAllResources() override;

        /**
         * @brief 添加檔案對象創建器
         */
        TResult addArchiveCreator(ArchiveCreator *creator);

        /**
         * @brief 移除檔案對象創建器
         */
        TResult removeArchiveCreator(const String &name);

        /**
         * @brief 移除所有檔案對象創建器
         */
        TResult removeAllArchiveCreator();

        /**
         * @brief 根據名稱獲取檔案對象
         * @param [in] name : 檔案名稱
         * @param [in] path : 相對檔案的路徑
         * @param [in][out] archive : 返回的檔案對象
         * @return 找到則返回 true
         */
        bool getArchive(const String &name, const String &path, 
            ArchivePtr &archive);

        /**
         * @brief 根據名稱獲取檔案對象
         * @param [in] filename : 文件名
         * @param [in][out] archive : 返回的檔案對象
         * @return 找到則返回true
         */
        bool getArchive(const String &filename, ArchivePtr &archive);

    protected:
        /** 構造函數 */
        ArchiveManager();

        /** 
         * @brief 重寫 Resource::create() 接口 
         */
        virtual ResourcePtr create(const String &name, int32_t argc, 
            va_list args) override;

        typedef TMap<String, ArchiveCreator*>   Creators;
        typedef Creators::iterator              CreatorsItr;
        typedef Creators::const_iterator        CreatorsConstItr;
        typedef Creators::value_type            CreatorsValue;

        typedef TMap<String, ArchivePtr>        Archives;
        typedef Archives::iterator              ArchivesItr;
        typedef Archives::const_iterator        ArchivesConstItr;
        typedef Archives::value_type            ArchivesValue;

        Creators    mCreators;      /**< 檔案創建器 */
        Archives    mArchives;      /**< 緩存的檔案對象 */
        Archives    mArchivesCache; /**< 用於加速查找文件的緩存 */
    };

    #define T3D_ARCHIVE_MGR     ArchiveManager::getInstance()
}


#endif  /*__T3D_ARCHIVE_MANAGER_H__*/
