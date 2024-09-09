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
    using UUIDMap = TUnorderedMap<UUID, String, UUIDHash, UUIDEqual>;
    using PathMap = TUnorderedMap<String, UUID>;
    
    TSTRUCT()
    struct Mappings
    {
        /// UUID => 文件路径映射表
        UUIDMap uuids {};
        
        /// 文件路径 => UUID 映射表
        TPROPERTY()
        PathMap paths {};
    };
    
    class UUIDManager : public Singleton<UUIDManager>
    {
    public:
        virtual ~UUIDManager() = default;

        TResult startup(const String &path);

        TResult shutdown();

        void refresh(const UUID &uuid, const String &path);

        const String &getPath(const UUID &uuid) const;

        const UUID &getUUID(const String &path) const;
        
    protected:
        /// 扫描对应路径的文件，建立映射关系
        TResult buildMappings();

        /// 读映射文件
        TResult readMappings();

        /// 写映射文件
        TResult writeMappings();
        
        /// 工程根目录
        String mProjectPath {};
        /// 映射表
        Mappings mMappings {};
        /// 是否刷新而写文件
        bool mIsDirty {false};

        static const char *MAPPINGS_FILE_NAME;
    };
}

