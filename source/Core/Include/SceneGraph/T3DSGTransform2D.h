/***************************************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
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
 **************************************************************************************************/

#ifndef __T3D_SG_TRANSFORM_2D_H__
#define __T3D_SG_TRANSFORM_2D_H__


#include "SceneGraph/T3DSGNode.h"
#include "T3DMath.h"
#include "T3DVector2.h"
#include "T3DTransform.h"


namespace Tiny3D
{
    /**
     * @brief 在 Scene Graph 中2D物体使用的空间变换结点
     */
    class T3D_ENGINE_API SGTransform2D : public SGNode
    {
    public:
        /**
         * @brief 创建对象
         * @param [in] unID : 结点唯一标识，默认是自动生成
         * @return 返回一个2D变换结点对象
         */
        static SGTransform2DPtr create(uint32_t uID = E_NID_AUTOMATIC);

        /**
         * @brief 析构函数
         */
        virtual ~SGTransform2D();

        /**
         * @brief 从父类继承来重写接口，获取结点类型
         * @return 返回结点类型
         */
        virtual Type getNodeType() const override;

        /**
         * @brief 设置在父节点空间坐标系下位置
         * @param [in] rkPos : 在父节点空间坐标系下的位置
         */
        void setPosition(const Vector2 &rkPos);

        /**
         * @brief 设置在父节点空间坐标系下位置
         * @param [in] x : 父节点空间坐标系下的x坐标
         * @param [in] y : 父节点空间坐标系下的y坐标
         */
        void setPosition(Real x, Real y);

        /**
         * @brief 返回在父节点空间坐标系下的位置
         * @return 返回的位置
         */
        const Vector2 &getPosition() const;

        /**
         * @brief 设置在父结点空间坐标系下的朝向
         * @param [in] rkQ : 方向四元素
         * @return void
         * @note 虽然是2D空间的变换，然后旋转可以实现分别绕X、Y、Z三个轴的旋转，
         *  因此在这里继续用四元素表示旋转
         * @see void setOrientation(Real w, Real x, Real y, Real z)
         */
        void setOrientation(const Quaternion &rkQ);

        /**
         * @brief 设置在父结点空间坐标系下的朝向
         * @param [in] w : 四元素的w分量
         * @param [in] x : 四元素的x分量
         * @param [in] y : 四元素的y分量
         * @param [in] z : 四元素的z分量
         * @return void
         * @see void setOrientation(const Quaternion &rkQ)
         */
        void setOrientation(Real w, Real x, Real y, Real z);

        /**
         * @brief 设置在父结点空间坐标系下的朝向
         * @param [in] radian : 旋转弧度
         * @param [in] axis : 父结点空间下的旋转轴
         * @return void
         * @see void setOrientation(const Quaternion &rkQ)
         */
        void setOrientation(const Radian &radian, const Vector3 &axis);

        /**
         * @brief 获取在父结点空间坐标系下的朝向
         * @return 返回一个表示朝向的四元素对象
         */
        const Quaternion &getOrientation() const;

        /**
         * @brief 设置在父结点空间坐标系下的缩放
         * @param [in] rkScale : 缩放值
         * @return void
         * @see void setScale(Real x, Real y)
         */
        void setScale(const Vector2 &rkScale);

        /**
         * @brief 设置在父结点空间坐标系下的缩放
         * @param [in] x : X轴缩放值
         * @param [in] y : Y轴缩放值
         * @return void
         * @see void setScale(const Vector2 &rkScale)
         */
        void setScale(Real x, Real y);

        /**
         * @brief 获取在父结点空间坐标系下的缩放值
         * @return 返回一个表示三个周缩放值的向量，每个分量表示一个轴的缩放值
         */
        const Vector2 &getScale() const;

        /**
         * @brief 平移一个偏移值
         * @param [in] rkOffset : 三个坐标轴上的偏移值
         * @return void
         */
        void translate(const Vector2 &rkOffset);

        /**
         * @brief 平移一个偏移值
         * @param [in] x : X轴上偏移值
         * @param [in] y : Y轴上偏移值
         * @return void
         */
        void translate(Real x, Real y);

        /**
         * @brief 朝一个向量所指方向移动某一个步长
         * @param [in] rkOrientation : 方向向量
         * @param [in] step : 移动步长
         * @return void
         */
        void translate(const Vector2 &rkOrientation, Real step);

        /**
         * @brief 在当前方向上旋转一个四元素表示的旋转变换
         * @param [in] rkQ : 表示旋转变换的四元素
         * @return void
         */
        void rotate(const Quaternion &rkQ);

        /**
         * @brief 在当前方向上旋转一个给定旋转轴和绕该轴的旋转角表示的旋转变换
         * @param [in] axis : 旋转轴
         * @param [in] radians : 旋转角，单位：弧度
         * @return void
         * @see void rotate(const Vector3 &axis, const Degree &degrees)
         */
        void rotate(const Vector3 &axis, const Radian &radians);

        /**
         * @brief 在当前方向上旋转一个给定旋转轴和绕该轴的旋转角表示的旋转变换
         * @param [in] axis : 旋转轴
         * @param [in] degrees : 旋转角，单位：角度
         * @return void
         * @see void rotate(const Vector3 &axis, const Radian &radians)
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
         * @param [in] rkScale : 缩放值
         * @return void
         * @see void scale(Real x, Real y)
         */
        void scale(const Vector2 &rkScale);

        /**
         * @brief 在当前大小下缩放
         * @param [in] x : X轴缩放值
         * @param [in] y : Y轴缩放值
         * @param [in] z : Z轴缩放值
         * @return void
         * @see void scale(const Vector2 &rkScale)
         */
        void scale(Real x, Real y);

        /**
         * @brief 获取局部到世界的变换
         * @return 返回一个局部到世界的变换对象
         */
        virtual const Transform &getLocalToWorldTransform();

        /**
         * @brief 获取局部变换
         * @return 返回一个局部变换对象
         */
        Transform getLocalTransform() const;

        /**
         * @brief 从父类继承，重写父类克隆方法，以实现本类对象的复制操作
         * @return 返回一个新的3D变换结点对象
         */
        virtual NodePtr clone() const override;
        
    protected:
        /**
         * @brief 从父类继承，重写以响应结点挂到父结点的通知事件
         * @param [in] parent : 父结点对象
         * @return void
         */
        virtual void onAttachParent(const NodePtr &parent) override;

        /**
         * @brief 从父类继承，重写以响应结点从父结点拿走的通知事件
         * @param [in] parent : 父结点对象
         * @return void
         */
        virtual void onDetachParent(const NodePtr &parent) override;

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
        virtual void cloneProperties(const NodePtr &node) const override;

    protected:
        /**
         * @brief 默认构造函数
         * @param [in] unID : 结点唯一标识，默认是自动生成
         */
        SGTransform2D(uint32_t uID = E_NID_AUTOMATIC);

        Vector2     mPosition;          /// 父节点坐标系下的局部位置
        Quaternion  mOrientation;       /// 父节点坐标系下的局部朝向
        Vector2     mScale;             /// 父节点坐标系下的局部大小

        Transform   mWorldTransform;    /// 从局部到世界的变换对象
    };
}


#include "T3DSGTransform2D.inl"


#endif  /*__T3D_SG_TRANSFORM_2D_H__*/
