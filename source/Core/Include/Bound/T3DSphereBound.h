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


#ifndef __T3D_SPHERE_BOUND_H__
#define __T3D_SPHERE_BOUND_H__


#include "Bound/T3DBound.h"


namespace Tiny3D
{
    /**
     * @class   SphereBound
     * @brief   球体碰撞体
     */
    TCLASS()
    class T3D_ENGINE_API SphereBound : public Bound
    {
        TRTTI_ENABLE(Bound)
        TRTTI_FRIEND

    public:
        /**
         * @fn  static SphereBoundPtr SphereBound::create(SceneNode *node, 
         *      ID uID = E_BID_AUTOMATIC);
         * @brief   创建球体碰撞体对象
         * @param [in]  node    : 碰撞体所在的结点对象.
         * @param [in]  uID     (Optional) : 碰撞体ID.
         * @return  返回一个新的球体碰撞体对象.
         */
        static SphereBoundPtr create(ID uID = E_CID_AUTOMATIC);

        /**
         * @fn  virtual SphereBound::~SphereBound();
         * @brief   析构函数
         */
        virtual ~SphereBound() override;

        /**
         * @fn  virtual Type SphereBound::getType() const override;
         * @brief   实现基类接口
         * @return  The type.
         * @sa  Bound::Type Bound::getType() const
         */
        TFUNCTION()
        virtual Type getType() const override;

        /**
         * @fn  virtual BoundPtr SphereBound::clone() const override;
         * @brief   实现基类接口
         * @return  A copy of this object.
         */
        virtual ComponentPtr clone() const override;

        /**
         * @fn  void SphereBound::setParams(const Vector3 &center, Real radius);
         * @brief   设置球体参数
         * @param [in]  center  : 球心.
         * @param [in]  radius  : 球半径.
         */
        TFUNCTION()
        void setParams(const Vector3 &center, Real radius);

        TPROPERTY(RTTRFuncName="sphere", RTTRFuncType="getter", "Description"="Sphere")
        const Sphere &getSphere() const { return mSphere; }

        TPROPERTY(RTTRFuncName="originalSphere", RTTRFuncType="getter", "Description"="OriginalSphere")
        const Sphere& getOriginalSphere() const { return mOriginalSphere; }

        /**
         * @fn  virtual RenderablePtr SphereBound::getRenderable() override;
         * @brief   实现基类接口
         * @return  The renderable.
         * @sa  RenderablePtr Bound::getRenderable()
         */
        virtual Renderable *getRenderable() override;

        /**
         * @fn  virtual void SphereBound::updateBound(
         *      const Transform &xform) override;
         * @brief   实现基类接口
         * @param   xform   The transform.
         * @sa  void Bound::updateBound(const Transform &amp;xform)
         */
        virtual void update() override;

    protected:
        SphereBound(ID uID = SphereBound::E_CID_AUTOMATIC);

        virtual bool testSphere(const Sphere &sphere) const override;

        virtual bool testAabb(const Aabb &aabb) const override; 

        virtual bool testObb(const Obb &obb) const override;

        virtual bool testFrustum(const Frustum &frustum) const override;

        virtual TResult cloneProperties(Component *newObj) const override;

        TPROPERTY(RTTRFuncName="sphere", RTTRFuncType="getter", "Description"="Sphere")
        void setSphere(const Sphere &sphere) { mSphere = sphere; }

        TPROPERTY(RTTRFuncName="originalSphere", RTTRFuncType="getter", "Description"="OriginalSphere")
        void setOriginalSphere(const Sphere &sphere) { mOriginalSphere = sphere; }

    private:
        Sphere    mOriginalSphere;  /**< 原始球体 */
        Sphere    mSphere;          /**< 用于快速检测相交性的球体 */
        GlobePtr  mRenderable;      /**< 用于渲染碰撞体的可渲染对象 */
    };
}

#endif  /*__T3D_SPHERE_BOUND_H__*/
