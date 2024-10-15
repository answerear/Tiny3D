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
    class MetaFSMonitor : public Object
    {
    public:
        using PathMappings = TUnorderedMap<String, MetaPtr>;
        using UUIDMappings = TUnorderedMap<UUID, String, UUIDHash, UUIDEqual>;
        
        static MetaFSMonitorPtr create(const String &rootPath);

        ~MetaFSMonitor() override;

        TResult init();

        TResult start();
        
        TResult stop();

        const String &getRootPath() const { return mRootPath; }

        Meta *getMeta(const String &path) const
        {
            const auto &it = mPathMappings.find(path);
            return it != mPathMappings.end() ? it->second : nullptr;
        }
        
        Meta *getMeta(const UUID &uuid) const
        {
            Meta *meta = nullptr;
            const auto &it = mUUIDMappings.find(uuid);
            if (it != mUUIDMappings.end())
            {
                meta = getMeta(it->second);
            }
            return meta;
        }

        const String &getPath(const UUID &uuid) const
        {
            const auto &it = mUUIDMappings.find(uuid);
            if (it == mUUIDMappings.end())
            {
                return EMPTY;
            }
            return it->second;
        }

        bool getPathAndMeta(const UUID &uuid, String &path, Meta *&meta) const
        {
            path = getPath(uuid);
            if (!path.empty())
            {
                meta = getMeta(path);
            }
            return !path.empty() && meta != nullptr;
        }
        
    protected:
        MetaFSMonitor(const String &rootPath);

        void generateMeta(const String &path);

        void generateFileMeta(const String &path);

        void generateFolderMeta(const String &path);

        MetaPtr generateFileMeta(const String &metaPath, const String &path, const String &title, const String &ext);

        void removeFileMeta(const String &path);

        void onFileChanged(const String &path, FSMonitorAction action);

        void onFileAdded(const String &path);

        void onFileRemoved(const String &path);

        void addPathLUT(const String &path, const MetaPtr &meta);

        void removePathLUT(const String &path);
        
    protected:
        static String EMPTY;
        
        /// 文件系统监控对象
        FileSystemMonitor *mFSMonitor {nullptr};
        /// 监控的根目录
        String mRootPath {};
        /// Path => Meta 映射
        PathMappings mPathMappings {};
        /// UUID => Path 映射
        UUIDMappings mUUIDMappings {};
    };
}

