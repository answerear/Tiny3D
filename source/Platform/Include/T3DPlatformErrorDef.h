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

#ifndef __T3D_PLATFORM_ERROR_DEF_H__
#define __T3D_PLATFORM_ERROR_DEF_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DCommonErrorDef.h"

namespace Tiny3D
{
    enum PlatformErrorCode
    {
        // Platform 模块错误码从 T3D_ERR_PLATFORM 开始

        /// 线程已经创建了
        T3D_ERR_THREAD_ALREADY_CREATED = T3D_ERR_PLATFORM,
        /// 创建系统线程失败
        T3D_ERR_THREAD_CREATED = T3D_ERR_PLATFORM + 1,
        /// 线程未创建
        T3D_ERR_THREAD_NOT_CREATED = T3D_ERR_PLATFORM + 2,
        /// 线程挂起失败
        T3D_ERR_THREAD_SUSPEND = T3D_ERR_PLATFORM + 3,
        /// 线程继续失败
        T3D_ERR_THREAD_RESUME = T3D_ERR_PLATFORM + 4,
        /// 实现对象没有创建
        T3D_ERR_IMPLEMENT_NOT_CREATED = T3D_ERR_PLATFORM + 5,
    };
};

#endif  /*__T3D_PLATFORM_ERROR_DEF_H__*/
