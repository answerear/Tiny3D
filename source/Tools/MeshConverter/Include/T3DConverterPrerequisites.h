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
#include <fbxsdk.h>


namespace Tiny3D
{
    #define TAG_MESH_CONVERTER      "MeshConverter"

    #define MCONV_LOG_ERROR(fmt, ...)   \
        T3D_LOG_ERROR(TAG_MESH_CONVERTER, fmt, ##__VA_ARGS__);  \
        printf(fmt, ##__VA_ARGS__); \
        printf("\n");

    #define MCONV_LOG_WARNING(fmt, ...) \
        T3D_LOG_WARNING(TAG_MESH_CONVERTER, fmt, ##__VA_ARGS__);    \
        printf(fmt, ##__VA_ARGS__); \
        printf("\n");

    #define MCONV_LOG_INFO(fmt, ...)    \
        T3D_LOG_INFO(TAG_MESH_CONVERTER, fmt, ##__VA_ARGS__);   \
        printf(fmt, ##__VA_ARGS__); \
        printf("\n");

    #define MCONV_LOG_DEBUG(fmt, ...)   \
        T3D_LOG_DEBUG(TAG_MESH_CONVERTER, fmt, ##__VA_ARGS__);  \
        printf(fmt, ##__VA_ARGS__); \
        printf("\n");

    enum MeshFileType : uint32_t
    {
        /// 根据扩展名自动判断文件格式
        kAuto = 0,
        /// FBX 格式
        kFbx = 0x10,
        /// OGRE 格式
        kOgre = 0x20,
        /// Tiny3D Static Mesh 格式
        kTMesh = 0x40,
        /// Tiny3D Skinned Mesh 格式
        kTSkin = 0x80,
        /// Tiny3D Skeleton 格式
        kTSkel = 0x100,
        /// Tiny3D Animation 格式
        kTAni = 0x200,
        /// Tiny3D mesh 格式
        kTiny3D = kTSkin | kTSkel | kTAni,
    };

    enum class BoundType : uint32_t
    {
        kSphere = 0,
        kAabb
    };

    #define MCONV_VERSION       0x00001000
    #define MCONV_VERSION_STR   "0.0.1"

    using Assets = TUnorderedMap<String, Resource*>;
}

