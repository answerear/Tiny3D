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


#include "Kernel/T3DArchiveManager.h"
#include "Kernel/T3DArchive.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ArchiveManagerPtr ArchiveManager::create()
    {
        ArchiveManagerPtr mgr = new ArchiveManager();
        // mgr->release();
        return mgr;
    }
    
    //--------------------------------------------------------------------------

    ArchiveManager::~ArchiveManager()
    {
        unloadAllArchives();
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

    TResult ArchiveManager::unloadAllArchives()
    {
        mArchives.clear();
        return T3D_OK;
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

