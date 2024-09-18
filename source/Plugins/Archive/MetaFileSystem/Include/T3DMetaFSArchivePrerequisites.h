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


#include <Tiny3D.h>

#if defined METAFSARCHIVE_EXPORT
    #define T3D_METAFSARCHIVE_API        T3D_EXPORT_API
#else
    #define T3D_METAFSARCHIVE_API        T3D_IMPORT_API
#endif


namespace Tiny3D
{
    #define LOG_TAG_METAFS      "MetaFSArchive"

    #define MFS_LOG_ERROR(fmt, ...)   T3D_LOG_ERROR(LOG_TAG_METAFS, fmt, ##__VA_ARGS__)
    #define MFS_LOG_WARNING(fmt, ...) T3D_LOG_WARNING(LOG_TAG_METAFS, fmt, ##__VA_ARGS__)
    #define MFS_LOG_INFO(fmt, ...)    T3D_LOG_INFO(LOG_TAG_METAFS, fmt, ##__VA_ARGS__)
    #define MFS_LOG_DEBUG(fmt, ...)   T3D_LOG_DEBUG(LOG_TAG_METAFS, fmt, ##__VA_ARGS__)

    inline bool archiveCanWrite(uint32_t mode)
    {
        uint32_t writeMode = (uint32_t)Archive::AccessMode::kAppend | (uint32_t)Archive::AccessMode::kTruncate;
        return (mode & writeMode) != 0;
    }
    
    class MetaFSArchive;

    T3D_DECLARE_SMART_PTR(MetaFSArchive);
}

