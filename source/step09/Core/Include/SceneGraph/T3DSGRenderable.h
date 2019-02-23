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


#ifndef __T3D_SG_RENDERABLE_H__
#define __T3D_SG_RENDERABLE_H__


#include "SceneGraph/T3DSGTransform3D.h"
#include "Render/T3DRenderer.h"


namespace Tiny3D
{
    /**
     * @brief 场景中 3D 可渲染对象基类，用于渲染 3D 物体
     */
    class T3D_ENGINE_API SGRenderable : public SGTransform3D
    {
    public:
        /**
         * @brief 析构函数
         */
        virtual ~SGRenderable();

        /**
         * @brief 是否使用VAO数据
         * @return 返回true表示使用VAO数据，false表示使用VBO数据。
         * @remarks 
         *  当使用VAO数据时，以下接口必须返回有效值才能渲染：
         *      - VertexArrayPtr getVertexArrayObject() const
         *  当使用VBO数据时，以下接口必须返回有效值才能渲染：
         *      - Renderer::PrimitiveType getPrimitiveType() const
         *      - HardwareVertexBufferPtr getVertexBufferObject() const
         *      - bool isIndicesUsed() const
         *      - 如果 isIndicesUsed() 返回 true，则以下接口需要返回有效值：
         *          HardwareIndexBufferPtr getIndexBufferObject() 
         */
        virtual bool isVAOUsed() const = 0;

        /**
         * @brief 获取渲染图元
         * @return 返回渲染对象使用的渲染图元
         * @remarks 如果 isVAOUsed() 返回 true，则本接口必须返回有效值。
         * @see bool isVAOUsed() const
         */
        virtual Renderer::PrimitiveType getPrimitiveType() const = 0;

        /**
         * @brief 获取渲染使用的VBO对象
         * @return 返回渲染数据的VBO对象
         * @remarks 如果 isVAOUsed() 返回 true，则本接口必须返回有效值。
         * @see bool isVAOUsed() const
         */
        virtual HardwareVertexBufferPtr getVertexBufferObject() const = 0;

        /**
         * @brief 获取渲染使用的IBO对象
         * @return 返回渲染数据的IBO对象
         * @remarks 如果 isVAOUsed() 返回 true 并且 isIndicesUed() 也返回 true，
         *      则本接口必须返回有效值。
         * @see bool isVAOUsed() const
         */
        virtual HardwareIndexBufferPtr getIndexBufferObject() const = 0;

        /**
         * @brief 获取是否使用IBO数据
         * @remarks 当 isVAOUsed() 返回 true 时，渲染器才会调用本接口以决定是否
         *      使用 IBO 数据。
         * @see bool isVAOUsed() const
         */
        virtual bool isIndicesUsed() const = 0;

        /**
         * @brief 获取渲染使用的VAO对象
         * @return 返回渲染数据的VAO对象
         * @remarks 当 isVAOUsed() 返回 true 时，则本接口必须返回有效值。
         */
        virtual VertexArrayPtr getVertexArrayObject() const = 0;

    protected:
        /**
         * @brief 构造函数
         */
        SGRenderable(ID uID = E_NID_AUTOMATIC);
    };
}


#endif
