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


#ifndef __T3D_RENDER_WINDOW_DESC_H__
#define __T3D_RENDER_WINDOW_DESC_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Render/T3DMultisamplerDesc.h"


namespace Tiny3D
{
    TSTRUCT()
    struct T3D_ENGINE_API RenderWindowDesc
    {
        THandle externalHandle {nullptr};
        /**< 窗口横向坐标位置，全屏窗口忽略该参数 */
        TPROPERTY()
        int32_t Left;
        /**< 窗口纵向坐标位置，全屏窗口忽略该参数 */
        TPROPERTY()
        int32_t Top;
        /**< 窗口宽度 */
        TPROPERTY()
        int32_t Width;
        /**< 窗口高度 */
        TPROPERTY()
        int32_t Height;
        /**< 窗口色深 */
        TPROPERTY()
        int32_t ColorDepth;
        /**< 抗锯齿质量 */
        TPROPERTY()
        MultisamplerDesc    MSAA;
        /**< 窗口标题 */
        String  Title;
        String  IconPath;       /**< 图标文件路径 */
        bool    IsFullscreen;     /**< 是否全屏，终端平台忽略该参数 */
        bool    IsVsync;          /**< 是否开启垂直同步 */
    };
}


#endif  /*__T3D_RENDER_WINDOW_DESC_H__*/
