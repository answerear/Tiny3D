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


#include "Kernel/T3DArchiveManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    T3D_INIT_SINGLETON(ArchiveManager);

    //--------------------------------------------------------------------------

    ArchiveManagerPtr ArchiveManager::create()
    {
        ArchiveManagerPtr mgr = new ArchiveManager();
        // mgr->release();
        return mgr;
    }
    
    //--------------------------------------------------------------------------

    ArchivePtr ArchiveManager::loadArchive(const String &name, const String &archiveType, Archive::AccessMode accessMode)
    {
        ArchivePtr archive;

        Key key = { name, accessMode };
        auto itr = mArchives.find(key);
        if (itr != mArchives.end())
        {
            // 已经缓存了，直接从缓存中获取
            archive = itr->second;
        }
        else
        {
            // 没有缓存，创建一个
            archive = create(name, archiveType, accessMode);

            // 加入缓存
            if (archive != nullptr)
            {
                mArchives.insert({ key, archive });
            }
        }

        return archive;
    }

    //--------------------------------------------------------------------------

    TResult ArchiveManager::unloadArchive(ArchivePtr archive)
    {
        TResult ret = T3D_OK;

        // 清理緩存
        mArchives.erase({archive->getName(), archive->getAccessMode()});
        return ret;
    }

    //--------------------------------------------------------------------------
    
    ArchivePtr ArchiveManager::create(const String &name, const String &archiveType, Archive::AccessMode mode)
    {
        ArchivePtr archive;
        auto itr = mCreators.find(archiveType);

        if (itr != mCreators.end())
        {
            ArchiveCreator creator = itr->second;
            archive = creator(name, mode);
        }

        return archive;
    }

    //--------------------------------------------------------------------------

    TResult ArchiveManager::addArchiveCreator(const String &archiveType, ArchiveCreator creator)
    {
        mCreators.insert(CreatorsValue(archiveType, creator));
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ArchiveManager::removeArchiveCreator(const String &archiveType)
    {
        auto itr = mCreators.find(archiveType);
        mCreators.erase(itr);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult ArchiveManager::removeAllArchiveCreator()
    {
        mCreators.clear();
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    bool ArchiveManager::getArchive(const String &name, Archive::AccessMode mode, ArchivePtr &archive)
    {
        bool found = false;
        auto itr = mArchives.find({name, mode});

        if (itr != mArchives.end())
        {
            archive = itr->second;
            found = true;
        }

        return found;
    }

    //--------------------------------------------------------------------------
}

