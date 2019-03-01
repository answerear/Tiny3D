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
        /** 顶点位置 */
        enum VertexIndex
        {
            VI_TOP_LEFT = 0,
            VI_TOP_RIGHT,
            VI_BOTTOM_LEFT,
            VI_BOTTOM_RIGHT,
            VI_MAX_VERTICES
        };

        /**
         * @brief 顶点信息
         */
        struct Vertex
        {
            Vector3     position;   /**< 顶点位置 */
            Vector2     uv;         /**< UV坐标 */
        };

        struct Quad
        {
            Vertex  vertices[VI_MAX_VERTICES];
        };

        /**
         * @brief 创建可渲染空间四边形对象
         * @param [in] quad : 空间四边形数据
         * @param [in] materialName : 材质名称
         * @param [in] uID : 结点ID，默认自动生成
         * @return 返回一个可渲染空间四边形对象
         */
        static SGQuadPtr create(const Quad &quad, const String &materialName,
            ID uID = E_NID_AUTOMATIC);

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

        /**
         * @brief 设置空间四边形四个顶点的本地坐标
         */
        void setVertexPos(size_t idx, const Vector3 &pos)
        {
            T3D_ASSERT(idx < VI_MAX_VERTICES);
            mQuad.vertices[idx].position = pos;
        }

        /**
         * @brief 获取空间四边形四个顶点的本地坐标
         */
        const Vector3 &getVertexPos(size_t idx) const
        {
            T3D_ASSERT(idx < VI_MAX_VERTICES);
            return mQuad.vertices[idx].position;
        }

        /**
         * @brief 设置空间四边形四个顶点的UV坐标
         */
        void setVertexUV(size_t idx, const Vector2 &uv)
        {
            T3D_ASSERT(idx < VI_MAX_VERTICES);
            mQuad.vertices[idx].uv = uv;
        }

        /**
         * @brief 获取空间四边形四个顶点的UV坐标
         */
        const Vector2 &getVertexUV(size_t idx) const
        {
            T3D_ASSERT(idx < VI_MAX_VERTICES);
            return mQuad.vertices[idx].uv;
        }

    protected:
        /**
         * @brief 构造函数
         */
        SGQuad(ID uID = E_NID_AUTOMATIC);
            
        /**
         * @brief 初始化对象
         * @param [in] quad : 空间四边形
         * @param [in] materialName : 材质名称
         * @return 成功返回 T3D_OK
         */
        virtual TResult init(const Quad &quad, const String &materialName);

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

    protected:
        Quad    mQuad;                          /**< 原始数据 */
        Vector3 mWorldQuad[VI_MAX_VERTICES];    /**< 变换成世界坐标的顶点 */

        MaterialPtr             mMaterial;      /**< 渲染用的材质 */
        VertexArrayObjectPtr    mVAO;           /**< 渲染用的VAO */
    };
}


#endif  /*__T3D_SG_QUAD_H__*/
