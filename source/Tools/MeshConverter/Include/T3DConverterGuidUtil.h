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
    /**
     * \brief MeshConverter 模块内部辅助类，用于在重新转换时保留已有 guid
     */
    class ConverterGuidUtil
    {
    public:
        /**
         * \brief 读取目标目录下已存在的 .meta 文件里记录的 UUID
         * \param [in] dir : meta 文件所在目录
         * \param [in] metaFilename : meta 文件名（如 "tortoise.tmesh.meta"）
         * \return 若 meta 存在且解析成功，返回其记录的 UUID；否则返回 UUID::INVALID
         */
        static UUID readExistingMetaUUID(const String &dir, const String &metaFilename);
    };
}
