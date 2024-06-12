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
#include "ImErrors.h"


namespace Tiny3D
{
    NS_BEGIN(Launcher)
    
    //--------------------------------------------------------------------------

    const char *ProjectManager::PROJECT_DATA_FILE = "ProjectData.dat";
    
    //--------------------------------------------------------------------------

    ProjectManager::ProjectManager()
    {
        
    }
    
    //--------------------------------------------------------------------------

    ProjectManager::~ProjectManager()
    {
        releaseProjectInfo();
    }

    //--------------------------------------------------------------------------
    
    void ProjectManager::releaseProjectInfo()
    {
        for (auto info : mProjectData.projects)
        {
            T3D_SAFE_DELETE(info);
        }

        mProjectData.projects.clear();
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::loadProjects()
    {
        TResult ret = T3D_OK;

        do
        {
            String path = Dir::getCachePath() + Dir::getNativeSeparator() + PROJECT_DATA_FILE;

            FileDataStream fs;
            if (!fs.open(path.c_str(), FileDataStream::EOpenMode::E_MODE_READ_ONLY))
            {
                // 
                break;
            }

            JsonSerializerPtr serializer = JsonSerializer::create();
            ret = serializer->deserializeObject(fs, mProjectData);

            fs.close();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::saveProjects()
    {
        TResult ret = T3D_OK;

        do
        {
            String path = Dir::getCachePath() + Dir::getNativeSeparator() + PROJECT_DATA_FILE;

            FileDataStream fs;
            if (!fs.open(path.c_str(), FileDataStream::EOpenMode::E_MODE_TRUNCATE|FileDataStream::EOpenMode::E_MODE_WRITE_ONLY))
            {
                break;
            }

            JsonSerializerPtr serializer = JsonSerializer::create();
            ret = serializer->serializeObject(fs, mProjectData);

            fs.close();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::createProject(const String &path, const String &name)
    {
        TResult ret = T3D_OK;

        do
        {
            auto it = std::find_if(
                mProjectData.projects.begin(),
                mProjectData.projects.end(),
                [&path, &name](ProjectInfo *info)
                {
                    return (path == info->path && name == info->name);
                });

            if (it != mProjectData.projects.end())
            {
                // 已经有同路径同名工程
                ret = T3D_ERR_DUPLICATED_ITEM;
                T3D_LOG_ERROR(LOG_TAG_LAUNCHER, "Duplicated project ! Path=[%s], Name=[%s]", path.c_str(), name.c_str());
                break;
            }
            
            // 新工程
            ProjectInfo *project = new ProjectInfo();
            project->name = name;
            project->path = path;
            project->recent = DateTime::currentSecsSinceEpoch();
            mProjectData.projects.emplace_back(project);

            // 重新排序
            sort();

            // 启动编辑器
            // ret = startTinyEditor(path, name, true);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::removeProject(const String &path, const String &name)
    {
        TResult ret = T3D_OK;

        do
        {
            ret = T3D_ERR_NOT_FOUND;
            
            for (auto itr = mProjectData.projects.begin(); itr != mProjectData.projects.end(); ++itr)
            {
                ProjectInfo *info = (*itr);
                if (info->path == path && info->name == name)
                {
                    mProjectData.projects.erase(itr);
                    T3D_SAFE_DELETE(info);
                    ret = T3D_OK;
                    break;
                }
            }

            if (T3D_FAILED(ret))
            {
                T3D_LOG_WARNING(LOG_TAG_LAUNCHER, "Project [Name=%s, Path=%s]not found when remove project !", name.c_str(), path.c_str());
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::removeProject(int32_t index)
    {
        TResult ret = T3D_OK;

        do
        {
            if (index >= mProjectData.projects.size())
            {
                ret = T3D_ERR_NOT_FOUND;
                break;
            }

            mProjectData.projects.erase(mProjectData.projects.begin()+index);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectManager::openProject(const String &path, const String &name)
    {
        TResult ret = T3D_OK;

        do
        {
            auto it = std::find_if(
                mProjectData.projects.begin(),
                mProjectData.projects.end(),
                [&path, &name](ProjectInfo *info)
                {
                    return (path == info->path && name == info->path);
                });

            if (it == mProjectData.projects.end())
            {
                // 该路径下不存在该名称的工程
                ret = T3D_ERR_NOT_FOUND;
                T3D_LOG_ERROR(LOG_TAG_LAUNCHER, "Project does not found ! Path=%s, Name=%s", path.c_str(), name.c_str());
                break;
            }

            // 重新排序
            ProjectInfo *info = (*it);
            info->recent = DateTime::currentSecsSinceEpoch();
            sort();

            // 启动编辑器
            ret = startTinyEditor(path, name, false);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------
    
    TResult ProjectManager::startTinyEditor(const String &path, const String &name, bool isNewProject)
    {
        TResult ret = T3D_OK;

        do
        {
            String appPath = Dir::getAppPath();
            String editorAppPath = appPath + Dir::getNativeSeparator() + "TinyEditor.exe";
            
            String cmdline = "path=" + path + " name=" + name;
            
            if (isNewProject)
            {
                cmdline = cmdline + " newProject=1";
            }
            else
            {
                cmdline = cmdline + " newProject=0";
            }

            cmdline = cmdline + " ip=127.0.0.1 port=5327";
            
            Process proc;
            ret = proc.start(editorAppPath, cmdline);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void ProjectManager::sort()
    {
        mDisplayProjects = mProjectData.projects;

        SortType type = mProjectData.sortType;
        bool ascending = mProjectData.ascending;

        // mDisplayProjects.sort(
        std::sort(mDisplayProjects.begin(), mDisplayProjects.end(),
            [type, ascending](ProjectInfo *p1, ProjectInfo *p2)
            {
                switch (type)
                {
                case SortType::kRecent:
                    {
                        if (ascending)
                        {
                            return p1->recent > p2->recent;
                        }
                        else
                        {
                            return p1->recent < p2->recent;
                        }
                    }
                    break;
                case SortType::kName:
                    {
                        if (ascending)
                        {
                            return p1->name < p2->name;
                        }
                        else
                        {
                            return p1->name > p2->name;
                        }
                    }
                    break;
                case SortType::kPath:
                    {
                        if (ascending)
                        {
                            return p1->path < p2->path;
                        }
                        else
                        {
                            return p1->path > p2->path;
                        }
                    }
                    break;
                case SortType::kTag:
                    {
                        if (ascending)
                        {
                            return p1->tags < p2->tags;
                        }
                        else
                        {
                            return p1->tags > p2->tags;
                        }
                    }
                    break;
                }

                T3D_ASSERT(0, "Invalid sort type !");
                return true;
            });
    }

    //--------------------------------------------------------------------------

    NS_END
}
