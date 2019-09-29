/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#ifndef __T3D_BLEND_MODE_H__
#define __T3D_BLEND_MODE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * @brief 混合类型
     */
    enum BlendType
    {
        /// 使用纹理上的alpha值来做混合计算，相当于：
        /// C_result = C_src * A_src + C_dst * (1 - A_src)
        E_BT_TRANSPARENT_ALPHA,
        /// 使用纹理上的颜色值来做混合计算，相当于：
        /// C_result = C_src * C_src + C_dst * (1 - C_src)
        E_BT_TRANSPARENT_COLOR,
        /// 把纹理颜色值直接叠加到现在渲染目标中的颜色值，相当于：
        /// C_result = C_src + C_dst
        E_BT_ADD,
        /// 直接把纹理颜色值跟渲染目标中的颜色值相乘，相当于：
        /// C_result = C_src * C_dst
        E_BT_MODULATE,
        /// 默认方式，直接用纹理颜色代替渲染目标的颜色
        E_BT_REPLACE,
    };

    /**
     * @brief 混合因子
     */
    enum BlendFactor
    {
        E_BF_ONE,                     /// 1.0
        E_BF_ZERO,                    /// 0.0
        E_BF_DEST_COLOR,              /// C_dst
        E_BF_SOURCE_COLOR,            /// C_src
        E_BF_ONE_MINUS_DEST_COLOR,    /// 1 - C_dst
        E_BF_ONE_MINUS_SOURCE_COLOR,  /// 1 - C_src
        E_BF_DEST_ALPHA,              /// A_dst
        E_BF_SOURCE_ALPHA,            /// A_src
        E_BF_ONE_MINUS_DEST_ALPHA,    /// 1 - A_dst
        E_BF_ONE_MINUS_SOURCE_ALPHA   /// 1 - A_src
    };

    /**
     * @brief 混合操作
     */
    enum BlendOperation
    {
        E_BO_ADD,                 /// C_result = C_src * F_src + C_dst * F_dst
        E_BO_SUBTRACT,            /// C_result = C_src * F_src - C_dst * F_dst
        E_BO_REVERSE_SUBTRACT,    /// C_result = C_dst * F_dst - C_src * F_src
        E_BO_MIN,                 /// 
        E_BO_MAX
    };
}


#endif  /*__T3D_BLEND_MODE_H__*/
