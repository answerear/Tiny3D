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


#ifndef __T3D_BOUND_H__
#define __T3D_BOUND_H__


#include "T3DPrerequisites.h"
#include "Component/T3DComponent.h"


namespace Tiny3D
{
    /**
     * @class   Bound
     * @brief   碰撞体基类
     */
    TCLASS()
    class T3D_ENGINE_API Bound : public Component
    {
        TRTTI_ENABLE(Component)
        TRTTI_FRIEND

    public:
        /**
         * \brief 包围盒类型
         */
        TENUM()
        enum class Type : uint32_t
        {
            /// 未知碰撞体类型
            NONE = 0,
            /// 球包围体碰撞体
            SPHERE,
            /// 轴对齐包围盒碰撞体
            AABB,
            /// 有向包围盒碰撞体
            OBB,
            /// 视锥体碰撞体
            FRUSTUM,
        };

        ~Bound() override;

        TFUNCTION("Description"="Bound Type")
        virtual Type getType() const = 0;

        TPROPERTY(RTTRFuncName="groupID", RTTRFuncType="setter", "Description"="Group ID")
        void setGroupID(ID groupID) { mGroupID = groupID; }

        TPROPERTY(RTTRFuncName="groupID", RTTRFuncType="getter", "Description"="Group ID")
        ID getGroupID() const { return mGroupID; }

        TFUNCTION()
        virtual bool test(Bound *bound) const;

        TPROPERTY(RTTRFuncName="collisionSource", RTTRFuncType="setter", "Description"="Collision Source")
        void setCollisionSource(bool isSource) { mIsCollisionSource = isSource; }

        TPROPERTY(RTTRFuncName="collisionSource", RTTRFuncType="getter", "Description"="Collision Source")
        bool isCollisionSource() const { return mIsCollisionSource; }

        TPROPERTY(RTTRFuncName="enable", RTTRFuncType="setter", "Description"="Enable")
        void setEnabled(bool isEnabled) { mIsEnabled = isEnabled; }

        TPROPERTY(RTTRFuncName="enable", RTTRFuncType="getter", "Description"="Enable")
        bool isEnabled() const { return mIsEnabled; }

        virtual Renderable *getRenderable() = 0;

    protected:
        Bound(const UUID &uuid = UUID::INVALID);

        virtual bool testSphere(const Sphere &sphere) const = 0;

        virtual bool testAabb(const Aabb &aabb) const = 0;

        virtual bool testObb(const Obb &obb) const = 0;

        virtual bool testFrustum(const Frustum &frustum) const = 0;

        TResult cloneProperties(const Component * const src) override;

        void onStart() override;

        virtual void onPositionChanged(const Vector3 &oldPos, const Vector3 &newPos) = 0;

        virtual void onOrientationChanged(const Quaternion &oldOri, const Quaternion &newOri) = 0;

        virtual void onScalingChanged(const Vector3 &oldScaling, const Vector3 &newScaling) = 0;
        
    private:
        /// 碰撞体分组ID
        ID          mGroupID {T3D_INVALID_ID};
        /// 是否可移动碰撞体
        bool        mIsMovable {false};
        /// 是否检测源碰撞体
        bool        mIsCollisionSource {false};
        /// 是否开启碰撞检测
        bool        mIsEnabled {false};
    };
}


#endif  /*__T3D_BOUND_H__*/
