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


#include "Resource/T3DArchiveManager.h"
#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveCreator.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(ArchiveManager);

    //--------------------------------------------------------------------------

    ArchiveManagerPtr ArchiveManager::create()
    {
        ArchiveManagerPtr mgr = new ArchiveManager();
        mgr->release();
        return mgr;
    }

    //--------------------------------------------------------------------------

    ArchiveManager::ArchiveManager()
    {

    }

    ArchiveManager::~ArchiveManager()
    {

    }

    //--------------------------------------------------------------------------

    ArchivePtr ArchiveManager::loadArchive(const String &name,
        const String &archiveType)
    {
        ArchivePtr archive = smart_pointer_cast<Archive>
            (ResourceManager::load(name, 1, archiveType.c_str()));

        if (archive != nullptr)
        {
            mArchives.insert(ArchivesValue(name, archive));
        }

        return archive;
    }

    void ArchiveManager::unloadArchive(ArchivePtr archive)
    {
        unload((ResourcePtr &)archive);
    }

    //--------------------------------------------------------------------------

    ResourcePtr ArchiveManager::create(const String &name, int32_t argc, va_list args)
    {
        ArchivePtr res;
        if (argc == 1)
        {
            String archiveType = va_arg(args, char *);

            CreatorsConstItr itr = mCreators.find(archiveType);

            if (itr != mCreators.end())
            {
                ArchiveCreator *creator = itr->second;
                res = creator->createObject(1, name.c_str());
            }
        }

        return res;
    }

    void ArchiveManager::addArchiveCreator(ArchiveCreator *creator)
    {
        mCreators.insert(CreatorsValue(creator->getType(), creator));
    }

    void ArchiveManager::removeArchiveCreator(const String &name)
    {
        auto itr = mCreators.find(name);
        T3D_SAFE_DELETE(itr->second);
        mCreators.erase(itr);
    }

    void ArchiveManager::removeAllArchiveCreator()
    {
        auto itr = mCreators.begin();

        while (itr != mCreators.end())
        {
            T3D_SAFE_DELETE(itr->second);
            ++itr;
        }

        mCreators.clear();
    }

    bool ArchiveManager::getArchive(const String &name, ArchivePtr &archive)
    {
        bool found = false;
        auto itr = mArchives.begin();

        while (itr != mArchives.end())
        {
            if (itr->second->exists(name))
            {
                archive = itr->second;
                found = true;
                break;
            }
            ++itr;
        }

        return found;
    }
}

