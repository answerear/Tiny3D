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


#ifndef __T3D_RENDERER_H__
#define __T3D_RENDERER_H__


#include "T3DPrerequisites.h"
#include "Kernel/T3DObject.h"


namespace Tiny3D
{
    /**
     * @brief 渲染器抽象类，负责提供抽象渲染接口，具体渲染器实现这些接口
     */
    class T3D_ENGINE_API Renderer : public Object
    {
    public:
        static const char * const T3DXRENDERER; /**< Tiny3D自带的软渲染器 */
        static const char * const DIRECT3D9;    /**< Direct3D9 渲染器 */
        static const char * const DIRECT3D11;   /**< Direct3D11 渲染器 */
        static const char * const OPENGL3;      /**< OpenGL 3.x 渲染器 */
        static const char * const OPENGLES2;    /**< OpenGL ES 2 渲染器 */
        static const char * const OPENGLES3;    /**< OpenGL ES 3 渲染器 */
        static const char * const VULKAN;       /**< Vulkan 渲染器 */
        static const char * const METAL;        /**< Metal 渲染器 */

        /**
         * @brief 渲染器能力值
         */
        enum Capability
        {

        };

        /**
         * @breif 设置变换矩阵状态
         */
        enum TransformState
        {
            E_TS_VIEW = 0,      /**< 视口变换矩阵 */
            E_TS_WORLD,         /**< 世界变换矩阵 */
            E_TS_PROJECTION,    /**< 投影变换矩阵 */
            E_TS_MAX
        };

        /**
         * @brief 渲染图元类型
         */
        enum PrimitiveType
        {
            E_PT_POINT_LIST = 0,    /**< 点列表图元 */
            E_PT_LINE_LIST,         /**< 线列表图元 */
            E_PT_LINE_STRIP,        /**< 线带图元 */
            E_PT_TRIANGLE_LIST,     /**< 三角形列表图元 */
            E_PT_TRIANGLE_STRIP,    /**< 三角形带图元 */
            E_PT_TRIANGLE_FAN,      /**< 三角形扇形图元 */ 
        };

        /**
         * @brief 背面剔除模式
         */
        enum CullingMode
        {
            E_CULL_NONE = 0,        /**< 不做消隐面剔除 */
            E_CULL_CLOCKWISE,       /**< 按照顶点顺时针顺序的消隐面剔除 */
            E_CULL_ANTICLOCKWISE,   /**< 按照顶点逆时针顺序的消隐面剔除 */
        };

        /**
         * @brief 渲染模式
         */
        enum RenderMode
        {
            E_RM_POINT = 0,         /**< 顶点模式 */
            E_RM_WIREFRAME,         /**< 线框模式 */
            E_RM_SOLID,             /**< 着色模式 */
        };

        /**
         * @brief 构造函数
         */
        Renderer();

        /**
         * @brief 析构函数
         */
        virtual ~Renderer();

        /**
         * @brief 初始化渲染器
         * @return 调用成功返回 T3D_ERR_OK
         */
        virtual TResult init() = 0;

        /**
         * @brief 销毁渲染器
         * @return 调用成功返回 T3D_ERR_OK
         */
        virtual TResult destroy() = 0;

        /**
         * @brief 获取渲染器名称
         */
        virtual String getName() const = 0;

        /**
         * @brief 渲染一帧
         * @return 调用成功返回 T3D_ERR_OK
         */
        virtual TResult render() = 0;
    };
}


#endif  /*__T3D_RENDERER_H__*/
