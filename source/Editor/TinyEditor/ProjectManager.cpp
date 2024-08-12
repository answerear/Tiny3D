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

            // 创建场景文件夹
            String scenesPath = assetsPath + Dir::getNativeSeparator() + SCENES;
            if (!Dir::makeDir(scenesPath))
            {
                EDITOR_LOG_ERROR("Create scenes folder [%s] failed !", scenesPath.c_str());
                ret = T3D_ERR_FAIL;
                break;
            }

            mPath = path;
            mName = name;
            mAssetsPath = assetsPath;
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
                EDITOR_LOG_ERROR("Open assets [%s] failed !", assetsPath.c_str());
                ret = T3D_ERR_FILE_NOT_EXIST;
                break;
            }

            mPath = path;
            mName = name;
            mAssetsPath = assetsPath;
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

