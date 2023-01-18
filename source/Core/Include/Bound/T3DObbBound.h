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


#ifndef __T3D_OBB_BOUND_H__
#define __T3D_OBB_BOUND_H__


#include "Bound/T3DBound.h"


namespace Tiny3D
{
    /**
     * @class   ObbBound
     * @brief   有向包围盒碰撞体
     */
    TCLASS()
    class T3D_ENGINE_API ObbBound : public Bound
    {
        TRTTI_ENABLE(Bound)
        TRTTI_FRIEND

    public:
        /**
         * @fn  static ObbBoundPtr ObbBound::create(SceneNode *node, 
         *      ID uID = E_BID_AUTOMATIC);
         * @brief   创建OBB碰撞体对象
         * @param [in]  node    : 碰撞体所在的结点对象.
         * @param [in]  uID     (Optional) : 碰撞体ID.
         * @return  返回一个OBB碰撞体对象.
         */
        static ObbBoundPtr create(ID uID = E_CID_AUTOMATIC);

        /**
         * @fn  virtual ObbBound::~ObbBound();
         * @brief   析构函数
         */
        virtual ~ObbBound() override;

        /**
         * @fn  virtual Type ObbBound::getType() const override;
         * @brief   实现基类接口
         * @return  The type.
         * @sa  Bound::Type Bound::getType() const
         */
        TFUNCTION("Description"="Bound Type")
        virtual Type getType() const override;

        /**
         * @fn  virtual BoundPtr ObbBound::clone() const override;
         * @brief   实现基类接口
         * @return  A copy of this object.
         * @sa  BoundPtr Bound::clone() const
         */
        virtual ComponentPtr clone() const override;

        /**
         * @fn  const Obb ObbBound::&getObb() const
         * @brief   获取有向包围盒对象
         * @return  The obb.
         */
        TPROPERTY(RTTRFuncName="OBB", RTTRFuncType="getter", "Description"="Oriented Bounding Box")
        const Obb &getObb() const
        {
            return mObb;
        }

        /**
         * @fn  const Obb ObbBound::&getObb() const
         * @brief   获取未参与变换的原始有向包围盒对象
         * @return  The obb.
         */
        TPROPERTY(RTTRFuncName="originOBB", RTTRFuncType="getter", "Description"="Original Oriented Bounding Box")
        const Obb& getOriginalObb() const
        {
            return mOriginalObb;
        }

        /**
         * @fn  void ObbBound::setCenter(const Vector3 &center);
         * @brief   设置碰撞体中心
         * @param   center  The center.
         */
        TFUNCTION()
        void setCenter(const Vector3 &center);

        /**
         * @fn  void ObbBound::setAxis(const Vector3 &axis0, 
         *      const Vector3 &axis1, const Vector3 &axis2);
         * @brief   设置碰撞体的三个轴
         * @param   axis0   The axis 0.
         * @param   axis1   The first axis.
         * @param   axis2   The second axis.
         */
        TFUNCTION()
        void setAxis(const Vector3 &axis0, const Vector3 &axis1,
            const Vector3 &axis2);

        /**
         * @fn  void ObbBound::setExtent(Real extent0, Real extent1, 
         *      Real extent2);
         * @brief   设置碰撞体在三个轴上的延伸长度
         * @param   extent0 The extent 0.
         * @param   extent1 The first extent.
         * @param   extent2 The second extent.
         */
        TFUNCTION()
        void setExtent(Real extent0, Real extent1, Real extent2);

        /**
         * @fn  virtual RenderablePtr ObbBound::getRenderable() override;
         * @brief   实现基类接口
         * @return  The renderable.
         * @sa  RenderablePtr Bound::getRenderable()
         */
        virtual Renderable *getRenderable() override;

        /**
         * @fn  virtual void ObbBound::updateBound(
         *      const Transform &xform) override;
         * @brief   实现基类接口
         * @param   xform   The transform.
         * @sa  void Bound::updateBound(const Transform &amp;xform)
         */
        virtual void update() override;

    protected:
        ObbBound(ID uID = ObbBound::E_CID_AUTOMATIC);

        virtual bool testSphere(const Sphere &sphere) const override;

        virtual bool testAabb(const Aabb &aabb) const override;

        virtual bool testObb(const Obb &obb) const override;

        virtual bool testFrustum(const Frustum &frustum) const override;

        virtual TResult cloneProperties(Component *newObj) const override;

        TPROPERTY(RTTRFuncName="OBB", RTTRFuncType="setter", "Description"="Oriented Bounding Box")
        void setAlignAxisBox(const Obb &obb)
        {
            mObb = obb;
        }

        TPROPERTY(RTTRFuncName="originalOBB", RTTRFuncType="setter", "Description"="Original Oriented Bounding Box")
        void setOriginalAlignAxisBox(const Obb &obb)
        {
            mOriginalObb = obb;
        }
        
    protected:
        Obb         mObb;           /**< 实时变换的OBB */
        Obb         mOriginalObb;   /**< 不参与变换的原始OBB */
        CubePtr     mRenderable;    /**< 用于渲染碰撞体的可渲染对象 */
    };
}


#endif  /*__T3D_OBB_BOUND_H__*/
