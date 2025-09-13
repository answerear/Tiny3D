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
    struct ConverterOptions : Allocator
    {
        /// 输入文件全路径
        String  srcPath {};
        /// 输出文件夹路径
        String  dstDir {};
        /// 输出文件名
        String  dstTitle {};
        /// 附加文件全路径，在 ogre 模型时是指 material 文件
        String  extraPath {};

        /// 源文件类型
        MeshFileType    srcFileType {MeshFileType::kAuto};
        /// 目标文件类型
        MeshFileType    dstFileType {MeshFileType::kAuto};
        /// 包围盒类型
        BoundType       boundType {BoundType::kAabb};

        /// 输出格式是文本还是二进制，只对 Tiny3D 格式有效
        bool    isTxt {false};
        /// 是否有顶点共享
        bool    hasVertexShared {false};
        /// 是否输出详细信息
        bool    verbose {true};
    };
}
