﻿/*******************************************************************************
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
#include "T3DTypedef.h"
#include "Kernel/T3DObject.h"
#include "Kernel/T3DTransform.h"
#include "Memory/T3DSmartPtr.h"
#include "Scene/T3DSceneNode.h"


namespace Tiny3D
{
    /**
     * @class   Bound
     * @brief   碰撞体基类
     */
    class T3D_ENGINE_API Bound : public Object
    {
    public:
        /**
         * @enum    Type
         * @brief   碰撞体类型
         */
        enum class Type : uint32_t
        {
            NONE = 0,       /**< 未知碰撞体类型 */
            SPHERE,         /**< 球包围体碰撞体 */
            AABB,           /**< 轴对齐包围盒碰撞体 */
            OBB,            /**< 有向包围盒碰撞体 */
            FRUSTUM,        /**< 视锥体碰撞体 */
        };

        /**
         * @enum    BoundID
         * @brief   碰撞体ID
         */
        enum BoundID : ID
        {
            E_BID_AUTOMATIC = 0xFFFFFFFF,   /**< 自动生成ID */
            E_BID_INVALID = 0,              /**< 无效ID */
        };

        /**
         * @fn  virtual Bound::~Bound();
         * @brief   析构函数
         */
        virtual ~Bound();

        /**
         * @fn  virtual Type Bound::getType() const = 0;
         * @brief   获取碰撞体类型
         * @return  The type.
         */
        virtual Type getType() const = 0;

        /**
         * @fn  ID Bound::getID() const;
         * @brief   获取碰撞体ID
         * @return  The identifier.
         */
        ID getID() const;

        /**
         * @fn  void Bound::setGroupID(ID groupID);
         * @brief   设置碰撞分组ID
         * @param   groupID Identifier for the group.
         *
         * ### remarks  相同分组ID的碰撞体相互之间不会检测碰撞.
         */
        void setGroupID(ID groupID);

        /**
         * @fn  ID Bound::getGroupID() const;
         * @brief   获取碰撞分组ID
         * @return  The group identifier.
         */
        ID getGroupID() const;

        /**
         * @fn  const Sphere Bound::&getSphere() const;
         * @brief   获取球体对象
         * @return  The sphere.
         * @remarks 为加速碰撞检测，所有碰撞体都内置球体用于初步快速的相交检测.
         */
        const Sphere &getSphere() const;

        /**
         * @fn  virtual bool Bound::test(BoundPtr bound) const;
         * @brief   相交检测
         * @param   bound   The bound.
         * @return  True if it succeeds, false if it fails.
         */
        virtual bool test(BoundPtr bound) const;

        /**
         * @fn  void Bound::setCollisionSource(bool isSource);
         * @brief   设置碰撞相交检测源，源对象发起检测，非源对象是被动检测
         * @param [in]  isSource    : true 表示源对象.
         */
        void setCollisionSource(bool isSource);

        /**
         * @fn  bool Bound::isCollisionSource() const;
         * @brief   获取是否碰撞相交检测源
         * @return  True if collision source, false if not.
         */
        bool isCollisionSource() const;

        /**
         * @fn  void Bound::setEnabled(bool isEnabled);
         * @brief   设置碰撞体是否有效
         * @param   isEnabled   True if is enabled, false if not.
         */
        void setEnabled(bool isEnabled);

        /**
         * @fn  bool Bound::isEnabled() const;
         * @brief   碰撞体是否有效
         * @return  True if enabled, false if not.
         */
        bool isEnabled() const;

        /**
         * @fn  virtual SceneRenderablePtr Bound::getRenderable() = 0;
         * @brief   获取可渲染对象，用于渲染该碰撞体
         * @return  返回可渲染对象.
         */
        virtual SceneRenderablePtr getRenderable() = 0;

        /**
         * @fn  virtual BoundPtr Bound::clone() const = 0;
         * @brief   克隆碰撞体对象
         * @return  返回一个新的碰撞体对象.
         */
        virtual BoundPtr clone() const = 0;

        /**
         * @fn  virtual void Bound::updateBound(const Transform &xform) = 0;
         * @brief   根据变换对象更新碰撞体
         * @param   xform   The transform.
         */
        virtual void updateBound(const Transform &xform) = 0;

    protected:
        /**
         * @fn  Bound::Bound(SceneNode *node, ID uID = E_BID_AUTOMATIC);
         * @brief   构造函数
         * @param [in]  node    : 碰撞体所属的场景结点.
         * @param [in]  uID     (Optional) : 碰撞体唯一标识.
         */
        Bound(ID uID = E_BID_AUTOMATIC);

        /**
         * @fn  virtual bool Bound::testSphere(const Sphere &sphere) const = 0;
         * @brief   检测与球体碰撞体相交
         * @param   sphere  The sphere.
         * @return  True if the test passes, false if the test fails.
         * @remarks 各类型碰撞体子类实现该相交检测.
         */
        virtual bool testSphere(const Sphere &sphere) const = 0;

        /**
         * @fn  virtual bool Bound::testAabb(const Aabb &aabb) const = 0;
         * @brief   检测与轴对齐包围盒碰撞体相交
         * @param   aabb    The aabb.
         * @return  True if the test passes, false if the test fails.
         * @remarks 各类型碰撞体子类实现该相交检测.
         */
        virtual bool testAabb(const Aabb &aabb) const = 0;

        /**
         * @fn  virtual bool Bound::testObb(const Obb &obb) const = 0;
         * @brief   检测与有向包围盒碰撞体相交
         * @param   obb The obb.
         * @return  True if the test passes, false if the test fails.
         * @remarks 各类型碰撞体子类实现该相交检测.
         */
        virtual bool testObb(const Obb &obb) const = 0;

        /**
         * @fn  virtual bool Bound::testFrustum(
         *      const Frustum &frustum) const = 0;
         * @brief   检测与视锥体包围盒碰撞相交
         * @param   frustum The frustum.
         * @return  True if the test passes, false if the test fails.
         * @remarks 各类型碰撞体子类实现该相交检测.
         */
        virtual bool testFrustum(const Frustum &frustum) const = 0;

        /**
         * @fn  virtual void Bound::cloneProperties(BoundPtr bound) const;
         * @brief   克隆属性
         * @param   bound   The bound.
         */
        virtual void cloneProperties(BoundPtr bound) const;

    private:
        /**
         * @fn  ID Bound::makeGlobalID() const;
         * @brief   Makes global identifier
         * @return  An ID.
         */
        ID makeGlobalID() const;

    protected:
        Sphere      mSphere;            /**< 用于快速检测相交性的球体 */

    private:
        ID          mID;                /**< 碰撞体ID */
        ID          mGroupID;           /**< 碰撞体分组ID */
        bool        mIsMovable;         /**< 是否可移动碰撞体 */
        bool        mIsCollisionSource; /**< 是否检测源碰撞体 */
        bool        mIsEnabled;         /**< 是否开启碰撞检测 */
    };
}


#include "T3DBound.inl"


#endif  /*__T3D_BOUND_H__*/
