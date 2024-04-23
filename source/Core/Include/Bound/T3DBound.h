/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
        Bound() = default;

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
