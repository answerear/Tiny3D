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
    T3D_INSTANTIATE_SINGLETON(MetaFSMonitorManager);

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

                if (T3D_FAILED(monitor->start()))
                {
                    MFS_LOG_ERROR("Failed to start monitor %s.", path.c_str());
                    mPathMonitors.erase(path);
                    break;
                }
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
        ret = mainMonitor->getPathAndMeta(uuid, path, meta);
        if (!ret)
        {
            // 从 main monitor 里面没找到，只能全局搜
            for (const auto &item : mPathMonitors)
            {
                if (item.second == mainMonitor)
                    continue;

                ret = item.second->getPathAndMeta(uuid, path, meta);
                if (ret)
                {
                    monitor = item.second;
                    break;
                }
                // path = item.second->getPath(uuid);
                // if (!path.empty())
                // {
                //     // 找到了
                //     monitor = item.second;
                //     ret = true;
                //     break;
                // }
            }
        }
        else
        {
            monitor = mainMonitor;
        }

        if (ret)
        {
            T3D_ASSERT(meta != nullptr);

            if (meta->getType() == Meta::Type::kShaderLab)
            {
                monitor = nullptr;
                
                // shader lab ，再搜一遍找到转换后的 shader
                MetaShaderLab *metaShaderLab = static_cast<MetaShaderLab *>(meta);
                ret = getPathAndMonitor(metaShaderLab->getShaderUUID(), mainMonitor, path, monitor);
            }
        }

#if 0
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
                        break;
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
            // if (found)
            {
                // 找到了
                // monitor = mainMonitor;
                ret = true;
            }
            // else
            // {
            //     // 没找到，从全局再找一次
            //     for (const auto &item : mPathMonitors)
            //     {
            //         if (item.second == mainMonitor)
            //             continue;
            //     
            //         path = item.second->getPath(uuid);
            //         if (!path.empty())
            //         {
            //             // 找到了
            //             monitor = item.second;
            //             ret = true;
            //             break;
            //         }
            //     }
            // }
        }
#endif
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MetaFSMonitorManager::refresh(const String &absPath)
    {
        MetaFSMonitor *matchedMonitor = nullptr;
        size_t bestLen = 0;

        for (const auto &item : mPathMonitors)
        {
            const String &rootPath = item.first;
            size_t rootLen = rootPath.length();
            while (rootLen > 0 && (rootPath[rootLen - 1] == '/' || rootPath[rootLen - 1] == '\\'))
            {
                --rootLen;
            }

            if (rootLen == 0 || absPath.length() < rootLen)
            {
                continue;
            }

            bool prefixMatch = true;
            for (size_t i = 0; i < rootLen; ++i)
            {
                char c1 = absPath[i];
                char c2 = rootPath[i];
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
                if (absPath.length() == rootLen
                    || absPath[rootLen] == '/'
                    || absPath[rootLen] == '\\'
                    || absPath[rootLen] == Dir::getNativeSeparator())
                {
                    if (rootLen > bestLen)
                    {
                        bestLen = rootLen;
                        matchedMonitor = item.second;
                    }
                }
            }
        }

        if (matchedMonitor == nullptr)
        {
            MFS_LOG_WARNING("No monitor found for path [%s] when refreshing !", absPath.c_str());
            return T3D_ERR_NOT_FOUND;
        }

        return matchedMonitor->refresh(absPath);
    }

    //--------------------------------------------------------------------------
}

