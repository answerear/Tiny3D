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


#ifndef __T3D_SG_BOX_H__
#define __T3D_SG_BOX_H__


#include "SceneGraph/T3DSGRenderable.h"


namespace Tiny3D
{
    /**
     * @brief 可渲染的方盒子结点
     */
    class T3D_ENGINE_API SGBox : public SGRenderable
    {
    public:
        /**
         * @brief 创建长方体渲染对象
         * @param [in] center : 长方体的中心点
         * @param [in] extent : 长方体沿轴的6个方向长度
         * @param [in] uID : 结点ID，默认自动生成
         * @return 返回新建的长方体渲染对象
         */
        static SGBoxPtr create(const Vector3 &center, const Vector3 &extent,
            ID uID = E_NID_AUTOMATIC);

        /**
         * @brief 析构函数
         */
        virtual ~SGBox();

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
         * @brief 获取长方体中心
         */
        const Vector3 &getCenter() const { return mCenter; }

        /**
         * @brief 获取三个轴方向上的长度
         */
        const Vector3 &getExtent() const { return mExtent; }

    protected:
        /**
         * @brief 构造函数
         */
        SGBox(ID uID = E_NID_AUTOMATIC);

        /**
         * @brief 初始化对象
         * @param [in] center : 长方体的中心点
         * @param [in] extent : 长方体沿轴的6个方向长度
         * @return 成功返回 T3D_OK
         */
        virtual TResult init(const Vector3 &center, const Vector3 &extent);

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

    private:
        void setupBox(void *vertices, size_t vertexCount,  uint16_t *indices, 
            size_t indexCount);

    protected:
        Vector3                 mCenter;        /**< 长方体的中心 */
        Vector3                 mExtent;        /**< 沿3个轴方向上的长度 */
        VertexArrayObjectPtr    mVAO;           /**< 渲染用的VAO */
        ObbBoundPtr             mBound;         /**< 有向碰撞体 */
        MaterialPtr             mMaterial;      /**< 材质 */
    };
}


#endif  /*__T3D_SG_BOX_H__*/
