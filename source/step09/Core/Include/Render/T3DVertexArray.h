/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Agent)
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


#ifndef __T3D_VERTEX_ARRAY_H__
#define __T3D_VERTEX_ARRAY_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Render/T3DRenderer.h"


namespace Tiny3D
{
    /**
     * @brief VAO，用于存储渲染操作
     * @remarks 具体渲染系统子类去实现里面的接口
     */
    class T3D_ENGINE_API VertexArray : public Object
    {
    public:
        T3D_DECLARE_INTERFACE(VertexArray);

        /**
         * @brief 开始绑定VAO
         */
        virtual TResult beginBinding() = 0;

        /**
         * @brief 结束绑定VAO
         */
        virtual TResult endBinding() = 0;

        /**
         * @brief 设置顶点声明
         * @param [in] decl : 顶点声明对象
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult setVertexDeclaration(VertexDeclarationPtr decl) = 0;

        /**
         * @brief 添加一个顶点缓冲
         * @param [in] vbo : 顶点缓冲对象
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult addVertexBuffer(HardwareVertexBufferPtr vbo) = 0;

        /**
         * @brief 设置索引缓冲
         * @param [in] ibo : 索引缓冲对象
         * @return 调用成功返回 T3D_OK
         */
        virtual TResult setIndexBuffer(HardwareIndexBufferPtr ibo) = 0;

        /**
         * @brief 设置绘制图元类型
         */
        virtual TResult setPrimitiveType(Renderer::PrimitiveType priType) = 0;
    };
}


#endif  /*__T3D_VERTEX_ARRAY_H__*/
