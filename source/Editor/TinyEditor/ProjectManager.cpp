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


#include "ProjectManager.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------

    const char *ProjectManager::ASSETS = "Assets";
    const char *ProjectManager::SCENES = "Scenes";
    const char *ProjectManager::TEMP = "Temp";
    
    //--------------------------------------------------------------------------

    ProjectManager::ProjectManager()
    {
        mFSMonitor = new FileSystemMonitor();
    }
    
    //--------------------------------------------------------------------------

    ProjectManager::~ProjectManager()
    {
        T3D_SAFE_DELETE(mFSMonitor);
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::createProject(const String &path, const String &name)
    {
        TResult ret = T3D_OK;

        do
        {
            // 创建工程文件夹
            String projectPath = path + Dir::getNativeSeparator() + name;
            if (!Dir::makeDir(projectPath))
            {
                EDITOR_LOG_ERROR("Create project folder [%s] failed !", projectPath.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            // 创建资产文件夹
            String assetsPath = projectPath + Dir::getNativeSeparator() + ASSETS;
            if (!Dir::makeDir(assetsPath))
            {
                EDITOR_LOG_ERROR("Create assets folder [%s] failed !", assetsPath.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            // 创建 Temp 文件夹
            String tempPath = projectPath + Dir::getNativeSeparator() + TEMP;
            if (!Dir::makeDir(tempPath))
            {
                EDITOR_LOG_ERROR("Create temp folder [%s] failed !", tempPath.c_str());
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
                EDITOR_LOG_ERROR("Copy builtin textures from editor [%s] to temporary folder [%s] failed !", srcPath.c_str(), dstPath.c_str());
                ret = T3D_ERR_COPY_DIR;
                break;
            }
            
            // shaders
            srcPath = builtinPath + Dir::getNativeSeparator() + "shaders";
            rval = Dir::copyDir(srcPath, dstPath, false);
            if (!rval)
            {
                EDITOR_LOG_ERROR("Copy builtin shaders from editor [%s] to temporary folder [%s] failed !", srcPath.c_str(), dstPath.c_str());
                ret = T3D_ERR_COPY_DIR;
                break;
            }
            
            // materials
            srcPath = builtinPath + Dir::getNativeSeparator() + "materials";
            rval = Dir::copyDir(srcPath, dstPath, false);
            if (!rval)
            {
                EDITOR_LOG_ERROR("Copy builtin materials from editor [%s] to temporary folder [%s] failed !", srcPath.c_str(), dstPath.c_str());
                ret = T3D_ERR_COPY_DIR;
                break;
            }

            // meshes
            srcPath = builtinPath + Dir::getNativeSeparator() + "meshes";
            rval = Dir::copyDir(srcPath, dstPath, false);
            if (!rval)
            {
                EDITOR_LOG_ERROR("Copy builtin meshes from editor [%s] to temporary folder [%s] failed !", srcPath.c_str(), dstPath.c_str());
                ret = T3D_ERR_COPY_DIR;
                break;
            }
            
            mPath = path;
            mName = name;
            mAssetsPath = assetsPath;
            mTempPath = tempPath;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::openProject(const String &path, const String &name)
    {
        TResult ret = T3D_OK;

        do
        {
            String projectPath = path + Dir::getNativeSeparator() + name;

            if (!Dir::exists(projectPath))
            {
                EDITOR_LOG_ERROR("Open project [%s] failed !", projectPath.c_str());
                ret = T3D_ERR_FILE_NOT_EXIST;
                break;
            }

            String assetsPath = projectPath + Dir::getNativeSeparator() + ASSETS;
            if (!Dir::exists(assetsPath))
            {
                EDITOR_LOG_ERROR("Assets folder [%s] did not exist !", assetsPath.c_str());
                ret = T3D_ERR_FILE_NOT_EXIST;
                break;
            }

            String tempPath = projectPath + Dir::getNativeSeparator() + TEMP;
            if (!Dir::exists(tempPath))
            {
                EDITOR_LOG_ERROR("Temporary folder [%s] did not exist ! ", tempPath.c_str());
                ret = T3D_ERR_FILE_NOT_EXIST;
                break;
            }

            mPath = path;
            mName = name;
            mAssetsPath = assetsPath;
            mTempPath = tempPath;
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::closeProject()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    NS_END
}

