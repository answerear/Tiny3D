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


#ifndef __T3D_SCENE_AXIS_H__
#define __T3D_SCENE_AXIS_H__


#include "Scene/T3DSceneRenderable.h"


namespace Tiny3D
{
    /**
     * @class   SceneAxis
     * @brief   坐标轴场景渲染类型，用于渲染三个坐标轴
     */
    class T3D_ENGINE_API SceneAxis : public SceneRenderable
    {
    public:
        /**
         * @enum    Axis
         * @brief   坐标类型
         */
        enum Axis
        {
            AXIS_X = 0, /**< X轴 */
            AXIS_Y,     /**< Y轴 */
            AXIS_Z      /**< Z轴 */
        };

        /**
         * @fn  static SceneAxisPtr SceneAxis::create(Real X, Real Y, Real Z, 
         *      ID uID = E_NID_AUTOMATIC);
         * @brief   创建坐标轴场景渲染对象
         * @param [in]  X   : X轴的长度.
         * @param [in]  Y   : Y轴的长度.
         * @param [in]  Z   : Z轴的长度.
         * @param [in]  uID (Optional) : 结点ID，默认自动生成.
         * @return  返回一个坐标轴场景渲染对象.
         */
        static SceneAxisPtr create(Real X, Real Y, Real Z, 
            ID uID = E_CID_AUTOMATIC);

        /**
         * @fn  virtual SceneAxis::~SceneAxis();
         * @brief   析构函数
         */
        virtual ~SceneAxis();

        /**
         * @fn  virtual Type SceneAxis::getNodeType() const override;
         * @brief   重写基类接口，返回当前结点类型
         * @return  The node type.
         * @sa  Node::Type Node::getNodeType() const
         */
        virtual const String &getType() const override;

        /**
         * @fn  virtual NodePtr SceneAxis::clone() const override;
         * @brief   重写基类接口，实现当前结点的克隆功能
         * @return  A copy of this object.
         * @sa  NodePr Node::clone() const
         */
        virtual ComponentPtr clone() const override;

        /**
         * @fn  Real SceneAxis::getAxisLength(Axis axis) const;
         * @brief   获取坐标轴长度
         * @param   axis    The axis.
         * @return  返回坐标轴长度.
         * @sa  void setAxisLength(Axis axis, Real length)
         * @sa  enum Axis
         */
        Real getAxisLength(Axis axis) const;

    protected:
        /**
         * @fn  SceneAxis::SceneAxis(ID uID = E_NID_AUTOMATIC);
         * @brief   构造函数
         * @param   uID (Optional) The identifier.
         */
        SceneAxis(ID uID = E_CID_AUTOMATIC);

        /**
         * @fn  virtual TResult SceneAxis::init(Real X, Real Y, Real Z);
         * @brief   初始化
         * @param [in]  X   : X轴的长度.
         * @param [in]  Y   : Y轴的长度.
         * @param [in]  Z   : Z轴的长度.
         * @return  成功返回 T3D_OK.
         */
        virtual TResult init(Real X, Real Y, Real Z);

        /**
         * @fn  virtual TResult SceneAxis::cloneProperties(NodePtr node) 
         *      const override;
         * @brief   重写基类接口，实现当前结点的属性克隆功能
         * @param   node    The node.
         * @return  A TResult.
         * @sa  TResult Node::cloneProperties(NodePtr node)
         */
        virtual TResult cloneProperties(ComponentPtr newObj) const override;

        /**
         * @fn  virtual void SceneAxis::updateTransform() override;
         * @brief   重写基类接口，实现结点的自身变换
         * @sa  void SceneNode::updateTransform()
         */
        virtual void updateBound() override;

        /**
         * @fn  virtual void SceneAxis::frustumCulling(BoundPtr bound, 
         *      RenderQueuePtr queue) override;
         * @brief   重写基类接口，实现结点的视锥体裁剪逻辑
         * @param   bound   The bound.
         * @param   queue   The queue.
         * @sa  void SceneNode::frustumCulling(BoundPtr bound, 
         *      RenderQueuePtr queue)
         */
        virtual void frustumCulling(BoundPtr bound, RenderQueuePtr queue) override;

        /**
         * @fn  virtual MaterialPtr SceneAxis::getMaterial() const override;
         * @brief   重写基类接口，实现获取渲染用的材质
         * @return  The material.
         * @sa  MaterialPtr SceneRenderable::getMaterial() const
         */
        virtual MaterialPtr getMaterial() const override;

        /**
         * @fn  virtual VertexArrayObjectPtr SceneAxis::getVertexArrayObject() 
         *      const override;
         * @brief   重写基类接口，实现获取VAO对象
         * @return  The vertex array object.
         * @sa  VertexArrayObjectPtr SceneRenderable::getVertexArrrayObject() const
         */
        virtual VertexArrayObjectPtr getVertexArrayObject() const override;

    private:
        Real                    mAxisX; /**< X轴长度 */
        Real                    mAxisY; /**< Y轴长度 */
        Real                    mAxisZ; /**< Z轴长度 */

        VertexArrayObjectPtr    mVAO;   /**< 渲染用的VAO */
        SphereBoundPtr          mBound; /**< 碰撞体 */
    };
}


#endif  /*__T3D_SCENE_AXIS_H__*/
