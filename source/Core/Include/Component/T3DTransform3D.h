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

#ifndef __T3D_TRANSFORM3D_H__
#define __T3D_TRANSFORM3D_H__


#include "Component/T3DTransformNode.h"
#include "Kernel/T3DTransform.h"


namespace Tiny3D
{
    /**
     * \brief 三维空间变换组件，管理局部 TRS 并缓存局部到世界的 Transform
     * \remarks 变换变化时标记脏并可选递归标记子节点；提供位置/朝向/缩放变更回调。
     */
    TCLASS()
    class T3D_ENGINE_API Transform3D : public TransformNode
    {
        TRTTI_ENABLE(TransformNode)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 创建带新生成 UUID 的 Transform3D 实例
         * \return 新建的 Transform3D 智能指针
         */
        static Transform3DPtr create();

        /// 析构
        ~Transform3D() override;

        /**
         * \brief 克隆当前 Transform3D，生成带新 UUID 的实例
         * \return 克隆成功返回新组件；cloneProperties 失败返回 nullptr
         */
        ComponentPtr clone() const override;

        /**
         * \brief 设置局部位置；值变化时触发回调并标记脏
         * \param [in] pos : 父坐标系下的位置
         */
        TPROPERTY(RTTRFuncName="Position", RTTRFuncType="setter")
        void setPosition(const Vector3 &pos);

        /**
         * \brief 设置局部位置（分量形式）
         * \param [in] x : X 分量
         * \param [in] y : Y 分量
         * \param [in] z : Z 分量
         */
        void setPosition(Real x, Real y, Real z);

        /// 返回父坐标系下的局部位置
        TPROPERTY(RTTRFuncName="Position", RTTRFuncType="getter")
        const Vector3 &getPosition() const;

        /**
         * \brief 设置局部朝向；值变化时触发回调并标记脏
         * \param [in] orientation : 四元数朝向
         */
        TPROPERTY(RTTRFuncName="Orientation",  RTTRFuncType="setter")
        void setOrientation(const Quaternion &orientation);

        /**
         * \brief 设置局部朝向（四元数分量形式）
         * \param [in] w : 实部
         * \param [in] x : i 分量
         * \param [in] y : j 分量
         * \param [in] z : k 分量
         */
        void setOrientation(Real w, Real x, Real y, Real z);

        /**
         * \brief 设置局部朝向（轴角形式）
         * \param [in] radian : 旋转角
         * \param [in] axis : 旋转轴
         */
        void setOrientation(const Radian &radian, const Vector3 &axis);

        /// 返回父坐标系下的局部朝向
        TPROPERTY(RTTRFuncName="Orientation",  RTTRFuncType="getter")
        const Quaternion &getOrientation() const;

        /**
         * \brief 设置局部缩放；值变化时触发回调并标记脏
         * \param [in] scaling : 缩放向量
         */
        TPROPERTY(RTTRFuncName="Scaling",  RTTRFuncType="setter")
        void setScaling(const Vector3 &scaling);

        /**
         * \brief 设置局部缩放（分量形式）
         * \param [in] x : X 缩放
         * \param [in] y : Y 缩放
         * \param [in] z : Z 缩放
         */
        void setScaling(Real x, Real y, Real z);

        /// 返回父坐标系下的局部缩放
        TPROPERTY(RTTRFuncName="Scaling",  RTTRFuncType="getter")
        const Vector3 &getScaling() const;

        /**
         * \brief 将局部朝向分解为 YXZ 欧拉角（弧度）
         * \param [out] pitch : 俯仰角
         * \param [out] yaw : 偏航角
         * \param [out] roll : 翻滚角
         */
        void getRotation(Radian &pitch, Radian &yaw, Radian &roll) const;

        /**
         * \brief 将局部朝向分解为 YXZ 欧拉角（角度）
         * \param [out] pitch : 俯仰角
         * \param [out] yaw : 偏航角
         * \param [out] roll : 翻滚角
         */
        void getRotation(Degree &pitch, Degree &yaw, Degree &roll) const;
        
        /**
         * \brief 返回 YXZ 欧拉角（角度）组成的 Vector3
         * \return (pitch°, yaw°, roll°)
         */
        Vector3 getRotation() const;

        /**
         * \brief 在局部位置上累加偏移
         * \param [in] offset : 位移向量；为零时不操作
         */
        void translate(const Vector3 &offset);

        /**
         * \brief 在局部位置上累加偏移（分量形式）
         * \param [in] x : X 偏移
         * \param [in] y : Y 偏移
         * \param [in] z : Z 偏移
         */
        void translate(Real x, Real y, Real z);

        /**
         * \brief 沿指定方向移动指定步长
         * \param [in] dir : 方向向量
         * \param [in] step : 步长
         */
        void translate(const Vector3 &dir, Real step);

        /**
         * \brief 右乘四元数旋转局部朝向
         * \param [in] orientation : 旋转四元数；为单位四元数时不操作
         */
        void rotate(const Quaternion &orientation);

        /**
         * \brief 绕轴旋转（弧度）
         * \param [in] axis : 旋转轴
         * \param [in] radians : 旋转角
         */
        void rotate(const Vector3 &axis, const Radian &radians);

        /**
         * \brief 绕轴旋转（角度）
         * \param [in] axis : 旋转轴
         * \param [in] degrees : 旋转角
         */
        void rotate(const Vector3 &axis, const Degree &degrees);

        /**
         * \brief 以 YXZ 欧拉角（弧度）直接设置局部朝向
         * \param [in] pitch : 俯仰角
         * \param [in] yaw : 偏航角
         * \param [in] roll : 翻滚角
         */
        void rotate(const Radian &pitch, const Radian &yaw, const Radian &roll);

        /**
         * \brief 以 YXZ 欧拉角（角度）直接设置局部朝向
         * \param [in] pitch : 俯仰角
         * \param [in] yaw : 偏航角
         * \param [in] roll : 翻滚角
         */
        void rotate(const Degree &pitch, const Degree &yaw, const Degree &roll);

        /// 绕 X 轴旋转（角度）
        void pitch(const Degree &degrees);

        /// 绕 X 轴旋转（弧度）
        void pitch(const Radian &radians);

        /// 绕 Y 轴旋转（角度）
        void yaw(const Degree &degrees);

        /// 绕 Y 轴旋转（弧度）
        void yaw(const Radian &radians);

        /// 绕 Z 轴旋转（角度）
        void roll(const Degree &degrees);

        /// 绕 Z 轴旋转（弧度）
        void roll(const Radian &radians);

        // Matrix3 fromEulerAnglesXYZ(const Radian &xAngle, const Radian &yAngle, const Radian &zAngle);
        //
        // Matrix3 fromEulerAnglesXZY(const Radian &xAngle, const Radian &zAngle, const Radian &yAngle);
        //
        // Matrix3 fromEulerAnglesYXZ(const Radian &yAngle, const Radian &xAngle, const Radian &zAngle);
        //
        // Matrix3 fromEulerAnglesYZX(const Radian &yAngle, const Radian &zAngle, const Radian &xAngle);
        //
        // Matrix3 fromEulerAnglesZXY(const Radian &zAngle, const Radian &xAngle, const Radian &yAngle);
        //
        // Matrix3 fromEulerAnglesZYX(const Radian &zAngle, const Radian &yAngle, const Radian &xAngle);
        //
        // void toEulerAnglesXYZ(Radian &xAngle, Radian &yAngle, Radian &zAngle);
        //
        // void toEulerAnglesXZY(Radian &xAngle, Radian &zAngle, Radian &yAngle);
        //
        // void toEulerAnglesYXZ(Radian &yAngle, Radian &xAngle, Radian &zAngle);
        //
        // void toEulerAnglesYZX(Radian &yAngle, Radian &zAngle, Radian &xAngle);
        //
        // void toEulerAnglesZXY(Radian &zAngle, Radian &xAngle, Radian &yAngle);
        //
        // void toEulerAnglesZYX(Radian &zAngle, Radian &yAngle, Radian &xAngle);

        /**
         * \brief 累加局部缩放
         * \param [in] scaling : 增量缩放；与当前值相同时不操作
         */
        void scale(const Vector3 &scaling);

        /**
         * \brief 累加局部缩放（分量形式）
         * \param [in] x : X 增量
         * \param [in] y : Y 增量
         * \param [in] z : Z 增量
         */
        void scale(Real x, Real y, Real z);

        /**
         * \brief 从 4x4 矩阵分解并设置局部 TRS，标记脏
         * \param [in] m : 含 TRS 的矩阵
         */
        void setLocalMatrix(const Matrix4 &m);

        /**
         * \brief 返回局部到世界的 Transform（脏时递归合成父变换）
         * \return 缓存的世界 Transform 引用
         */
        virtual const Transform &getLocalToWorldTransform() const;

        /**
         * \brief 返回仅含局部 TRS 的 Transform（不含父级）
         * \return 由 mPosition、mScaling、mOrientation 构造的 Transform
         */
        Transform getLocalTransform() const;

        /**
         * \brief 每帧刷新世界 Transform 缓存
         */
        void onUpdate() override;

        /**
         * \brief 设置脏标记并可递归标记所有子 Transform3D
         * \param [in] isDirty : 脏标记值
         * \param [in] recursive : 是否递归到子节点
         */
        virtual void setDirty(bool isDirty, bool recursive = false);

        /// 返回世界 Transform 是否需要重新计算
        bool isDirty() const;

        /// 位置变更回调类型：(旧位置, 新位置)
        using PositionChangedCallback = TFunction<void(const Vector3&, const Vector3&)>;
        
        /**
         * \brief 注册位置变更回调
         * \param [in] component : 回调所属组件（作 map 键）
         * \param [in] callback : 回调函数
         */
        void addPositionChangedCallback(Component *component, const PositionChangedCallback &callback);

        /**
         * \brief 移除位置变更回调
         * \param [in] component : 先前注册的组件
         */
        void removePositionChangedCallback(Component *component);

        /// 朝向变更回调类型：(旧朝向, 新朝向)
        using OrientationChangedCallback = TFunction<void(const Quaternion&, const Quaternion&)>;
        
        /**
         * \brief 注册朝向变更回调
         * \param [in] component : 回调所属组件
         * \param [in] callback : 回调函数
         */
        void addOrientationChangedCallback(Component *component, const OrientationChangedCallback &callback);

        /**
         * \brief 移除朝向变更回调
         * \param [in] component : 先前注册的组件
         */
        void removeOrientationChangedCallback(Component *component);

        /// 缩放变更回调类型：(旧缩放, 新缩放)
        using ScalingChangedCallback = TFunction<void(const Vector3&, const Vector3&)>;

        /**
         * \brief 注册缩放变更回调
         * \param [in] component : 回调所属组件
         * \param [in] callback : 回调函数
         */
        void addScalingChangedCallback(Component *component, const ScalingChangedCallback &callback);

        /**
         * \brief 移除缩放变更回调
         * \param [in] component : 先前注册的组件
         */
        void removeScalingChangedCallback(Component *component);
        
        /**
         * \brief DFS 遍历子树并输出层次结构文本
         * \param [in] outputLog : 是否写入 DEBUG 日志
         * \param [in] verbose : 是否输出组件列表与局部 TRS
         * \return 层次结构字符串
         */
        String printHierarchy(bool outputLog, bool verbose = false) override;

        // void debugInfo(int32_t tab = 0);
        
    protected:
        /// 默认构造
        Transform3D() = default;
        
        /**
         * \brief 以指定 UUID 构造并初始化 mWorldTransform
         * \param [in] uuid : 组件唯一标识
         */
        Transform3D(const UUID &uuid);

        /**
         * \brief 从源 Transform3D 拷贝 TRS、世界 Transform 与脏标记
         * \param [in] src : 源组件，不可为 nullptr
         * \return 成功返回 T3D_OK；src 为空返回 T3D_ERR_INVALID_POINTER
         */
        TResult cloneProperties(const Component * const src) override;

        /**
         * \brief 挂接父节点后标记自身及子节点脏
         * \param [in] parent : 新的父 TransformNode
         */
        void onAttachParent(TransformNode *parent) override;

        /**
         * \brief 脱离父节点回调（当前无额外逻辑）
         * \param [in] parent : 原父 TransformNode
         */
        void onDetachParent(TransformNode *parent) override;
        
        /**
         * \brief 销毁时调用 TransformNode::onDestroy
         */
        void onDestroy() override;

        /**
         * \brief 通知所有已注册的位置变更回调
         * \param [in] oldPos : 变更前位置
         * \param [in] newPos : 变更后位置
         */
        void onPositionChanged(const Vector3 &oldPos, const Vector3 &newPos);

        /**
         * \brief 通知所有已注册的朝向变更回调
         * \param [in] oldOrient : 变更前朝向
         * \param [in] newOrient : 变更后朝向
         */
        void onOrientationChanged(const Quaternion &oldOrient, const Quaternion &newOrient);

        /**
         * \brief 通知所有已注册的缩放变更回调
         * \param [in] oldScaling : 变更前缩放
         * \param [in] newScaling : 变更后缩放
         */
        void onScalingChanged(const Vector3 &oldScaling, const Vector3 &newScaling);

    private:
        /// 父节点坐标系下的局部位置
        Vector3             mPosition {};
        /// 父节点坐标系下的局部朝向
        Quaternion          mOrientation {};
        /// 父节点坐标系下的局部缩放
        Vector3             mScaling {REAL_ONE, REAL_ONE, REAL_ONE};

        /// 缓存的局部到世界 Transform
        mutable Transform   mWorldTransform {};
        
        /// 世界 Transform 是否需要重新计算
        mutable bool        mIsDirty {true};

        using PositionCallbacks = TUnorderedMap<Component*, PositionChangedCallback>;
        /// 位置变更回调表
        PositionCallbacks       mPositionCallbacks {};
        
        using OrientationCallbacks = TUnorderedMap<Component*, OrientationChangedCallback>;
        /// 朝向变更回调表
        OrientationCallbacks    mOrientationCallbacks {};
        
        using ScalingCallbacks = TUnorderedMap<Component*, ScalingChangedCallback>;
        /// 缩放变更回调表
        ScalingCallbacks        mScalingCallbacks {};
    };
}


#include "T3DTransform3D.inl"


#endif    /*__T3D_TRANSFORM3D_H__*/
