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


#ifndef __T3D_SG_TRANSFORM3D_H__
#define __T3D_SG_TRANSFORM3D_H__


#include "SceneGraph/T3DSGNode.h"
#include "Kernel/T3DTransform.h"


namespace Tiny3D
{
    /**
     * @brief 带变换属性的场景树结点
     */
    class T3D_ENGINE_API SGTransform3D : public SGNode
    {
    public:
        /**
         * @brief 创建对象
         * @param [in] unID : 结点唯一标识，默认是自动生成
         * @return 返回一个3D变换结点对象
         */
        static SGTransform3DPtr create(ID uID = E_NID_AUTOMATIC);

        /**
         * @brief 析构函数
         */
        virtual ~SGTransform3D();

        /**
         * @brief 从父类继承来重写接口，获取结点类型
         * @return 返回结点类型
         */
        virtual Type getNodeType() const override;

        /**
         * @brief 设置在父结点空间坐标系下的位置
         * @param [in] pos : 位置
         * @return void
         * @see void setPosition(Real x, Real y, Real z)
         */
        void setPosition(const Vector3 &pos);

        /**
         * @brief 设置在父结点空间坐标系下的位置
         * @param [in] x : X方向位置
         * @param [in] y : Y方向位置
         * @param [in] z : Z方向位置
         * @return void
         * @see void setPosition(const Vector3 &rkPos)
         */
        void setPosition(Real x, Real y, Real z);

        /**
         * @brief 获取在父结点空间坐标系下的位置
         * @return 返回位置
         */
        const Vector3 &getPosition() const;

        /**
         * @brief 设置在父结点空间坐标系下的朝向
         * @param [in] orientation : 方向四元素
         * @return void
         * @see void setOrientation(Real w, Real x, Real y, Real z)
         */
        void setOrientation(const Quaternion &orientation);

        /**
         * @brief 设置在父结点空间坐标系下的朝向
         * @param [in] w : 四元素的w分量
         * @param [in] x : 四元素的x分量
         * @param [in] y : 四元素的y分量
         * @param [in] z : 四元素的z分量
         * @return void
         * @see void setOrientation(const Quaternion &orientation)
         */
        void setOrientation(Real w, Real x, Real y, Real z);

        /**
         * @brief 设置在父结点空间坐标系下的朝向
         * @param [in] radian : 旋转弧度
         * @param [in] axis : 父结点空间下的旋转轴
         * @return void
         * @see void setOrientation(const Quaternion &ori)
         */
        void setOrientation(const Radian &radian, const Vector3 &axis);

        /**
         * @brief 获取在父结点空间坐标系下的朝向
         * @return 返回一个表示朝向的四元素对象
         */
        const Quaternion &getOrientation() const;

        /**
         * @brief 设置在父结点空间坐标系下的缩放
         * @param [in] scaling : 缩放值
         * @return void
         * @see void setScaling(Real x, Real y, Real z)
         */
        void setScaling(const Vector3 &scaling);

        /**
         * @brief 设置在父结点空间坐标系下的缩放
         * @param [in] x : X轴缩放值
         * @param [in] y : Y轴缩放值
         * @param [in] z : Z轴缩放值
         * @return void
         * @see void setScaling(const Vector3 &scaling)
         */
        void setScaling(Real x, Real y, Real z);

        /**
         * @brief 获取在父结点空间坐标系下的缩放值
         * @return 返回一个表示三个周缩放值的向量，每个分量表示一个轴的缩放值
         */
        const Vector3 &getScaling() const;

        /**
         * @brief 在当前位置基础上平移一个偏移值
         * @param [in] offset : 三个坐标轴上的偏移值
         * @return void
         */
        void translate(const Vector3 &offset);

        /**
         * @brief 在当前位置基础上平移一个偏移值
         * @param [in] x : X轴上偏移值
         * @param [in] y : Y轴上偏移值
         * @param [in] z : Z轴上偏移值
         * @return void
         */
        void translate(Real x, Real y, Real z);

        /**
         * @brief 在当前位置基础上，朝一个向量所指方向移动某一个步长
         * @param [in] dir : 方向向量
         * @param [in] step : 移动步长
         * @return void
         */
        void translate(const Vector3 &dir, Real step);

        /**
         * @brief 在当前方向上旋转一个四元素表示的旋转变换
         * @param [in] orientation : 表示旋转变换的四元素
         * @return void
         */
        void rotate(const Quaternion &orientation);

        /**
         * @brief 在当前方向上旋转一个给定旋转轴和绕该轴的旋转角表示的旋转变换
         * @param [in] axis : 旋转轴
         * @param [in] radians : 旋转角，单位：弧度
         * @return void
         * @see void rotate(const Vector3 &axis, const Degree &degrees)
         * @see void rotate(const Quaternion &orientation)
         */
        void rotate(const Vector3 &axis, const Radian &radians);

        /**
         * @brief 在当前方向上旋转一个给定旋转轴和绕该轴的旋转角表示的旋转变换
         * @param [in] axis : 旋转轴
         * @param [in] degrees : 旋转角，单位：角度
         * @return void
         * @see void rotate(const Vector3 &axis, const Radian &radians)
         * @see void rotate(const Quaternion &orientation)
         */
        void rotate(const Vector3 &axis, const Degree &degrees);

        /**
         * @brief 在当前方向上绕X轴旋转，倾斜角
         * @param [in] degrees : 倾斜角，单位：角度
         * @return void
         * @see void pitch(const Radian &radians)
         */
        void pitch(const Degree &degrees);

        /**
         * @brief 在当前方向上绕Y轴旋转，航向角
         * @param [in] degrees : 航向角，单位：角度
         * @return void
         * @see void yaw(const Radian &radians)
         */
        void yaw(const Degree &degrees);

        /**
         * @brief 在当前方向上绕Z周旋转，翻滚角
         * @param [in] degrees : 翻滚角，单位：角度
         * @return void
         * @see void roll(const Radian &radians)
         */
        void roll(const Degree &degrees);

        /**
         * @brief 在当前方向上绕X轴旋转，倾斜角
         * @param [in] degrees : 倾斜角，单位：弧度
         * @return void
         * @see void pitch(const Degree &degrees)
         */
        void pitch(const Radian &radians);

        /**
         * @brief 在当前方向上绕Y轴旋转，航向角
         * @param [in] degrees : 航向角，单位：弧度
         * @return void
         * @see void yaw(const Degree &degrees)
         */
        void yaw(const Radian &radians);

        /**
         * @brief 在当前方向上绕Z周旋转，翻滚角
         * @param [in] degrees : 翻滚角，单位：弧度
         * @return void
         * @see void roll(const Degree &degrees)
         */
        void roll(const Radian &radians);

        /**
         * @brief 在当前大小下缩放
         * @param [in] scaling : 缩放值
         * @return void
         * @see void scale(Real x, Real y, Real z)
         */
        void scale(const Vector3 &scaling);

        /**
         * @brief 在当前大小下缩放
         * @param [in] x : X轴缩放值
         * @param [in] y : Y轴缩放值
         * @param [in] z : Z轴缩放值
         * @return void
         * @see void scale(const Vector3 &scaling)
         */
        void scale(Real x, Real y, Real z);

        /**
         * @brief 直接设置一个局部变换矩阵
         * @param [in] m : 局部变换矩阵
         * @return void
         * @note 本接口会根据设置的局部变换矩阵来分解出旋转、平移、缩放三个分量
         */
        void setLocalMatrix(const Matrix4 &m);

        /**
         * @brief 获取局部到世界的变换
         * @return 返回一个局部到世界的变换对象
         */
        virtual const Transform &getLocalToWorldTransform() const;

        /**
         * @brief 获取局部变换
         * @return 返回一个局部变换对象
         */
        Transform getLocalTransform() const;

        /**
         * @brief 设置结点是否需要重绘、重新计算标记
         * @param [in] isDirty : 结点数据是否脏了标记
         * @param [in] recursive : 是否需要递归所有子节点都设置上该标记，
         *       默认为只设置本结点
         * @return void
         * @see bool isDirty() const
         */
        virtual void setDirty(bool isDirty, bool recursive = false);

        /**
         * @brief 返回结点数据是否脏，需要重新绘制、计算等标记
         * @return 返回结点数据脏标记
         * @see void setDirty(bool isDirty, bool recursive)
         */
        bool isDirty() const;

        /**
         * @brief 从父类继承，重写父类克隆方法，以实现本类对象的复制操作
         * @return 返回一个新的3D变换结点对象
         */
        virtual NodePtr clone() const override;

    protected:
        /**
         * @brief 默认构造函数
         * @param [in] uID : 结点唯一标识，默认是自动生成
         */
        SGTransform3D(ID uID = E_NID_AUTOMATIC);

        /**
         * @brief 从父类继承，重写以响应结点挂到父结点的通知事件
         * @param [in] parent : 父结点对象
         * @return void
         */
        virtual void onAttachParent(NodePtr parent) override;

        /**
         * @brief 从父类继承，重写以响应结点从父结点拿走的通知事件
         * @param [in] parent : 父结点对象
         * @return void
         */
        virtual void onDetachParent(NodePtr parent) override;

        /**
         * @brief 从父类继承，重写实现更新变换操作
         * @return void
         */
        virtual void updateTransform() override;

        /**
         * @brief 从父类继承，重写实现3D变换对象属性数据的复制
         * @param [in] node : 新的结点对象
         * @return void
         */
        virtual TResult cloneProperties(NodePtr node) const override;

    private:
        Vector3     mPosition;          /**< 父节点坐标系下的局部位置 */
        Quaternion  mOrientation;       /**< 父节点坐标系下的局部朝向 */
        Vector3     mScaling;           /**< 父节点坐标系下的局部大小 */

        mutable Transform   mWorldTransform;/**< 从局部到世界的变换对象 */

        mutable bool        mIsDirty;       /**< 结点数据脏标记，需要重新计算 */
    };
}


#include "T3DSGTransform3D.inl"


#endif  /*__T3D_SG_TRANSFORM3D_H__*/
