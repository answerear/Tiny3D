/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
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


#ifndef __T3D_SCENE_SPHERE_H__
#define __T3D_SCENE_SPHERE_H__

#include "Scene/T3DSceneRenderable.h"


namespace Tiny3D
{
    /**
     * @brief 可渲染球体
     */
    class T3D_ENGINE_API SceneSphere : public SceneRenderable
    {
    public:
        /**
         * @brief 创建可渲染球体对象
         * @param [in] center : 球心
         * @param [in] radius : 球半径
         * @param [in] uID : 结点ID，默认自动生成
         * @return 返回一个可渲染球体对象
         */
        static SceneSpherePtr create(const Vector3 &center, Real radius, 
            ID uID = E_CID_AUTOMATIC);

        /**
         * @brief 析构函数
         */
        virtual ~SceneSphere();

        /**
         * @brief 重写基类接口，实现获取结点类型
         * @see Node::Type Node::getNodeType() const
         */
        virtual const String &getType() const override;

        /**
         * @brief 重写基类接口，实现克隆对象功能
         * @see NodePtr Node::clone() const
         */
        virtual ComponentPtr clone() const override;

        /**
         * @brief 获取球心
         */
        const Vector3 &getCenter() const { return mCenter; }

        /**
         * @brief 获取球半径
         */
        Real getRadius() const { return mRadius; }

    protected:
        /**
         * @brief 构造函数
         */
        SceneSphere(ID uID = E_CID_AUTOMATIC);

        /**
         * @brief 初始化对象
         * @param [in] center : 球心
         * @param [in] radius : 球半径
         * @return 成功返回 T3D_OK
         */
        virtual TResult init(const Vector3 &center, Real radius);

        /**
         * @brief 重写基类接口，实现克隆对象属性
         * @see TResult Node::cloneProperties(NodePtr node) const
         */
        virtual TResult cloneProperties(ComponentPtr newObj) const override;

        /**
         * @brief 重写基类接口，实现结点的自身变换
         * @see void SceneNode::updateTransform()
         */
        virtual void updateBound() override;

        /**
         * @brief 重写基类接口，实现结点的视锥体裁剪逻辑
         * @see void SceneNode::frustumCulling(BoundPtr bound, RenderQueuePtr queue)
         */
        virtual void frustumCulling(BoundPtr bound,
            RenderQueuePtr queue) override;

        /**
         * @brief 重写基类接口，获取渲染材质对象
         * @see MaterialPtr SceneRenderable::getMaterial() const
         */
        virtual MaterialPtr getMaterial() const override;

        /**
         * @brief 重写基类接口，获取渲染VAO数据对象
         * @see VertexArrayObjectPtr SceneRenderable::getVertexArrayObject() const
         */
        virtual VertexArrayObjectPtr getVertexArrayObject() const override;

    private:
        void setupSphere(void *vertices, size_t vertexCount, uint16_t *indices,
            size_t indexCount);

    protected:
        Vector3                 mCenter;    /**< 球心 */
        Real                    mRadius;    /**< 球半径 */

        VertexArrayObjectPtr    mVAO;       /**< 渲染用的VAO */
        SphereBoundPtr          mBound;     /**< 碰撞体 */

        MaterialPtr             mMaterial;      /**< 材质 */
    };
}


#endif  /*__T3D_SCENE_SPHERE_H__*/
