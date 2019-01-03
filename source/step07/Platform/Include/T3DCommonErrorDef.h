/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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

#ifndef __T3D_COMMON_ERROR_DEF_H__
#define __T3D_COMMON_ERROR_DEF_H__


#include "T3DMacro.h"


namespace Tiny3D
{
    /// 成功
    #define T3D_ERR_NONE                    0
    #define T3D_ERR_OK                      T3D_ERR_NONE
    #define T3D_ERR_SUCCESS                 T3D_ERR_NONE
    /// 通用错误
    #define T3D_ERR_FAIL                    1
    /// 空指针或者非法指针
    #define T3D_ERR_INVALID_POINTER         2
    /// 无效的定时器ID
    #define T3D_ERR_INVALID_TIMERID         3
};

#endif  /*__T3D_COMMON_ERROR_DEF_H__*/
