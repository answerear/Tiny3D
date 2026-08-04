/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#ifndef __T3D_CONFIG_H__
#define __T3D_CONFIG_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    /** \brief Core 编译期容量与特性开关常量 */

    /** \brief 多渲染目标（MRT）数量上限 */
    #define T3D_MAX_MULTIPLE_RENDER_TARGETS     8

    /** \brief 同时光源数量上限 */
    #define T3D_MAX_SIMULTANEOUS_LIGHTS         8

#if !defined (T3D_ENABLE_RHI_THREAD)
    /** \brief 是否启用独立 RHI 提交线程；未定义时默认为 0（主线程同步提交） */
    #define T3D_ENABLE_RHI_THREAD               0
#endif

    /** \brief 坐标系手性：0 为左手系，非 0 为右手系（如 Camera::lookAt 的矩阵构造） */
    #define T3D_COORDINATION_RH                 0

    /** \brief 单顶点蒙皮权重与骨骼索引槽位数 */
    #define T3D_MAX_BLEND_BONES                 4

    /** \brief 骨骼蒙皮矩阵数组长度上限 */
    #define T3D_MAX_SKIN_BONES                  200

    /** \brief 单网格 UV 层（纹理坐标集）数量上限 */
    #define T3D_MAX_TEXTURE_LEVEL               16
}


#endif    /*__T3D_CONFIG_H__*/
