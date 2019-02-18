/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Agent)
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


#ifndef __T3D_BOUND_H__
#define __T3D_BOUND_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Kernel/T3DObject.h"
#include "Kernel/T3DTransform.h"
#include "Memory/T3DSmartPtr.h"
#include "SceneGraph/T3DSGNode.h"


namespace Tiny3D
{
    /**
     * @brief 碰撞体基类
     */
    class T3D_ENGINE_API Bound : public Object
    {
    public:
        /**
         * @brief 碰撞体类型
         */
        enum Type
        {
            E_BT_NONE = 0,      /**< 未知碰撞体类型 */
            E_BT_SPHERE,        /**< 球包围体碰撞体 */
            E_BT_AABB,          /**< 轴对齐包围盒碰撞体 */
            E_BT_OBB,           /**< 有向包围盒碰撞体 */
            E_BT_FRUSTUM,       /**< 视锥体碰撞体 */
        };

        /**
         * @brief 析构函数
         */
        virtual ~Bound();

        /**
         * @brief 获取碰撞体类型
         */
        virtual Type getType() const = 0;

        /**
         * @brief 获取碰撞体ID
         */
        ID getID() const;

        /**
         * @brief 设置碰撞分组ID
         * @remarks 相同分组ID的碰撞体相互之间不会检测碰撞
         */
        void setGroupID(ID groupID);

        /**
         * @brief 获取碰撞分组ID
         */
        ID getGroupID() const;

        /**
         * @brief 获取球体对象
         * @remarks 为了加速碰撞检测，所有碰撞体都内置一个球体用于初步快速的相交
         *      检测。
         */
        const Sphere &getSphere() const;

        /**
         * @brief 相交检测
         */
        virtual bool test(BoundPtr bound) const;

        /**
         * @brief 获取包含当前碰撞体对应的场景结点
         */
        SGNodePtr getNode() const;

        /**
         * @brief 设置碰撞相交检测源，源对象发起检测，非源对象是被动检测
         * @param [in] isSource : true 表示源对象
         */
        void setCollisionSource(bool isSource);

        /**
         * @brief 获取是否碰撞相交检测源
         */
        bool isCollisionSource() const;

        /**
         * @brief 设置碰撞体是否有效
         */
        void setEnabled(bool isEnabled);

        /**
         * @brief 碰撞体是否有效
         */
        bool isEnabled() const;

        /**
         * @brief 获取可渲染对象，用于渲染该碰撞体
         * @return 返回可渲染对象
         */
        virtual SGRenderablePtr getRenderable() = 0;

        /**
         * @brief 克隆碰撞体对象
         * @return 返回一个新的碰撞体对象
         */
        virtual BoundPtr clone() const = 0;

    protected:
        /**
         * @brief 构造函数
         * @param [in] uID : 碰撞体唯一标识
         */
        Bound(ID uID, SGNode *node);

        /**
         * @brief 检测与球体碰撞体相交
         * @remarks 各类型碰撞体子类实现该相交检测
         */
        virtual bool testSphere(const Sphere &sphere) const = 0;

        /**
         * @brief 检测与轴对齐包围盒碰撞体相交
         * @remarks 各类型碰撞体子类实现该相交检测
         */
        virtual bool testAabb(const Aabb &aabb) const = 0;

        /**
         * @brief 检测与有向包围盒碰撞体相交
         * @remarks 各类型碰撞体子类实现该相交检测
         */
        virtual bool testObb(const Obb &obb) const = 0;

        /**
         * @brief 检测与视锥体包围盒碰撞相交
         * @remarks 各类型碰撞体子类实现该相交检测
         */
        virtual bool testFrustum(const Frustum &frustum) const = 0;

        /**
         * @brief 根据变换对象更新碰撞体
         */
        virtual void updateBound(const Transform &xform) = 0;

        /**
         * @brief 克隆属性
         */
        virtual void cloneProperties(BoundPtr bound) const;

    protected:
        Sphere      mSphere;            /**< 用于快速检测相交性的球体 */

    private:
        SGNode      *mNode;             /**< 包含碰撞体的结点对象 */
        ID          mID;                /**< 碰撞体ID */
        ID          mGroupID;           /**< 碰撞体分组ID */
        bool        mIsMovable;         /**< 是否可移动碰撞体 */
        bool        mIsCollisionSource; /**< 是否检测源碰撞体 */
        bool        mIsEnabled;         /**< 是否开启碰撞检测 */
    };
}


#include "T3DBound.inl"


#endif  /*__T3D_BOUND_H__*/
