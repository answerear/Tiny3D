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


#include "T3DMetaFSProjectManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    const char *MFSProjectManager::ASSETS = "Assets";
    const char *MFSProjectManager::SCENES = "Scenes";
    const char *MFSProjectManager::TEMP = "Temp";
    const char *MFSProjectManager::MAPPINGS_FILE_NAME = "project.idx";
    
    //--------------------------------------------------------------------------

    MFSProjectManager::MFSProjectManager()
    {
        mFSMonitor = new FileSystemMonitor();
    }
    
    //--------------------------------------------------------------------------

    MFSProjectManager::~MFSProjectManager()
    {
        T3D_SAFE_DELETE(mFSMonitor);
    }

    //--------------------------------------------------------------------------

    TResult MFSProjectManager::createProject(const String &path, const String &name)
    {
        TResult ret = T3D_OK;

        do
        {
            // 创建工程文件夹
            String projectPath = path + Dir::getNativeSeparator() + name;
            if (!Dir::makeDir(projectPath))
            {
                MFS_LOG_ERROR("Create project folder [%s] failed !", projectPath.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            // 创建资产文件夹
            String assetsPath = projectPath + Dir::getNativeSeparator() + ASSETS;
            if (!Dir::makeDir(assetsPath))
            {
                MFS_LOG_ERROR("Create assets folder [%s] failed !", assetsPath.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            // 创建 Temp 文件夹
            String tempPath = projectPath + Dir::getNativeSeparator() + TEMP;
            if (!Dir::makeDir(tempPath))
            {
                MFS_LOG_ERROR("Create temp folder [%s] failed !", tempPath.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            // 复制 builtin assets 到 temp 文件夹
            String builtinPath = Dir::getAppPath() + Dir::getNativeSeparator() + "Editor" + Dir::getNativeSeparator() + "builtin";
            
            // textures
            String srcPath = builtinPath + Dir::getNativeSeparator() + "textures";
            String dstPath = tempPath + Dir::getNativeSeparator() + "builtin";
            bool rval = Dir::copyDir(srcPath, dstPath, false);
            if (!rval)
            {
                MFS_LOG_ERROR("Copy builtin textures from editor [%s] to temporary folder [%s] failed !", srcPath.c_str(), dstPath.c_str());
                ret = T3D_ERR_COPY_DIR;
                break;
            }
            
            // shaders
            srcPath = builtinPath + Dir::getNativeSeparator() + "shaders";
            rval = Dir::copyDir(srcPath, dstPath, false);
            if (!rval)
            {
                MFS_LOG_ERROR("Copy builtin shaders from editor [%s] to temporary folder [%s] failed !", srcPath.c_str(), dstPath.c_str());
                ret = T3D_ERR_COPY_DIR;
                break;
            }
            
            // materials
            srcPath = builtinPath + Dir::getNativeSeparator() + "materials";
            rval = Dir::copyDir(srcPath, dstPath, false);
            if (!rval)
            {
                MFS_LOG_ERROR("Copy builtin materials from editor [%s] to temporary folder [%s] failed !", srcPath.c_str(), dstPath.c_str());
                ret = T3D_ERR_COPY_DIR;
                break;
            }

            // meshes
            srcPath = builtinPath + Dir::getNativeSeparator() + "meshes";
            rval = Dir::copyDir(srcPath, dstPath, false);
            if (!rval)
            {
                MFS_LOG_ERROR("Copy builtin meshes from editor [%s] to temporary folder [%s] failed !", srcPath.c_str(), dstPath.c_str());
                ret = T3D_ERR_COPY_DIR;
                break;
            }
            
            mPath = projectPath;
            mName = name;
            mAssetsPath = assetsPath;
            mTempPath = tempPath;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MFSProjectManager::openProject(const String &path, const String &name)
    {
        TResult ret = T3D_OK;

        do
        {
            String projectPath = path + Dir::getNativeSeparator() + name;

            if (!Dir::exists(projectPath))
            {
                MFS_LOG_ERROR("Open project [%s] failed !", projectPath.c_str());
                ret = T3D_ERR_FILE_NOT_EXIST;
                break;
            }

            String assetsPath = projectPath + Dir::getNativeSeparator() + ASSETS;
            if (!Dir::exists(assetsPath))
            {
                MFS_LOG_ERROR("Assets folder [%s] did not exist !", assetsPath.c_str());
                ret = T3D_ERR_FILE_NOT_EXIST;
                break;
            }

            String tempPath = projectPath + Dir::getNativeSeparator() + TEMP;
            if (!Dir::exists(tempPath))
            {
                MFS_LOG_ERROR("Temporary folder [%s] did not exist ! ", tempPath.c_str());
                ret = T3D_ERR_FILE_NOT_EXIST;
                break;
            }

            mPath = projectPath;
            mName = name;
            mAssetsPath = assetsPath;
            mTempPath = tempPath;
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MFSProjectManager::closeProject()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult MFSProjectManager::buildMappings()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MFSProjectManager::readMappings()
    {
        TResult ret = T3D_OK;

        do
        {
            String filename = mProjectPath + Dir::getNativeSeparator() + MAPPINGS_FILE_NAME;
            FileDataStream fs;

            // 打開映射文件
            if (!fs.open(filename.c_str(), FileDataStream::EOpenMode::E_MODE_READ_ONLY))
            {
                T3D_LOG_ERROR(LOG_TAG_METAFS, "Failed to open file %s !", filename.c_str());
                ret = T3D_ERR_FILE_NOT_EXIST;
                break;
            }

            // 从文件读取生成映射对象
            ret = T3D_SERIALIZER_MGR.deserialize(fs, mMappings);
            fs.close();

            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_METAFS, "Failed to deserialize object !");
                ret = T3D_ERR_DESERIALIZE_OBJECT;
                break;
            }

            // 从读取回来的映射文件建立 UUID => Path 的映射关系
            for (const auto &item : mMappings.paths)
            {
                mMappings.uuids.emplace(item.second, item.first);
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MFSProjectManager::writeMappings()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void MFSProjectManager::refresh(const UUID &uuid, const String &path)
    {
        if (path.empty())
            return;

        String oldPath;
        
        auto itr = mMappings.uuids.find(uuid);
        if (itr == mMappings.uuids.end())
        {
            // 没有保存过 UUID，可能 UUID 更新了
            mMappings.uuids.emplace(uuid, path);
        }
        else
        {
            if (itr->second != path)
            {
                // 路径更新了
                oldPath = itr->second;
                itr->second = path;
                mIsDirty = true;
            }
        }

        if (!oldPath.empty())
        {
            auto it = mMappings.paths.find(oldPath);
            if (it != mMappings.paths.end())
            {
                // 路径映射里面有老的路径，先删除
                mMappings.paths.erase(oldPath);
                mIsDirty = true;
            }
        }

        auto it = mMappings.paths.find(path);
        if (it != mMappings.paths.end())
        {
            if (uuid != it->second)
            {
                // 路径映射里面有路径，并且 UUID 更新了
                it->second = uuid;
                mIsDirty = true;
            }
        }
        else
        {
            // 路径映射里面没有路径，直接更新 UUID
            mMappings.paths.emplace(path, uuid);
            mIsDirty = true;
        }
    }

    //--------------------------------------------------------------------------

    const String &MFSProjectManager::getPathByUUID(const UUID &uuid) const
    {
        static String empty;

        auto itr = mMappings.uuids.find(uuid);
        if (itr != mMappings.uuids.end())
        {
            return itr->second;
        }

        return empty;
    }

    //--------------------------------------------------------------------------

    const UUID &MFSProjectManager::getUUIDByPath(const String &path) const
    {
        auto itr = mMappings.paths.find(path);
        if (itr != mMappings.paths.end())
        {
            return itr->second;
        }
        
        return UUID::INVALID;
    }
    
    //--------------------------------------------------------------------------
}

