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


#ifndef __T3D_COMMON_H__
#define __T3D_COMMON_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * @brief 比较函数，用于深度缓冲测试和模板缓冲测试
     */
    enum CompareFunction
    {
        E_CMPF_ALWAYS_FAIL,
        E_CMPF_ALWAYS_PASS,
        E_CMPF_LESS,
        E_CMPF_LESS_EQUAL,
        E_CMPF_EQUAL,
        E_CMPF_NOT_EQUAL,
        E_CMPF_GREATER_EQUAL,
        E_CMPF_GREATER
    };

    enum TrackVertexColorType
    {
        E_TVCT_NONE     = 0x0,
        E_TVCT_AMBIENT  = 0x1,
        E_TVCT_DIFFUSE  = 0x2,
        E_TVCT_SPECULAR = 0x4,
        E_TVCT_EMISSIVE = 0x8
    };

    /**
     * @brief 背面剔除模式
     */
    enum CullingMode
    {
        E_CULL_NONE = 0,        /**< 不做消隐面剔除 */
        E_CULL_CLOCKWISE,       /**< 按照顶点顺时针顺序的消隐面剔除 */
        E_CULL_ANTICLOCKWISE,   /**< 按照顶点逆时针顺序的消隐面剔除 */
        E_CULL_MAX
    };

    enum ManualCullingMode
    {
        E_MANUAL_CULL_NONE,
        E_MANUAL_CULL_BACK,
        E_MANUAL_CULL_FRONT
    };

    enum ShadeMode
    {
        E_SM_FLAT,
        E_SM_GOURAUD,
        E_SM_PHONG
    };

    /**
     * @brief 多边形渲染模式
     */
    enum PolygonMode
    {
        E_PM_NONE,
        E_PM_POINT,             /**< 顶点模式 */
        E_PM_WIREFRAME,         /**< 线框模式 */
        E_PM_SOLID,             /**< 着色模式 */
        E_PM_MAX
    };

    enum IlluminationStage
    {
        E_IS_UNKNOWN,
        E_IS_AMBIENT,
        E_IS_PER_LIGHT,
        E_IS_DECAL
    };

    enum FogMode
    {
        E_FOG_NONE,
        E_FOG_EXP,
        E_FOG_EXP2,
        E_FOG_LINEAR
    };

    enum FilterType
    {
        E_FT_MIN,
        E_FT_MAG,
        E_FT_MIP
    };

    enum FilterOptions
    {
        E_FO_NONE,
        E_FO_POINT,
        E_FO_LINEAR,
        E_FO_ANISOTROPIC
    };

    enum WaveformType
    {
        E_WFT_SINE,
        E_WFT_TRIANGLE,
        E_WFT_SQUARE,
        E_WFT_SAWTOOTH,
        E_WFT_INVERSE_SAWTOOTH,
        E_WFT_PWM
    };
}


#endif  /*__T3D_COMMON_H__*/
