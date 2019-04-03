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


#ifndef __T3D_OBB_BOUND_H__
#define __T3D_OBB_BOUND_H__


#include "Bound/T3DBound.h"


namespace Tiny3D
{
    /**
     * @brief 有向包围盒碰撞体
     */
    class T3D_ENGINE_API ObbBound : public Bound
    {
    public:
        /**
         * @brief 创建OBB碰撞体对象
         * @param [in] node : 碰撞体所在的结点对象
         * @param [in] uID : 碰撞体ID
         * @return 返回一个OBB碰撞体对象
         */
        static ObbBoundPtr create(SGNode *node, ID uID = E_BID_AUTOMATIC);

        /**
         * @brief 析构函数
         */
        virtual ~ObbBound();

        /**
         * @brief 实现基类接口
         * @see Bound::Type Bound::getType() const
         */
        virtual Type getType() const override;

        /**
         * @brief 实现基类接口
         * @see BoundPtr Bound::clone() const
         */
        virtual BoundPtr clone() const override;

        /**
         * @brief 获取有向包围盒对象
         */
        const Obb &getObb() const
        {
            return mObb;
        }

        /**
         * @brief 设置碰撞体中心
         */
        void setCenter(const Vector3 &center);

        /**
         * @brief 设置碰撞体的三个轴
         */
        void setAxis(const Vector3 &axis0, const Vector3 &axis1,
            const Vector3 &axis2);

        /**
         * @brief 设置碰撞体在三个轴上的延伸长度
         */
        void setExtent(Real extent0, Real extent1, Real extent2);

        /**
         * @brief 实现基类接口
         * @see SGRenderablePtr Bound::getRenderable()
         */
        virtual SGRenderablePtr getRenderable() override;

        /**
         * @brief 实现基类接口
         * @see void Bound::updateBound(const Transform &xform)
         */
        virtual void updateBound(const Transform &xform) override;

    protected:
        /**
         * @brief 构造函数
         * @param [in] node : 碰撞体所在的结点对象
         * @param [in] uID : 碰撞体ID
         */
        ObbBound(SGNode *node, ID uID = E_BID_AUTOMATIC);

        /**
         * @brief 实现基类接口
         * @see bool Bound::testSphere(const Spher &sphere) const
         */
        virtual bool testSphere(const Sphere &sphere) const override;

        /**
         * @brief 实现基类接口
         * @see bool Bound::testAabb(const Aabb &aabb) const
         */
        virtual bool testAabb(const Aabb &aabb) const override;

        /**
         * @brief 实现基类接口
         * @see bool Bound::testObb(const Obb &obb) const
         */
        virtual bool testObb(const Obb &obb) const override;

        /**
         * @brief 实现基类接口
         * @see bool Bound::testFrustum(const Frustum &frustum) const
         */
        virtual bool testFrustum(const Frustum &frustum) const override;

        /**
         * @brief 实现基类接口
         * @see void Bound::cloneProperties(BoundPtr bound) const
         */
        virtual void cloneProperties(BoundPtr bound) const override;

    protected:
        Obb         mObb;           /**< 实时变换的OBB */
        Obb         mOriginalObb;   /**< 不参与变换的原始OBB */
        SGBoxPtr    mRenderable;    /**< 用于渲染碰撞体的可渲染对象 */
    };
}


#endif  /*__T3D_OBB_BOUND_H__*/
