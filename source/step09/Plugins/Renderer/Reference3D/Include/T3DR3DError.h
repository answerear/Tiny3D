/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#ifndef __T3D_R3D_ERROR_H__
#define __T3D_R3D_ERROR_H__


#include "T3DR3DPrerequisites.h"


namespace Tiny3D
{
    enum R3DErrorCode
    {
        T3D_ERR_R3D_WINDOW_ALREADY = T3D_ERR_RENDERER,  /**< 窗口已经创建 */
        T3D_ERR_R3D_UNSUPPORT_FORMAT_ICON,              /**< 不支持的图标文件格式 */
        T3D_ERR_R3D_IMAGE_SIZE_NOT_EQL,                 /**< 图片大小不一致 */
    };
}


#endif  /*__T3D_R3D_ERROR_H__*/
