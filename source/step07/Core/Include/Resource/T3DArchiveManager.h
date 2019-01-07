/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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
     * @brief 档案系统资源管理器
     */
    class T3D_ENGINE_API ArchiveManager 
        : public Singleton<ArchiveManager>
        , public ResourceManager
    {
    public:
        /** 创建 ArchiveManager 对象 */
        static ArchiveManagerPtr create();

        /** 析构函数 */
        virtual ~ArchiveManager();

        /**
         * @brief 加载档案系统对象
         */
        virtual ArchivePtr loadArchive(const String &name, 
            const String &archiveType);

        /**
         * @brief 卸载档案系统对象
         */
        virtual void unloadArchive(ArchivePtr archive);

        /**
         * @brief 添加档案对象创建器
         */
        void addArchiveCreator(ArchiveCreator *creator);

        /**
         * @brief 移除档案对象创建器
         */
        void removeArchiveCreator(const String &name);

        /**
         * @brief 移除所有档案对象创建器
         */
        void removeAllArchiveCreator();

        /**
         * @brief 获取指定名称档案管理器对象
         */
        bool getArchive(const String &name, ArchivePtr &archive);

    protected:
        /** 构造函数 */
        ArchiveManager();

        /** 
         * @brief 重写 Resource::create() 接口 
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

        Creators    mCreators;
        Archives    mArchives;
    };

    #define T3D_ARCHIVE_MGR     ArchiveManager::getInstance()
}


#endif  /*__T3D_ARCHIVE_MANAGER_H__*/
