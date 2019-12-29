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


#ifndef __T3D_FRUSTUM_BOUND_H__
#define __T3D_FRUSTUM_BOUND_H__


#include "Bound/T3DBound.h"


namespace Tiny3D
{
    class T3D_ENGINE_API FrustumBound : public Bound
    {
    public:
        /**
         * @brief 创建Frustum碰撞体对象
         * @param [in] node : 碰撞体所在的结点对象
         * @param [in] uID : 碰撞体ID
         * @return 返回一个Frustum碰撞体对象
         */
        static FrustumBoundPtr create(SceneNode *node, ID uID = E_BID_AUTOMATIC);

        /**
         * @brief 析构函数
         */
        virtual ~FrustumBound();

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
         * @brief 获取视锥体包围体对象
         */
        const Frustum &getFrustum() const
        {
            return mFrustum;
        }

        /**
         * @brief 设置视锥体面
         */
        void setFrustumFace(Frustum::Face face, const Plane &plane);

        /**
         * @brief 设置视锥体所有面
         */
        void setFrustumFaces(Plane *plane, size_t planeCount);

        /**
         * @brief 实现基类接口
         * @see SceneRenderablePtr Bound::getRenderable()
         */
        virtual SceneRenderablePtr getRenderable() override;

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
        FrustumBound(SceneNode *node, ID uID = E_BID_AUTOMATIC);

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
        Frustum     mFrustum;           /**< 可变换的视锥体对象 */
        Frustum     mOriginalFrustum;   /**< 不参与变换的原始视锥体 */
        SceneBoxPtr    mRenderable;        /**< 用于渲染碰撞体的可渲染对象 */
    };
}


#endif  /*__T3D_FRUSTUM_BOUND_H__*/
