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


#include "T3DMetaFSArchivePrerequisites.h"
#include "T3DMetaFSMonitorManager.h"
#include "T3DMetaFSMonitor.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    MetaFSMonitorManager::~MetaFSMonitorManager()
    {
        removeAllMonitors();
    }

    //--------------------------------------------------------------------------

    MetaFSMonitor *MetaFSMonitorManager::addMonitor(const String &path)
    {
        MetaFSMonitorPtr monitor;

        do
        {
            
                
            // 找出是否已经有启动监控的，子文件夹都在监控中，不重复监控
            for (const auto &item : mPathMonitors)
            {
                if (item.first == path)
                {
                    // 完全相同路径
                    monitor = item.second;
                    break;
                }

                // String::size_type pos = name.find_first_not_of(item.first);
                bool match = StringUtil::match(path, item.first, false);
                if (match)
                {
                    // 包含了部分路径，name 是其子路径
                    monitor = item.second;
                    break;
                }
            }
                    
            if (monitor == nullptr)
            {
                // 创建监控对象
                monitor = MetaFSMonitor::create(path);
                if (T3D_FAILED(monitor->init()))
                {
                    MFS_LOG_ERROR("Failed to initialize monitor %s.", path.c_str());
                    break;
                }

                mPathMonitors.emplace(path, monitor);
            }
        } while (false);
        
        return monitor;
    }

    //--------------------------------------------------------------------------

    TResult MetaFSMonitorManager::removeMonitor(const String &path)
    {
        TResult ret = T3D_OK;

        mPathMonitors.erase(path);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MetaFSMonitorManager::removeAllMonitors()
    {
        for (const auto &item : mPathMonitors)
        {
            item.second->stop();
        }
        mPathMonitors.clear();
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    // const String &MetaFSMonitorManager::getPath(const UUID &uuid) const
    // {
    //     for (const auto &item : mPathMonitors)
    //     {
    //         const String &path = item.second->getPath(uuid);
    //         if (!path.empty())
    //         {
    //             return path;
    //         }
    //     }
    //
    //     static String EMPTY;
    //     return EMPTY;
    // }
    //
    // //--------------------------------------------------------------------------
    //
    // MetaFSMonitor *MetaFSMonitorManager::getMonitor(const UUID &uuid) const
    // {
    //     MetaFSMonitor *monitor = nullptr;
    //     
    //     for (const auto &item : mPathMonitors)
    //     {
    //         const String &path = item.second->getPath(uuid);
    //         if (!path.empty())
    //         {
    //             monitor = item.second;
    //             break;
    //         }
    //     }
    //     
    //     return monitor;
    // }

    //--------------------------------------------------------------------------

    bool MetaFSMonitorManager::getPathAndMonitor(const UUID &uuid, MetaFSMonitor *mainMonitor, String &path, MetaFSMonitor *&monitor)
    {
        bool ret = false;

        // 先启动 monitor 查找
        Meta *meta = nullptr;
        bool found = mainMonitor->getPathAndMeta(uuid, path, meta);

        if (meta->getType() == Meta::Type::kShaderLab)
        {
            // 是 shader lab
            MetaShaderLab *metaShaderLab = static_cast<MetaShaderLab *>(meta);
            path = mainMonitor->getPath(metaShaderLab->getShaderUUID());
                
            if (path.empty())
            {
                // 从 main monitor 里面没找到，只能全局搜
                for (const auto &item : mPathMonitors)
                {
                    if (item.second == mainMonitor)
                        continue;

                    path = item.second->getPath(metaShaderLab->getShaderUUID());
                        
                    if (!path.empty())
                    {
                        // 找到了
                        monitor = item.second;
                        ret = true;
                    }
                }
            }
            else
            {
                // 找到了
                monitor = mainMonitor;
                ret = true;
            }
        }
        else
        {
            // 不是 shader lab
            if (found)
            {
                // 找到了
                monitor = mainMonitor;
                ret = true;
            }
            else
            {
                // 没找到，从全局再找一次
                for (const auto &item : mPathMonitors)
                {
                    if (item.second == mainMonitor)
                        continue;

                    path = item.second->getPath(uuid);
                    if (!path.empty())
                    {
                        // 找到了
                        monitor = item.second;
                        ret = true;
                    }
                }
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------
}

