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


namespace Tiny3D
{
    #define TAG_BUNDLE_BUILDER      "BundleBuilder"

    #define BB_LOG_ERROR(fmt, ...)   \
        T3D_LOG_ERROR(TAG_BUNDLE_BUILDER, fmt, ##__VA_ARGS__);  \
        printf(fmt, ##__VA_ARGS__); \
        printf("\n");

    #define BB_LOG_WARNING(fmt, ...) \
        T3D_LOG_WARNING(TAG_BUNDLE_BUILDER, fmt, ##__VA_ARGS__);    \
        printf(fmt, ##__VA_ARGS__); \
        printf("\n");

    #define BB_LOG_INFO(fmt, ...)    \
        T3D_LOG_INFO(TAG_BUNDLE_BUILDER, fmt, ##__VA_ARGS__);   \
        printf(fmt, ##__VA_ARGS__); \
        printf("\n");

    /// 与 BundleManifest 一致的清单魔数版本头
    #define BUNDLE_MANIFEST_MAGIC       "T3D_BUNDLE_MANIFEST 1"
    /// 清单文件名
    #define BUNDLE_MANIFEST_NAME        "bundle.manifest"

    #define ARCHIVE_TYPE_METAFS         "MetaFileSystem"
    #define ARCHIVE_TYPE_FS             "FileSystem"
}
