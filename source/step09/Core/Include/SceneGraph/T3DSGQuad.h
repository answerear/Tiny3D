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


#ifndef __T3D_SG_QUAD_H__
#define __T3D_SG_QUAD_H__


#include "SceneGraph/T3DSGRenderable.h"


namespace Tiny3D
{
    /**
     * @brief 可渲染四边形
     */
    class T3D_ENGINE_API SGQuad : public SGRenderable
    {
    public:
        /**
         * @brief 
         */
        static SGQuadPtr create(const Vector3 &V0, const Vector3 &V1,
            const Vector3 &V2, const Vector3 &V3, ID uID = E_NID_AUTOMATIC);

        /**
         * @brief 析构函数
         */
        virtual ~SGQuad();

        /**
         * @brief 重写基类接口，实现获取结点类型
         * @see Node::Type Node::getNodeType() const
         */
        virtual Type getNodeType() const override;

        /**
         * @brief 重写基类接口，实现克隆对象功能
         * @see NodePtr Node::clone() const
         */
        virtual NodePtr clone() const override;

    protected:
        SGQuad(ID uID = E_NID_AUTOMATIC);

        /**
         * @brief 重写基类接口，实现克隆对象属性
         * @see TResult Node::cloneProperties(NodePtr node) const
         */
        virtual TResult cloneProperties(NodePtr node) const override;

        /**
         * @brief 重写基类接口，实现结点的自身变换
         * @see void SGNode::updateTransform()
         */
        virtual void updateTransform() override;

        /**
         * @brief 重写基类接口，实现结点的视锥体裁剪逻辑
         * @see void SGNode::frustumCulling(BoundPtr bound, RenderQueuePtr queue)
         */
        virtual void frustumCulling(BoundPtr bound,
            RenderQueuePtr queue) override;

        /**
         * @brief 重写基类接口，获取渲染材质对象
         * @see MaterialPtr SGRenderable::getMaterial() const
         */
        virtual MaterialPtr getMaterial() const override;

        /**
         * @brief 重写基类接口，获取渲染VAO数据对象
         * @see VertexArrayObjectPtr SGRenderable::getVertexArrayObject() const
         */
        virtual VertexArrayObjectPtr getVertexArrayObject() const override;
    };
}


#endif  /*__T3D_SG_QUAD_H__*/
