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


#ifndef __T3D_SG_AXIS_H__
#define __T3D_SG_AXIS_H__


#include "SceneGraph/T3DSGRenderable.h"


namespace Tiny3D
{
    /**
     * @brief 坐标轴场景渲染类型，用于渲染三个坐标轴
     */
    class T3D_ENGINE_API SGAxis : public SGRenderable
    {
    public:
        /**
         * @brief 坐标类型
         */
        enum Axis
        {
            AXIS_X = 0, /**< X轴 */
            AXIS_Y,     /**< Y轴 */
            AXIS_Z      /**< Z轴 */
        };

        /**
         * @brief 创建坐标轴场景渲染对象
         * @param [in] X : X轴的长度
         * @param [in] Y : Y轴的长度
         * @param [in] Z : Z轴的长度
         * @param [in] uID : 结点ID，默认自动生成
         * @return 返回一个坐标轴场景渲染对象
         */
        static SGAxisPtr create(Real X, Real Y, Real Z, 
            ID uID = E_NID_AUTOMATIC);

        /**
         * @brief 析构函数
         */
        virtual ~SGAxis();

        /**
         * @brief 重写基类接口，返回当前结点类型
         * @see Node::Type Node::getNodeType() const
         */
        virtual Type getNodeType() const override;

        /**
         * @brief 重写基类接口，实现当前结点的克隆功能
         * @see NodePr Node::clone() const
         */
        virtual NodePtr clone() const override;

        /**
         * @brief 获取坐标轴长度
         * @return 返回坐标轴长度
         * @see void setAxisLength(Axis axis, Real length)
         * @see enum Axis
         */
        Real getAxisLength(Axis axis) const;

        /**
         * @brief 设置坐标轴长度
         * @param [in] axis : 坐标轴类型
         * @param [in] length : 坐标轴长度
         * @return void
         * @see Real getAxisLength(Axis axis) const
         * @see enum Axis
         */
        void setAxisLength(Axis axis, Real length);

    protected:
        /**
         * @brief 构造函数
         */
        SGAxis(ID uID = E_NID_AUTOMATIC);

        /**
         * @brief 初始化
         * @param [in] X : X轴的长度
         * @param [in] Y : Y轴的长度
         * @param [in] Z : Z轴的长度
         * @return 成功返回 T3D_OK
         */
        virtual TResult init(Real X, Real Y, Real Z);

        /**
         * @brief 重写基类接口，实现当前结点的属性克隆功能
         * @see TResult Node::cloneProperties(NodePtr node)
         */
        virtual TResult cloneProperties(NodePtr node) const override;

        /**
         * @brief 重写基类接口，实现结点的自身变换
         */
        virtual void updateTransform() override;

        /**
         * @brief 重写基类接口，实现结点的视锥体裁剪逻辑
         * @see void SGNode::frustumCulling(BoundPtr bound, RenderQueuePtr queue)
         */
        virtual void frustumCulling(BoundPtr bound, 
            RenderQueuePtr queue) override;

        /**
         * @brief 重写基类接口，实现决定是否使用VAO
         * @see bool SGRenderable::isVAOUsed() const
         */
        virtual bool isVAOUsed() const override;

        /**
         * @brief 重写基类接口，实现获取渲染图元
         * @see Renderer::PrimitiveType SGRenderable::getPrimitiveType() const
         */
        virtual Renderer::PrimitiveType getPrimitiveType() const override;

        /**
         * @brief 重写基类接口，实现获取VBO对象
         * @see HardwareVertexBufferPtr SGRenderable::getVertexBufferObject() const
         */
        virtual HardwareVertexBufferPtr getVertexBufferObject() const override;

        /**
         * @brief 重写基类接口，实现获取IBO对象
         * @see HardwareIndexBufferPtr SGRenderable::getIndexBufferObject() const
         */
        virtual HardwareIndexBufferPtr getIndexBufferObject() const override;

        /**
         * @brief 重写基类接口，实现决定是否使用IBO
         * @see bool SGRenderable::isIndicesUsed() const
         */
        virtual bool isIndicesUsed() const override;

        /**
         * @brief 重写基类接口，实现获取VAO对象
         * @see VertexArrayPtr SGRenderable::getVertexArrrayObject() const
         */
        virtual VertexArrayPtr getVertexArrayObject() const override;

        /**
         * @brief 更新顶点数据
         */
        void updateVertices();

    private:
        /**
         * @brief 顶点数据格式，只需要位置和颜色
         */
        struct Vertex
        {
            Vertex()
                : position(Vector3::ZERO)
                , color(Color4::WHITE)
            {}

            Vector3     position;
            Color4      color;
        };

        Vertex                  mVertices[6];       /**< 顶点数据 */
        bool                    mIsVertexDirty;     /**< 顶点数据更新标记 */

        HardwareVertexBufferPtr mVBO;               /**< VBO 数据对象 */
    };
}


#endif  /*__T3D_SG_AXIS_H__*/
