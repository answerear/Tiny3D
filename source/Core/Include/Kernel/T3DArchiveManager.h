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


#include "Kernel/T3DArchive.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    typedef TFunction<ArchivePtr(const String&, Archive::AccessMode)> ArchiveCreator;

    /**
     * @class   ArchiveManager
     * @brief   檔案系統資源管理器
     */
    class T3D_ENGINE_API ArchiveManager final
        : public Singleton<ArchiveManager>
        , public Object
    {
    public:
        /**
         * @fn  static ArchiveManagerPtr create();
         * @brief   創建 ArchiveManager 對象
         * @return  An ArchiveManagerPtr.
         */
        static ArchiveManagerPtr create();

        /**
         * @fn  virtual ~ArchiveManager();
         * @brief   析構函數
         */
        ~ArchiveManager() override;

        /**
         * @fn  virtual ArchivePtr loadArchive(const String &name, 
         *      const String &archiveType);
         * @brief   記載檔案系統對象
         * @param   name        The name.
         * @param   archiveType Type of the archive.
         * @return  The archive.
         */
        ArchivePtr loadArchive(const String &name, const String &archiveType, Archive::AccessMode accessMode);

        /**
         * @fn  virtual TResult unloadArchive(ArchivePtr archive);
         * @brief   卸載檔案系統對象
         * @param   archive The archive.
         * @return  A TResult.
         */
        TResult unloadArchive(ArchivePtr archive);

        TResult unloadAllArchives();

        /**
         * @fn  TResult addArchiveCreator(ArchiveCreator *creator);
         * @brief   添加檔案對象創建器
         * @param [in]  creator If non-null, the creator.
         * @return  A TResult.
         */
        TResult addArchiveCreator(const String &archiveType, ArchiveCreator creator);

        /**
         * @fn  TResult removeArchiveCreator(const String &name);
         * @brief   移除檔案對象創建器
         * @param   archiveType    The name.
         * @return  A TResult.
         */
        TResult removeArchiveCreator(const String &archiveType);

        /**
         * @fn  TResult removeAllArchiveCreator();
         * @brief   移除所有檔案對象創建器
         * @return  A TResult.
         */
        TResult removeAllArchiveCreator();

        /**
         * @fn  bool getArchive(const String &name, Archive::AccessMode mode, ArchivePtr &archive);
         * @brief   根據名稱獲取檔案對象
         * @param [in]  name            : 檔案根路径.
         * @param [in]  mode            : 档案访问模式.
         * @param [in]  archive archive : 返回的檔案對象.
         * @return  找到則返回 true.
         */
        bool getArchive(const String &name, Archive::AccessMode mode, ArchivePtr &archive);

    protected:
        /**
         * @fn  ArchiveManager();
         * @brief   構造函數
         */
        ArchiveManager() = default;

        ArchivePtr create(const String &name, const String &archiveType, Archive::AccessMode mode);

    protected:
        struct Key
        {
            String              name;
            Archive::AccessMode access;

            bool operator <(const Key& other) const
            {
                return (name < other.name || access < other.access);
            }
        };

        typedef TMap<String, ArchiveCreator>    Creators;
        typedef Creators::iterator              CreatorsItr;
        typedef Creators::const_iterator        CreatorsConstItr;
        typedef Creators::value_type            CreatorsValue;

        typedef TMap<Key, ArchivePtr>           Archives;
        typedef Archives::iterator              ArchivesItr;
        typedef Archives::const_iterator        ArchivesConstItr;
        typedef Archives::value_type            ArchivesValue;

        Creators    mCreators;      /**< 檔案創建器 */
        Archives    mArchives;      /**< 緩存的檔案對象 */
    };

    #define T3D_ARCHIVE_MGR     ArchiveManager::getInstance()
}


#endif  /*__T3D_ARCHIVE_MANAGER_H__*/
