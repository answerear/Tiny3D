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
     * \brief 碰撞体组件抽象基类，提供类型分发相交检测与分组/开关属性
     * \remarks 具体形状由派生类维护局部与世界空间几何体，相交检测使用派生类的世界空间数据
     */
    TCLASS()
    class T3D_ENGINE_API Bound : public Component
    {
        TRTTI_ENABLE(Component)
        TRTTI_FRIEND

    public:
        /**
         * \brief 碰撞体几何形状类型标识
         */
        TENUM()
        enum class Type : uint32_t
        {
            /// 未知或未支持的碰撞体类型
            NONE = 0,
            /// 球体
            SPHERE,
            /// 轴对齐包围盒（AABB）
            AABB,
            /// 有向包围盒（OBB）
            OBB,
            /// 胶囊体
            CAPSULE,
            /// 视锥体
            FRUSTUM,
        };

        ~Bound() override;

        /**
         * \brief 返回本碰撞体的形状类型
         * \return 派生类对应的 Type 枚举值
         */
        TFUNCTION("Description"="Bound Type")
        virtual Type getType() const = 0;

        /// 设置碰撞体分组 ID
        TPROPERTY(RTTRFuncName="groupID", RTTRFuncType="setter", "Description"="Group ID")
        void setGroupID(ID groupID) { mGroupID = groupID; }

        /// 返回碰撞体分组 ID
        TPROPERTY(RTTRFuncName="groupID", RTTRFuncType="getter", "Description"="Group ID")
        ID getGroupID() const { return mGroupID; }

        /**
         * \brief 与另一碰撞体做相交检测
         * \param [in] bound : 待检测的对方碰撞体，须为非空且类型已知
         * \return 两几何体相交返回 true；对方类型为 NONE 或未知时返回 false
         * \remarks 按对方 getType() 取其世界空间几何体，再调用本类对应的 test* 实现
         */
        TFUNCTION()
        virtual bool test(Bound *bound) const;

        /// 设置是否为碰撞检测源（参与发起检测的一方）
        TPROPERTY(RTTRFuncName="collisionSource", RTTRFuncType="setter", "Description"="Collision Source")
        void setCollisionSource(bool isSource) { mIsCollisionSource = isSource; }

        /// 返回是否为碰撞检测源
        TPROPERTY(RTTRFuncName="collisionSource", RTTRFuncType="getter", "Description"="Collision Source")
        bool isCollisionSource() const { return mIsCollisionSource; }

        /// 设置是否启用碰撞检测
        TPROPERTY(RTTRFuncName="enable", RTTRFuncType="setter", "Description"="Enable")
        void setEnabled(bool isEnabled) { mIsEnabled = isEnabled; }

        /// 返回是否启用碰撞检测
        TPROPERTY(RTTRFuncName="enable", RTTRFuncType="getter", "Description"="Enable")
        bool isEnabled() const { return mIsEnabled; }

        /**
         * \brief 获取用于调试或可视化的 Renderable
         * \return 当前各派生类实现均返回 nullptr
         */
        virtual Renderable *getRenderable() = 0;

    protected:
        Bound() = default;

        /**
         * \brief 以指定 UUID 构造碰撞体组件
         * \param [in] uuid : 组件实例 UUID
         */
        Bound(const UUID &uuid);

        /**
         * \brief 检测本碰撞体与世界空间球体是否相交
         * \param [in] sphere : 世界空间球体
         * \return 相交返回 true
         */
        virtual bool testSphere(const Sphere &sphere) const = 0;

        /**
         * \brief 检测本碰撞体与世界空间 AABB 是否相交
         * \param [in] aabb : 世界空间轴对齐包围盒
         * \return 相交返回 true
         */
        virtual bool testAabb(const Aabb &aabb) const = 0;

        /**
         * \brief 检测本碰撞体与世界空间 OBB 是否相交
         * \param [in] obb : 世界空间有向包围盒
         * \return 相交返回 true
         */
        virtual bool testObb(const Obb &obb) const = 0;

        /**
         * \brief 检测本碰撞体与世界空间胶囊体是否相交
         * \param [in] capsule : 世界空间胶囊体
         * \return 相交返回 true
         */
        virtual bool testCapsule(const Capsule &capsule) const = 0;

        /**
         * \brief 检测本碰撞体与世界空间视锥体是否相交
         * \param [in] frustum : 世界空间视锥体
         * \return 相交返回 true
         */
        virtual bool testFrustum(const Frustum &frustum) const = 0;

        /**
         * \brief 从源组件复制碰撞体通用属性
         * \param [in] src : 源 Component，须可转为 Bound
         * \return 基类复制成功时继续复制分组 ID、可移动标志、碰撞源与启用状态；否则返回基类错误码
         */
        TResult cloneProperties(const Component * const src) override;
        
    private:
        /// 碰撞体分组 ID，默认 T3D_INVALID_ID
        ID          mGroupID {T3D_INVALID_ID};
        /// 是否可移动碰撞体，仅通过 cloneProperties 复制，无公开访问器
        bool        mIsMovable {false};
        /// 是否作为碰撞检测源
        bool        mIsCollisionSource {false};
        /// 是否启用碰撞检测
        bool        mIsEnabled {false};
    };
}


#endif  /*__T3D_BOUND_H__*/
