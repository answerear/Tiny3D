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


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ArchiveManagerPtr ArchiveManager::create()
    {
        return T3D_NEW ArchiveManager();
    }
    
    //--------------------------------------------------------------------------

    ArchiveManager::~ArchiveManager()
    {
        unloadAllArchives();
    }

    //--------------------------------------------------------------------------
//
//     TResult ArchiveManager::readArchive(const String &archiveName, const String &archiveType, const String &fileName, const ArchiveReadCallback &callback)
//     {
//         TResult ret = T3D_OK;
//
//         do
//         {
// #if defined (T3D_OS_DESKTOP)
//             // Windows & Mac OS X & Linux
//             Archive *archive;
//             if (archiveType == "FileSystem")
//             {
//                 //  桌面系统的文件系统，直接特殊处理一番
//                 String path = archiveName + Dir::getNativeSeparator() + fileName;
//                 archive = T3D_ARCHIVE_MGR.loadArchive("Computer", archiveType, Archive::AccessMode::kRead);
//                 if (archive == nullptr)
//                 {
//                     ret = T3D_ERR_INVALID_POINTER;
//                     T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid archive when reading archive [%s] !", archiveName.c_str());
//                     break;
//                 }
//                 ret = archive->read(path, callback);
//             }
//             else
//             {
//                 // 其他的，按照常规处理
//                 archive = T3D_ARCHIVE_MGR.loadArchive(archiveName, archiveType, Archive::AccessMode::kRead);
//                 if (archive == nullptr)
//                 {
//                     ret = T3D_ERR_INVALID_POINTER;
//                     T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid archive when reading archive [%s] !", archiveName.c_str());
//                 }
//                 ret = archive->read(fileName, callback);
//             }
// #elif defined (T3D_OS_MOBILE)
//             // iOS & Android
//             Archive *archive = T3D_ARCHIVE_MGR.loadArchive(archiveName, archiveType, Archive::AccessMode::kRead);
//             if (archive == nullptr)
//             {
//                 ret = T3D_ERR_INVALID_POINTER;
//                 T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid archive when reading archive [%s] !", archiveName.c_str());
//             }
//             ret = archive->read(fileName, callback);
// #else
//             #error "Invalid platform" 
// #endif
//         } while (false);
//
//         return ret;
//     }
//     
//     //--------------------------------------------------------------------------
//
//     TResult ArchiveManager::writeArchive(const String &archiveName, const String &archiveType, const String &fileName, const ArchiveWriteCallback &callback, bool truncate)
//     {
//         TResult ret = T3D_OK;
//
//         do
//         {
//             Archive::AccessMode mode = truncate ? Archive::AccessMode::kTruncate : Archive::AccessMode::kAppend;
//             
// #if defined (T3D_OS_DESKTOP)
//             // Windows & Mac OS X & Linux
//             Archive *archive;
//             
//             if (archiveType == "FileSystem")
//             {
//                 // 桌面系统的文件系统，直接特殊处理一番
//                 String path = archiveName + Dir::getNativeSeparator() + fileName;
//                 archive = T3D_ARCHIVE_MGR.loadArchive("Computer", archiveType, mode);
//                 if (archive == nullptr)
//                 {
//                     ret = T3D_ERR_INVALID_POINTER;
//                     T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid archive when reading archive [%s] !", archiveName.c_str());
//                     break;
//                 }
//                 ret = archive->write(path, callback);
//             }
//             else
//             {
//                 // 其他的，按照常规处理
//                 archive = T3D_ARCHIVE_MGR.loadArchive(archiveName, archiveType, mode);
//                 if (archive == nullptr)
//                 {
//                     ret = T3D_ERR_INVALID_POINTER;
//                     T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid archive when reading archive [%s] !", archiveName.c_str());
//                 }
//                 ret = archive->write(fileName, callback);
//             }
// #elif defined (T3D_OS_MOBILE)
//             // iOS & Android
//             Archive *archive = T3D_ARCHIVE_MGR.loadArchive(archiveName, archiveType, mode);
//             if (archive == nullptr)
//             {
//                 ret = T3D_ERR_INVALID_POINTER;
//                 T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid archive when reading archive [%s] !", archiveName.c_str());
//             }
//             
//             ret = archive->write(fileName, callback);
// #else
//             #error "Invalid platform"
// #endif
//         } while (false);
//         
//         return ret;
//     }

    //--------------------------------------------------------------------------

    ArchivePtr ArchiveManager::loadArchive(const String &name, const String &archiveType, Archive::AccessMode accessMode)
    {
        ArchivePtr archive;

        Key key;
        makeKey(archiveType, name, accessMode, key);
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
        Key key;
        makeKey(archive->getArchiveType(), archive->getName(), archive->getAccessMode(), key);
        mArchives.erase(key);
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ArchiveManager::unloadArchive(const String &archiveType)
    {
        auto itr = mArchives.begin();
        while (itr != mArchives.end())
        {
            if (itr->second->getArchiveType() == archiveType)
            {
                itr = mArchives.erase(itr);
            }
            else
            {
                ++itr;
            }
        }

        return T3D_OK;
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
            ArchiveCreatorNew creator = itr->second;
            archive = creator(name, mode);
        }

        return archive;
    }

    //--------------------------------------------------------------------------

    TResult ArchiveManager::addArchiveCreator(const String &archiveType, const ArchiveCreatorNew &creator)
    {
        mCreators.emplace(archiveType, creator);
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

    Archive *ArchiveManager::getArchive(const String &achiveType, const String &name, Archive::AccessMode mode) const
    {
        Archive *archive = nullptr;
        Key key;
        makeKey(achiveType, name, mode, key);
        auto itr = mArchives.find(key);

        if (itr != mArchives.end())
        {
            archive = itr->second;
        }

        return archive;
    }

    //--------------------------------------------------------------------------

    void ArchiveManager::makeKey(const String &archiveType, const String &name, Archive::AccessMode mode, Key &key) const
    {
        key.name = archiveType + "#" + name;;
        key.access = mode;
    }

    //--------------------------------------------------------------------------
}

