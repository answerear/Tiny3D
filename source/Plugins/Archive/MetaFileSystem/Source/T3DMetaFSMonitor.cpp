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
        return new MetaFSMonitor(rootPath);
    }
    
    //--------------------------------------------------------------------------

    MetaFSMonitor::MetaFSMonitor(const String &rootPath)
        : mFSMonitor(new FileSystemMonitor())
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

    void MetaFSMonitor::generateFileMeta(const String &path)
    {
        do
        {
            String dir, title, ext;
            if (!Dir::parsePath(path, dir, title, ext))
            {
                break;
            }

            if (ext == "meta")
            {
                // meta 文件直接跳过
                break;
            }
            
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
                else
                {
                    // 无法打开文件
                    MFS_LOG_ERROR("Failed to open file %s for reading !", metaPath.c_str());
                    break;
                }
            }
            else
            {
                // 不存在，生成 meta
                String filename = dir + Dir::getNativeSeparator() + title + "." + ext;
                meta = generateFileMeta(metaPath, dir, title, ext);
                if (meta == nullptr)
                {
                    break;
                }
            }

            T3D_ASSERT(meta != nullptr);

            // 添加到缓存映射表中
            addPathLUT(path, meta);
        } while (false);
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

    MetaPtr MetaFSMonitor::generateFileMeta(const String &metaPath, const String &path, const String &title, const String &ext)
    {
        MetaPtr meta;

        do
        {
            if (ext == Resource::EXT_MATERIAL
                    || ext == Resource::EXT_TEXTURE
                    || ext == Resource::EXT_SHADER
                    || ext == Resource::EXT_MESH
                    || ext == Resource::EXT_PREFAB
                    || ext == Resource::EXT_SCENE)
            {
                // ResourcePtr resource;
                // FileDataStream fs;
                // if (fs.open(path.c_str(), FileDataStream::EOpenMode::E_MODE_READ_ONLY))
                // {
                //     resource = T3D_SERIALIZER_MGR.deserialize<Resource>(fs);
                //     fs.close();
                // }
                // else
                // {
                //     MFS_LOG_ERROR("Failed to open file %s for reading !", path.c_str());
                //     break;
                // }

                ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(path, ARCHIVE_TYPE_FS, Archive::AccessMode::kRead);
                T3D_ASSERT(archive != nullptr);
                String filename = title + "." + ext;

                if (ext == Resource::EXT_MATERIAL)
                {
                    MaterialPtr material = T3D_MATERIAL_MGR.loadMaterial(archive, filename);
                    meta = MetaMaterial::create(material->getUUID());
                }
                else if (ext == Resource::EXT_TEXTURE)
                {
                    TexturePtr texture = T3D_TEXTURE_MGR.loadTexture(archive, filename);
                    meta = MetaTexture::create(texture->getUUID());
                }
                else if (ext == Resource::EXT_SHADER)
                {
                    ShaderPtr shader = T3D_SHADER_MGR.loadShader(archive, filename);
                    meta = MetaShader::create(shader->getUUID());
                }
                else if (ext == Resource::EXT_MESH)
                {
                    MeshPtr mesh = T3D_MESH_MGR.loadMesh(archive, filename);
                    meta = MetaMesh::create(mesh->getUUID());
                }
                else if (ext == Resource::EXT_PREFAB)
                {
                    PrefabPtr prefab = T3D_PREFAB_MGR.loadPrefab(archive, filename);
                    meta = MetaPrefab::create(prefab->getUUID());
                }
                else if (ext == Resource::EXT_SCENE)
                {
                    ScenePtr scene = T3D_SCENE_MGR.loadScene(archive, filename);
                    meta = MetaScene::create(scene->getUUID());
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
                break;
            }
        } while (false);

        return meta;
    }

    //--------------------------------------------------------------------------

    void MetaFSMonitor::removeFileMeta(const String &path)
    {
        
    }

    //--------------------------------------------------------------------------

    void MetaFSMonitor::addPathLUT(const String &path, const MetaPtr &meta)
    {
        // auto pos = path.find_first_not_of(mRootPath);
        auto pos = mRootPath.length() + 1;  // 这里还要跳过 '\' 路径分隔符 
        if (pos != String::npos)
        {
            String relativePath = path.substr(pos);
            T3D_ASSERT(!relativePath.empty());
            mPathMappings.emplace(relativePath, meta);
            mUUIDMappings.emplace(meta->getUUID(), relativePath);
        }
    }

    //--------------------------------------------------------------------------

    void MetaFSMonitor::removePathLUT(const String &path)
    {
        auto pos = path.find_first_not_of(mRootPath);
        if (pos != String::npos)
        {
            String relativePath = path.substr(pos);
            const auto &itr = mPathMappings.find(relativePath);
            if (itr != mPathMappings.end())
            {
                mUUIDMappings.erase(itr->second->getUUID());
                mPathMappings.erase(itr);
            }
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
        MetaPtr meta;
        String dir, title, ext;
        Dir::parsePath(path, dir, title, ext);
        
        if (path == mRootPath)
        {
            // int a = 0;
        }
        else if (std::filesystem::is_directory(path))
        {
            generateFolderMeta(path);
        }
        else
        {
            // 文件
            String metaPath = path + ".meta";
            meta = generateFileMeta(metaPath, dir, title, ext);
            if (meta != nullptr)
            {
                addPathLUT(path, meta);
            }
            else
            {
                MFS_LOG_ERROR("Failed to add new file %s corresponding meta file !", path.c_str());
            }
        }
    }

    //--------------------------------------------------------------------------

    void MetaFSMonitor::onFileRemoved(const String &path)
    {
        removeFileMeta(path);
        removePathLUT(path);
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

