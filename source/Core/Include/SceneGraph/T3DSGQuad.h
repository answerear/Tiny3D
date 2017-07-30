/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
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
    class T3D_ENGINE_API SGQuad : public SGRenderable
    {
    public:
        static SGQuadPtr create(uint32_t uID = E_NID_AUTOMATIC);

        virtual ~SGQuad();

        virtual Type getNodeType() const override;

    protected:
        SGQuad(uint32_t uID = E_NID_AUTOMATIC);

        virtual void frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue) override;

        virtual NodePtr clone() const override;

        virtual void cloneProperties(const NodePtr &node) const override;

        /**
        * @brief 返回渲染材质对象。
        * @note 所有可渲染派生类需要重写本接口以返回对应的渲染材质对象。
        */
        virtual MaterialPtr getMaterial() const override;

        /**
        * @brief 返回渲染元类型
        * @see Renderer::PrimitiveType
        */
        virtual Renderer::PrimitiveType getPrimitiveType() const override;

        /**
        * @brief 返回顶点数据对象
        */
        virtual VertexDataPtr getVertexData() const override;

        /**
        * @brief 返回索引数据对象
        */
        virtual IndexDataPtr getIndexData() const override;

        /**
        * @brief 是否使用顶点索引
        */
        virtual bool isIndicesUsed() const override;

    protected:
        VertexDataPtr   mVertexData;
    };
}


#endif  /*__T3D_SG_QUAD_H__*/
