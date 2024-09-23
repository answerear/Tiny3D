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


#include "T3DMetaFSArchivePlugin.h"
#include "T3DMetaFSArchive.h"
#include "T3DMetaFSMonitor.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    MetaFSArchivePlugin::MetaFSArchivePlugin()
        : mName("MetaFSArchive")
    {

    }

    //--------------------------------------------------------------------------

    MetaFSArchivePlugin::~MetaFSArchivePlugin()
    {

    }

    //--------------------------------------------------------------------------

    const String &MetaFSArchivePlugin::getName() const
    {
        return mName;
    }

    //--------------------------------------------------------------------------

    TResult MetaFSArchivePlugin::install()
    {
        return T3D_ARCHIVE_MGR.addArchiveCreator(
            MetaFSArchive::ARCHIVE_TYPE,
            [this](const String &name, Archive::AccessMode mode)
            {
                MetaFSArchivePtr archive;

                do
                {
                    MetaFSMonitorPtr monitor;
                
                    // 找出是否已经有启动监控的，子文件夹都在监控中，不重复监控
                    for (const auto &item : mPathMonitors)
                    {
                        if (item.first == name)
                        {
                            // 完全相同路径
                            monitor = item.second;
                            break;
                        }

                        // String::size_type pos = name.find_first_not_of(item.first);
                        bool match = StringUtil::match(name, item.first, false);
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
                        monitor = MetaFSMonitor::create(name);
                        TResult ret = monitor->init();
                        if (T3D_FAILED(ret))
                        {
                            MFS_LOG_ERROR("Failed to initialize monitor %s.", name.c_str());
                            break;
                        }

                        mPathMonitors.emplace(name, monitor);
                    }
                    
                    archive = MetaFSArchive::create(name, mode, monitor);
                } while (false);
                
                return archive;
            });
    }

    //--------------------------------------------------------------------------

    TResult MetaFSArchivePlugin::startup()
    {
        TResult ret = T3D_OK;
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MetaFSArchivePlugin::shutdown()
    {
        TResult ret = T3D_OK;

        for (const auto &item : mPathMonitors)
        {
            item.second->stop();
        }

        for (const auto &archive : mArchives)
        {
            T3D_ARCHIVE_MGR.unloadArchive(archive);
        }

        mArchives.clear();
        mPathMonitors.clear();
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MetaFSArchivePlugin::uninstall()
    {
        return T3D_ARCHIVE_MGR.removeArchiveCreator(MetaFSArchive::ARCHIVE_TYPE);
    }
}

