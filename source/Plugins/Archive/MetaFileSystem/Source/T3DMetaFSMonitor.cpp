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


#include "T3DMetaFSMonitor.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    String MetaFSMonitor::EMPTY;
    
    //--------------------------------------------------------------------------

    MetaFSMonitorPtr MetaFSMonitor::create(const String &rootPath)
    {
        return T3D_NEW MetaFSMonitor(rootPath);
    }
    
    //--------------------------------------------------------------------------

    MetaFSMonitor::MetaFSMonitor(const String &rootPath)
        : mFSMonitor(T3D_NEW FileSystemMonitor())
        , mRootPath(rootPath)
    {
        
    }

    //--------------------------------------------------------------------------

    MetaFSMonitor::~MetaFSMonitor()
    {
        if (mFSMonitor != nullptr)
        {
            mFSMonitor->stopMonitor();
        }
        T3D_SAFE_DELETE(mFSMonitor);
    }

    //--------------------------------------------------------------------------

    TResult MetaFSMonitor::init()
    {
        TResult ret = T3D_OK;

        generateMeta(mRootPath);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MetaFSMonitor::generateMetaFile(const String &name, void *res)
    {
        TResult ret = T3D_OK;

        do
        {
            String dir, title, ext;
            if (!Dir::parsePath(name, dir, title, ext))
            {
                ret = T3D_ERR_FAIL;
                MFS_LOG_ERROR("Parse directory [%s] failed !", name.c_str());
                break;
            }

            if (ext == "meta" || ext == "tmp")
            {
                break;
            }

            if (!dir.empty())
            {
                title = dir + Dir::getNativeSeparator() + title;
            }

            String fullPath = toFullPath(name);
            String metaPath = fullPath + ".meta";
            MetaPtr meta;

            if (Dir::exists(metaPath))
            {
                FileDataStream fs;
                if (fs.open(metaPath.c_str(), FileDataStream::EOpenMode::E_MODE_READ_ONLY))
                {
                    meta = T3D_SERIALIZER_MGR.deserialize<Meta>(fs);
                    fs.close();
                }

                if (meta == nullptr)
                {
                    ret = T3D_ERR_FAIL;
                    MFS_LOG_ERROR("Failed to read meta file %s !", metaPath.c_str());
                    break;
                }
            }
            else
            {
                meta = generateFileMeta(metaPath, getRootPath(), title, ext, res);
                if (meta == nullptr)
                {
                    ret = T3D_ERR_FAIL;
                    break;
                }
            }

            mPendingPaths.erase(toRelativePath(name));
            addPathLUT(name, meta);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MetaFSMonitor::refresh(const String &path)
    {
        String fullPath = toFullPath(path);
        if (fullPath == mRootPath || !Dir::exists(fullPath) || Dir::isDirectory(fullPath))
        {
            return T3D_OK;
        }

        return generateFileMeta(fullPath);
    }

    //--------------------------------------------------------------------------

    void MetaFSMonitor::generateMeta(const String &path)
    {
        String searchPath = path + Dir::getNativeSeparator() + "*.*";
        Dir dir;
        bool working = dir.findFile(searchPath);

        while (working)
        {
            if (dir.isDots())
            {
                // . or ..
            }
            else if (dir.isDirectory())
            {
                // directory
                generateFolderMeta(dir.getFilePath());
                generateMeta(dir.getFilePath());
            }
            else
            {
                // file
                const String filePath = dir.getFilePath();
                generateFileMeta(filePath);
            }

            working = dir.findNextFile();
        }

        dir.close();
    }

    //--------------------------------------------------------------------------

    TResult MetaFSMonitor::generateFileMeta(const String &path)
    {
        TResult ret = T3D_OK;

        do
        {
            if (!Dir::exists(path) || Dir::isDirectory(path))
            {
                break;
            }

            String dir, title, ext;
            if (!Dir::parsePath(path, dir, title, ext))
            {
                ret = T3D_ERR_FAIL;
                break;
            }

            if (ext == "meta" || ext == "tmp")
            {
                // meta 文件和临时文件直接跳过
                break;
            }

            String relPath = toRelativePath(path);
            MetaPtr meta;
            String metaPath = path + ".meta";
            bool exists = Dir::exists(metaPath);
            if (exists)
            {
                // 存在，直接读取
                FileDataStream fs;
                if (fs.open(metaPath.c_str(), FileDataStream::EOpenMode::E_MODE_READ_ONLY))
                {
                    meta = T3D_SERIALIZER_MGR.deserialize<Meta>(fs);
                    fs.close();
                }

                if (meta == nullptr)
                {
                    if (mPendingPaths.insert(relPath).second)
                    {
                        MFS_LOG_ERROR("Failed to read meta file %s !", metaPath.c_str());
                    }
                    ret = T3D_ERR_FAIL;
                    break;
                }
            }
            else
            {
                // 不存在，生成 meta
                meta = generateFileMeta(metaPath, dir, title, ext);
                if (meta == nullptr)
                {
                    if (mPendingPaths.insert(relPath).second)
                    {
                        MFS_LOG_ERROR("Failed to generate meta for file %s !", path.c_str());
                    }
                    ret = T3D_ERR_FAIL;
                    break;
                }
            }

            // 成功：移出等待重试集合，添加到缓存映射表中
            mPendingPaths.erase(relPath);
            addPathLUT(path, meta);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void MetaFSMonitor::generateFolderMeta(const String &path)
    {
        do
        {
            MetaPtr meta;
            String metaPath = path + ".meta";
            bool exists = Dir::exists(metaPath);
            if (exists)
            {
                // 存在 meta 文件，直接读取
                // FileDataStream fs;
                // if (fs.open(metaPath.c_str(), FileDataStream::EOpenMode::E_MODE_READ_ONLY))
                // {
                //     meta = T3D_SERIALIZER_MGR.deserialize<Meta>(fs);
                //     fs.close();
                // }
                // else
                // {
                //     MFS_LOG_ERROR("Failed to open file %s for reading !", metaPath.c_str());
                //     break;
                // }
            }
            else
            {
                // 不存在 meta 文件，创建一个
                meta = MetaFolder::create(UUID::generate());

                FileDataStream fs;
                if (fs.open(metaPath.c_str(), FileDataStream::EOpenMode::E_MODE_TRUNCATE|FileDataStream::EOpenMode::E_MODE_READ_WRITE|FileDataStream::EOpenMode::E_MODE_TEXT))
                {
                    T3D_SERIALIZER_MGR.serialize(fs, meta);
                    fs.close();
                }
                else
                {
                    MFS_LOG_ERROR("Failed to open file %s for writing !", metaPath.c_str());
                    break;
                }
            }
        } while (false);
    }

    //--------------------------------------------------------------------------

    MetaPtr MetaFSMonitor::generateFileMeta(const String &metaPath, const String &path, const String &title, const String &ext, void *userData)
    {
        MetaPtr meta;

        do
        {
            if (ext == Resource::EXT_MATERIAL
                    || ext == Resource::EXT_TEXTURE
                    || ext == Resource::EXT_SHADER
                    || ext == Resource::EXT_MESH
                    || ext == Resource::EXT_PREFAB
                    || ext == Resource::EXT_SCENE
                    || ext == Resource::EXT_ANIMATION
                    || ext == Resource::EXT_SKELETON)
            {
                ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(path, ARCHIVE_TYPE_FS, Archive::AccessMode::kRead);
                if (archive == nullptr)
                {
                    break;
                }
                String filename = title + "." + ext;

                Resource *res = static_cast<Resource *>(userData);
                
                if (ext == Resource::EXT_MATERIAL)
                {
                    MaterialPtr material = res;
                    if (res == nullptr)
                    {
                        material = T3D_MATERIAL_MGR.loadMaterial(archive, filename);
                    }
                    if (material == nullptr)
                    {
                        break;
                    }
                    meta = MetaMaterial::create(material->getUUID());
                }
                else if (ext == Resource::EXT_TEXTURE)
                {
                    TexturePtr texture = res;
                    if (res == nullptr)
                    {
                        texture = T3D_TEXTURE_MGR.loadTexture(archive, filename);
                    }
                    if (texture == nullptr)
                    {
                        break;
                    }
                    meta = MetaTexture::create(texture->getUUID());
                }
                else if (ext == Resource::EXT_SHADER)
                {
                    ShaderPtr shader = res;
                    if (res == nullptr)
                    {
                        shader = T3D_SHADER_MGR.loadShader(archive, filename);
                    }
                    if (shader == nullptr)
                    {
                        break;
                    }
                    meta = MetaShader::create(shader->getUUID());
                }
                else if (ext == Resource::EXT_MESH)
                {
                    MeshPtr mesh = res;
                    if (res == nullptr)
                    {
                        mesh = T3D_MESH_MGR.loadMesh(archive, filename);
                    }
                    if (mesh == nullptr)
                    {
                        break;
                    }
                    meta = MetaMesh::create(mesh->getUUID());
                }
                else if (ext == Resource::EXT_PREFAB)
                {
                    PrefabPtr prefab = res;
                    if (res == nullptr)
                    {
                        prefab = T3D_PREFAB_MGR.loadPrefab(archive, filename);
                    }
                    if (prefab == nullptr)
                    {
                        break;
                    }
                    meta = MetaPrefab::create(prefab->getUUID());
                }
                else if (ext == Resource::EXT_SCENE)
                {
                    Scene *scene = static_cast<Scene*>(res);
                    if (res == nullptr)
                    {
                        scene = T3D_SCENE_MGR.loadScene(archive, filename);
                    }
                    if (scene == nullptr)
                    {
                        break;
                    }
                    meta = MetaScene::create(scene->getUUID());
                }
                else if (ext == Resource::EXT_ANIMATION)
                {
                    SkeletalAnimation *anim = static_cast<SkeletalAnimation *>(res);
                    if (res == nullptr)
                    {
                        anim = T3D_ANIMATION_MGR.loadSkeletalAnimation(archive, filename);
                    }
                    if (anim == nullptr)
                    {
                        break;
                    }
                    meta = MetaAnimation::create(anim->getUUID());
                }
                else if (ext == Resource::EXT_SKELETON)
                {
                    Skeleton *skel = static_cast<Skeleton *>(res);
                    if (res == nullptr)
                    {
                        skel = T3D_SKELETON_MGR.loadSkeleton(archive, filename);
                    }
                    if (skel == nullptr)
                    {
                        break;
                    }
                    meta = MetaSkeleton::create(skel->getUUID());
                }
            }
            else if (ext == Resource::EXT_SHADERLAB)
            {
                meta = MetaShaderLab::create(UUID::generate(), UUID::INVALID);
            }
            else if (ext == Resource::EXT_TXT)
            {
                meta = MetaTxt::create(UUID::generate());
            }
            else if (ext == Resource::EXT_BIN)
            {
                meta = MetaBin::create(UUID::generate());
            }
            else if (ext == Resource::EXT_DYLIB)
            {
                meta = MetaDylib::create(UUID::generate());
            }
            else
            {
                meta = MetaFile::create(UUID::generate());
            }

            if (meta == nullptr)
            {
                break;
            }

            FileDataStream fs;
            if (fs.open(metaPath.c_str(), FileDataStream::EOpenMode::E_MODE_TRUNCATE|FileDataStream::EOpenMode::E_MODE_READ_WRITE|FileDataStream::EOpenMode::E_MODE_TEXT))
            {
                T3D_SERIALIZER_MGR.serialize(fs, meta);
                fs.close();
            }
            else
            {
                // 写失败
                MFS_LOG_ERROR("Failed to open file %s for writing !", metaPath.c_str());
                meta = nullptr;
                break;
            }
        } while (false);

        return meta;
    }

    //--------------------------------------------------------------------------

    void MetaFSMonitor::removeFileMeta(const String &path)
    {
        String metaPath = path + ".meta";
        Dir::remove(metaPath);
    }

    //--------------------------------------------------------------------------

    String MetaFSMonitor::toRelativePath(const String &path) const
    {
        String result = path;

        size_t rootLen = mRootPath.length();
        while (rootLen > 0 && (mRootPath[rootLen - 1] == '/' || mRootPath[rootLen - 1] == '\\'))
        {
            --rootLen;
        }

        if (rootLen > 0 && path.length() > rootLen)
        {
            bool prefixMatch = true;
            for (size_t i = 0; i < rootLen; ++i)
            {
                char c1 = path[i];
                char c2 = mRootPath[i];
                if (c1 == '/' || c1 == '\\')
                    c1 = Dir::getNativeSeparator();
                if (c2 == '/' || c2 == '\\')
                    c2 = Dir::getNativeSeparator();
                if (c1 != c2)
                {
                    prefixMatch = false;
                    break;
                }
            }

            if (prefixMatch)
            {
                char sep = path[rootLen];
                if (sep == '/' || sep == '\\' || sep == Dir::getNativeSeparator())
                {
                    result = path.substr(rootLen + 1);
                }
            }
        }

        for (char &c : result)
        {
            if (c == '/' || c == '\\')
            {
                c = Dir::getNativeSeparator();
            }
        }

        return result;
    }

    //--------------------------------------------------------------------------

    String MetaFSMonitor::toFullPath(const String &path) const
    {
        size_t rootLen = mRootPath.length();
        while (rootLen > 0 && (mRootPath[rootLen - 1] == '/' || mRootPath[rootLen - 1] == '\\'))
        {
            --rootLen;
        }

        String cleanRoot = mRootPath.substr(0, rootLen);

        if (rootLen > 0 && path.length() >= rootLen)
        {
            bool prefixMatch = true;
            for (size_t i = 0; i < rootLen; ++i)
            {
                char c1 = path[i];
                char c2 = mRootPath[i];
                if (c1 == '/' || c1 == '\\')
                    c1 = Dir::getNativeSeparator();
                if (c2 == '/' || c2 == '\\')
                    c2 = Dir::getNativeSeparator();
                if (c1 != c2)
                {
                    prefixMatch = false;
                    break;
                }
            }

            if (prefixMatch)
            {
                if (path.length() == rootLen)
                {
                    return cleanRoot;
                }
                char sep = path[rootLen];
                if (sep == '/' || sep == '\\' || sep == Dir::getNativeSeparator())
                {
                    return cleanRoot + Dir::getNativeSeparator() + toRelativePath(path);
                }
            }
        }

        return cleanRoot + Dir::getNativeSeparator() + toRelativePath(path);
    }

    //--------------------------------------------------------------------------

    void MetaFSMonitor::addPathLUT(const String &path, const MetaPtr &meta)
    {
        if (meta == nullptr)
        {
            return;
        }

        String relativePath = toRelativePath(path);
        T3D_ASSERT(!relativePath.empty());
        const UUID &newUUID = meta->getUUID();

        // 1. 该路径已有条目且 UUID 不同：先从 mUUIDMappings 删除旧 UUID
        auto pathItr = mPathMappings.find(relativePath);
        if (pathItr != mPathMappings.end() && pathItr->second != nullptr)
        {
            const UUID &oldUUID = pathItr->second->getUUID();
            if (oldUUID != newUUID)
            {
                auto uuidItr = mUUIDMappings.find(oldUUID);
                if (uuidItr != mUUIDMappings.end() && uuidItr->second == relativePath)
                {
                    mUUIDMappings.erase(uuidItr);
                }
            }
        }

        // 2. 新 UUID 已被另一路径占用：输出重复 UUID 警告（P8），保留先登记的那个
        auto uuidItr = mUUIDMappings.find(newUUID);
        if (uuidItr != mUUIDMappings.end() && uuidItr->second != relativePath)
        {
            MFS_LOG_WARNING("Duplicate UUID [%s] detected: [%s] conflicts with existing [%s], keeping existing mapping !",
                newUUID.toString().c_str(), relativePath.c_str(), uuidItr->second.c_str());
            mPathMappings[relativePath] = meta;
            return;
        }

        // 3. 覆盖写入
        mPathMappings[relativePath] = meta;
        mUUIDMappings[newUUID] = relativePath;
    }

    //--------------------------------------------------------------------------

    void MetaFSMonitor::removePathLUT(const String &path)
    {
        String relativePath = toRelativePath(path);
        auto itr = mPathMappings.find(relativePath);
        if (itr != mPathMappings.end())
        {
            if (itr->second != nullptr)
            {
                auto uuidItr = mUUIDMappings.find(itr->second->getUUID());
                if (uuidItr != mUUIDMappings.end() && uuidItr->second == relativePath)
                {
                    mUUIDMappings.erase(uuidItr);
                }
            }
            mPathMappings.erase(itr);
        }
    }

    //--------------------------------------------------------------------------
    
    TResult MetaFSMonitor::start()
    {
        TResult ret = T3D_OK;

        do
        {
            FSMonitorExts excludeExts;
            excludeExts.emplace_back("meta");
            excludeExts.emplace_back("tmp");
            FSMonitorExcludes excludeFolders;
            ret = mFSMonitor->startMonitor(mRootPath, excludeExts, excludeFolders,
                [this](const String &path, FSMonitorAction action)
                {
                    onFileChanged(path, action);
                });
            if (T3D_FAILED(ret))
            {
                MFS_LOG_ERROR("Start monitor path %s failed ! ERROR [%d]", mRootPath.c_str(), ret);
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void MetaFSMonitor::onFileAdded(const String &path)
    {
        if (path == mRootPath)
        {
            return;
        }
        else if (Dir::isDirectory(path))
        {
            generateFolderMeta(path);
        }
        else
        {
            // 文件：meta 已存在时读取并登记，不存在时生成并登记
            generateFileMeta(path);
        }
    }

    //--------------------------------------------------------------------------

    void MetaFSMonitor::onFileRemoved(const String &path)
    {
        if (Dir::exists(path))
        {
            // 文件仍然存在：说明是被覆盖式替换而非删除，改为调用 refresh，不删除 meta
            if (!Dir::isDirectory(path))
            {
                refresh(path);
            }
        }
        else
        {
            removeFileMeta(path);
            removePathLUT(path);
            mPendingPaths.erase(toRelativePath(path));
        }
    }

    //--------------------------------------------------------------------------

    void MetaFSMonitor::onFileChanged(const String &path, FSMonitorAction action)
    {
        switch (action)
        {
        case FSMonitorAction::kAdded:
            {
                // 新增文件
                onFileAdded(path);
            }
            break;
        case FSMonitorAction::kRemoved:
            {
                // 删除文件
                onFileRemoved(path);
            }
            break;
        case FSMonitorAction::kModifed:
            {
                // 修改文件：刷新映射表或重试未完成的加载
                if (!Dir::isDirectory(path))
                {
                    refresh(path);
                }
            }
            break;
        case FSMonitorAction::kRenamedOld:
            {
                // 重命名，老文件名，先删除
                onFileRemoved(path);
            }
            break;
        case FSMonitorAction::kRenamedNew:
            {
                // 重命名，新文件名，创建新的 meta
                onFileAdded(path);
            }
            break;
        default:
            break;
        }
    }

    //--------------------------------------------------------------------------

    TResult MetaFSMonitor::stop()
    {
        TResult ret = T3D_OK;

        do
        {
            ret = mFSMonitor->stopMonitor();
            if (T3D_FAILED(ret))
            {
                MFS_LOG_ERROR("Stop monitor path %s failed ! ERROR [%d]", mRootPath.c_str(), ret);
                break;
            }
        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------
}

