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
// #include "T3DMetaFSProjectManager.h"


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
        // MFSProjectManager *projectMgr = new MFSProjectManager();
        // T3D_AGENT.getEditor()->setProjectManager(projectMgr);
        
        return T3D_ARCHIVE_MGR.addArchiveCreator(
            MetaFSArchive::ARCHIVE_TYPE,
            [this](const String &name, Archive::AccessMode mode)
            {
                MetaFSArchivePtr archive;

                bool shouldMonitor = false;
                
                if (archiveCanWrite((uint32_t)mode))
                {
                    // 找出是否已经有启动监控的，子文件夹都在监控中，不重复监控
                    for (const auto &item : mRootArchives)
                    {
                        // 路径不完全相同，并且父路径不在监控中
                        if (item.first != name && !StringUtil::match(name, item.first))
                        {
                            shouldMonitor = true;
                            break;
                        }
                    }
                }
                
                archive = MetaFSArchive::create(name, mode);
                if (archive != nullptr && shouldMonitor)
                {
                    // 启动监控
                    if (!archive->startMonitor())
                    {
                        // 启动失败
                        archive = nullptr;
                    }
                    else
                    {
                        // 启动成功过
                        mRootArchives.emplace(name, archive);
                    }
                }
                
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

    void MetaFSArchivePlugin::update()
    {
        for (const auto &item : mRootArchives)
        {
            item.second->update();
        }
    }

    //--------------------------------------------------------------------------

    TResult MetaFSArchivePlugin::shutdown()
    {
        TResult ret = T3D_OK;

        for (const auto &item : mRootArchives)
        {
            item.second->stopMonitor();
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MetaFSArchivePlugin::uninstall()
    {
        // MFSProjectManager *projectMgr = static_cast<MFSProjectManager *>(T3D_AGENT.getEditor()->getProjectManager());
        // T3D_SAFE_DELETE(projectMgr);
        // T3D_AGENT.getEditor()->setProjectManager(nullptr);
        
        return T3D_ARCHIVE_MGR.removeArchiveCreator(MetaFSArchive::ARCHIVE_TYPE);
    }
}

