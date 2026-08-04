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


#ifndef __T3D_CAPSULE_BOUND_H__
#define __T3D_CAPSULE_BOUND_H__


#include "Bound/T3DBound.h"


namespace Tiny3D
{
    /**
     * \brief 胶囊体碰撞体组件
     * \remarks 局部空间原始胶囊体经 Transform 每帧变换端点并按最大缩放分量缩放半径，相交检测使用世界空间 mCapsule
     */
    TCLASS()
    class T3D_ENGINE_API CapsuleBound : public Bound
    {
        TRTTI_ENABLE(Bound)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建默认 CapsuleBound 实例
         * \return 新建的 CapsuleBound 智能指针
         */
        static CapsuleBoundPtr create();

        ~CapsuleBound() override;

        /**
         * \brief 返回碰撞体类型
         * \return Type::CAPSULE
         */
        TFUNCTION()
        Type getType() const override;

        /**
         * \brief 克隆本组件
         * \return 复制属性成功返回新 CapsuleBound；cloneProperties 失败返回 nullptr
         */
        ComponentPtr clone() const override;

        /**
         * \brief 以线段两端点与半径设置局部空间胶囊体
         * \param [in] p0 : 线段端点 0
         * \param [in] p1 : 线段端点 1
         * \param [in] radius : 胶囊半径
         */
        TFUNCTION()
        void setParams(const Vector3 &p0, const Vector3 &p1, Real radius)
        {
            setOriginalCapsule(Capsule(p0, p1, radius));
        }

        /**
         * \brief 以中心、圆柱半高、半径与轴向设置局部空间胶囊体
         * \param [in] center : 胶囊中心
         * \param [in] cylinderHalfHeight : 圆柱段半高
         * \param [in] radius : 胶囊半径
         * \param [in] axis : 胶囊轴向
         */
        TFUNCTION()
        void setParams(const Vector3 &center, Real cylinderHalfHeight,
            Real radius, const Vector3 &axis)
        {
            setOriginalCapsule(Capsule(center, cylinderHalfHeight, radius, axis));
        }

        /// 世界空间胶囊体，由 update() 每帧重算，不参与反射与序列化
        const Capsule &getCapsule() const { return mCapsule; }

        /// 局部空间原始胶囊体，作为编辑与序列化数据源
        const Capsule &getOriginalCapsule() const { return mOriginalCapsule; }

        /**
         * \brief 返回局部空间胶囊线段端点 0
         * \return 只读引用，数据来自 mOriginalCapsule
         * \remarks Capsule 类型未注册 RTTR，故拆分为 Vector3/Real 基础类型供 Inspector 使用
         */
        TPROPERTY(RTTRFuncName="Point0", RTTRFuncType="getter", "Description"="Capsule segment endpoint 0 in local space")
        const Vector3 &getPoint0() const { return mOriginalCapsule.getPoint0(); }

        /**
         * \brief 返回局部空间胶囊线段端点 1
         * \return 只读引用，数据来自 mOriginalCapsule
         */
        TPROPERTY(RTTRFuncName="Point1", RTTRFuncType="getter", "Description"="Capsule segment endpoint 1 in local space")
        const Vector3 &getPoint1() const { return mOriginalCapsule.getPoint1(); }

        /// 返回局部空间胶囊半径
        TPROPERTY(RTTRFuncName="Radius", RTTRFuncType="getter", "Description"="Capsule radius in local space")
        Real getRadius() const { return mOriginalCapsule.getRadius(); }

        /**
         * \brief 获取关联 Renderable
         * \return 当前实现恒为 nullptr
         */
        Renderable *getRenderable() override;

    protected:
        CapsuleBound() = default;

        /**
         * \brief 以指定 UUID 构造
         * \param [in] uuid : 组件实例 UUID
         */
        CapsuleBound(const UUID &uuid);

        /**
         * \brief 检测世界空间胶囊体与球体是否相交
         * \param [in] sphere : 世界空间球体
         * \return 使用 IntrCapsuleSphere 判定，相交返回 true
         */
        bool testSphere(const Sphere &sphere) const override;

        /**
         * \brief 检测世界空间胶囊体与 AABB 是否相交
         * \param [in] aabb : 世界空间 AABB
         * \return 使用 IntrCapsuleAabb 判定，相交返回 true
         */
        bool testAabb(const Aabb &aabb) const override;

        /**
         * \brief 检测世界空间胶囊体与 OBB 是否相交
         * \param [in] obb : 世界空间 OBB
         * \return 使用 IntrCapsuleObb 判定，相交返回 true
         */
        bool testObb(const Obb &obb) const override;

        /**
         * \brief 检测两个世界空间胶囊体是否相交
         * \param [in] capsule : 对方世界空间胶囊体
         * \return 使用 IntrCapsuleCapsule 判定，相交返回 true
         */
        bool testCapsule(const Capsule &capsule) const override;

        /**
         * \brief 检测世界空间胶囊体与视锥体是否相交
         * \param [in] frustum : 世界空间视锥体
         * \return 使用 IntrFrustumCapsule 判定，相交返回 true
         */
        bool testFrustum(const Frustum &frustum) const override;

        /**
         * \brief 复制 CapsuleBound 特有属性
         * \param [in] src : 源 Component，须为 CapsuleBound
         * \return 基类成功后复制 mOriginalCapsule 与 mCapsule
         */
        TResult cloneProperties(const Component * const src) override;

        /**
         * \brief 根据局部到世界变换重算世界空间 mCapsule
         * \remarks 端点经仿射矩阵变换；半径乘以缩放三轴分量的最大值
         */
        void update();

        /**
         * \brief 每帧更新回调，调用 update()
         */
        void onUpdate() override;

    private:
        /**
         * \brief 设置局部空间原始胶囊体并同步世界空间副本
         * \param [in] capsule : 新的局部空间胶囊体
         * \remarks 在下一帧 update() 运行前，先将 mCapsule 设为相同值，避免剔除逻辑读到未初始化的世界胶囊体
         */
        void setOriginalCapsule(const Capsule &capsule)
        {
            mOriginalCapsule = capsule;
            // update() 要等到下一帧才跑，这里先同步一份，避免剔除逻辑读到未初始化的世界胶囊体
            mCapsule = capsule;
        }

        /**
         * \brief 设置局部空间线段端点 0，保留端点 1 与半径
         * \param [in] p0 : 新的端点 0
         */
        TPROPERTY(RTTRFuncName="Point0", RTTRFuncType="setter", "Description"="Capsule segment endpoint 0 in local space")
        void setPoint0(const Vector3 &p0)
        {
            setOriginalCapsule(Capsule(p0, mOriginalCapsule.getPoint1(),
                mOriginalCapsule.getRadius()));
        }

        /**
         * \brief 设置局部空间线段端点 1，保留端点 0 与半径
         * \param [in] p1 : 新的端点 1
         */
        TPROPERTY(RTTRFuncName="Point1", RTTRFuncType="setter", "Description"="Capsule segment endpoint 1 in local space")
        void setPoint1(const Vector3 &p1)
        {
            setOriginalCapsule(Capsule(mOriginalCapsule.getPoint0(), p1,
                mOriginalCapsule.getRadius()));
        }

        /**
         * \brief 设置局部空间胶囊半径，保留两端点
         * \param [in] radius : 新的半径
         */
        TPROPERTY(RTTRFuncName="Radius", RTTRFuncType="setter", "Description"="Capsule radius in local space")
        void setRadius(Real radius)
        {
            setOriginalCapsule(Capsule(mOriginalCapsule.getPoint0(),
                mOriginalCapsule.getPoint1(), radius));
        }

    private:
        /// 局部空间原始胶囊体，作为编辑与序列化数据源
        Capsule mOriginalCapsule {};
        /// 世界空间胶囊体，由 update() 每帧重算，不参与序列化
        Capsule mCapsule {};
    };
}

#endif  /*__T3D_CAPSULE_BOUND_H__*/
