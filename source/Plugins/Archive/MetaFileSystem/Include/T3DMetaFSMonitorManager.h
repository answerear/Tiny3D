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


#pragma once


#include "T3DMetaFSArchivePrerequisites.h"


namespace Tiny3D
{
    class MetaFSMonitorManager
        : public Allocator
        , public Singleton<MetaFSMonitorManager>
    {
    public:
        ~MetaFSMonitorManager() override;

        MetaFSMonitor *addMonitor(const String &path);

        TResult removeMonitor(const String &path);

        TResult removeAllMonitors();

        bool getPathAndMonitor(const UUID &uuid, MetaFSMonitor *mainMonitor, String &path, MetaFSMonitor *&monitor);

        MetaFSMonitor *getMonitor(const String &path) const
        {
            auto itr = mPathMonitors.find(path);
            return itr != mPathMonitors.end() ? itr->second : nullptr;
        }
        
    protected:
        using PathMonitors = TUnorderedMap<String, MetaFSMonitorPtr>;

        /// 根路径集合，用于自动检测是否生成 meta 文件
        PathMonitors mPathMonitors {};
    };

    #define T3D_MFS_MONITOR_MGR (MetaFSMonitorManager::getInstance())
}

