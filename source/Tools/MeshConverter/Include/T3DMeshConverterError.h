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


#include "T3DConverterPrerequisites.h"


namespace Tiny3D
{
    enum MConvErrorCode : TResult
    {
        T3D_ERR_MCONV_BASE = T3D_ERR_USER,

        /// FBX SDK 初始化失败
        T3D_ERR_FBX_SDK_CREATE_FAILED,
        /// 加载 FBX 插件 DLL 失败
        T3D_ERR_FBX_PLUGIN_LOAD_FAILED,
        /// 创建 FBX 场景失败
        T3D_ERR_FBX_SCENE_CREATE_FAILED,
        /// FBX 导入器初始化失败
        T3D_ERR_FBX_IMPORTER_INIT_FAILED,
        //// FBX 场景导入失败
        T3D_ERR_FBX_SCENE_IMPORT_FAILED,
    };
}

