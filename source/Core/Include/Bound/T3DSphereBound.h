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


#ifndef __T3D_SPHERE_BOUND_H__
#define __T3D_SPHERE_BOUND_H__


#include "Bound/T3DBound.h"


namespace Tiny3D
{
    /**
     * \brief 球体碰撞体组件
     * \remarks 局部空间原始球体经 Transform 每帧变换球心并按最大缩放分量缩放半径，相交检测使用世界空间 mSphere
     */
    TCLASS()
    class T3D_ENGINE_API SphereBound : public Bound
    {
        TRTTI_ENABLE(Bound)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建默认 SphereBound 实例
         * \return 新建的 SphereBound 智能指针
         */
        static SphereBoundPtr create();

        ~SphereBound() override;

        /**
         * \brief 返回碰撞体类型
         * \return Type::SPHERE
         */
        TFUNCTION()
        Type getType() const override;

        /**
         * \brief 克隆本组件
         * \return 复制属性成功返回新 SphereBound；cloneProperties 失败返回 nullptr
         */
        ComponentPtr clone() const override;

        /**
         * \brief 以中心与半径设置局部空间球体
         * \param [in] center : 球心
         * \param [in] radius : 半径
         */
        TFUNCTION()
        void setParams(const Vector3 &center, Real radius)
        {
            Sphere sphere;
            sphere.setCenter(center);
            sphere.setRadius(radius);
            setOriginalSphere(sphere);
        }

        /// 世界空间球体，由 update() 每帧重算，不参与反射与序列化
        const Sphere &getSphere() const { return mSphere; }

        /// 局部空间原始球体，作为编辑与序列化数据源
        const Sphere& getOriginalSphere() const { return mOriginalSphere; }

        /**
         * \brief 返回局部空间球心
         * \return 只读引用，数据来自 mOriginalSphere
         * \remarks 拆分为 Center/Radius 以匹配 Inspector 平铺展示，避免整体反射 Sphere 产生可折叠子结构
         */
        TPROPERTY(RTTRFuncName="Center", RTTRFuncType="getter", "Description"="Sphere center in local space")
        const Vector3 &getCenter() const { return mOriginalSphere.getCenter(); }

        /// 返回局部空间球半径
        TPROPERTY(RTTRFuncName="Radius", RTTRFuncType="getter", "Description"="Sphere radius in local space")
        Real getRadius() const { return mOriginalSphere.getRadius(); }

        /**
         * \brief 获取关联 Renderable
         * \return 当前实现恒为 nullptr
         */
        Renderable *getRenderable() override;

    protected:
        SphereBound() = default;

        /**
         * \brief 以指定 UUID 构造
         * \param [in] uuid : 组件实例 UUID
         */
        SphereBound(const UUID &uuid);

        /**
         * \brief 检测两个世界空间球体是否相交
         * \param [in] sphere : 对方世界空间球体
         * \return 使用 IntrSphereSphere 判定，相交返回 true
         */
        bool testSphere(const Sphere &sphere) const override;

        /**
         * \brief 检测世界空间球体与 AABB 是否相交
         * \param [in] aabb : 世界空间 AABB
         * \return 使用 IntrSphereAabb 判定，相交返回 true
         */
        bool testAabb(const Aabb &aabb) const override; 

        /**
         * \brief 检测世界空间球体与 OBB 是否相交
         * \param [in] obb : 世界空间 OBB
         * \return 使用 IntrSphereObb 判定，相交返回 true
         */
        bool testObb(const Obb &obb) const override;

        /**
         * \brief 检测世界空间球体与胶囊体是否相交
         * \param [in] capsule : 世界空间胶囊体
         * \return 使用 IntrCapsuleSphere 判定，相交返回 true
         */
        bool testCapsule(const Capsule &capsule) const override;

        /**
         * \brief 检测世界空间球体与视锥体是否相交
         * \param [in] frustum : 世界空间视锥体
         * \return 使用 IntrFrustumSphere 判定，相交返回 true
         */
        bool testFrustum(const Frustum &frustum) const override;

        /**
         * \brief 复制 SphereBound 特有属性
         * \param [in] src : 源 Component，须为 SphereBound
         * \return 基类成功后复制 mOriginalSphere 与 mSphere
         */
        TResult cloneProperties(const Component * const src) override;
        
        /**
         * \brief 根据局部到世界变换重算世界空间 mSphere
         * \remarks 球心经仿射矩阵变换；半径乘以缩放三轴分量的最大值；不回写 mOriginalSphere 以免累积漂移
         */
        void update();

        /**
         * \brief 每帧更新回调，调用 update()
         */
        void onUpdate() override;

    private:
        /**
         * \brief 设置局部空间原始球体并同步世界空间副本
         * \param [in] sphere : 新的局部空间球体
         * \remarks 在下一帧 update() 运行前，先将 mSphere 设为相同值，避免剔除逻辑读到未初始化的世界球体
         */
        void setOriginalSphere(const Sphere &sphere)
        {
            mOriginalSphere = sphere;
            // update() 要等到下一帧才跑，这里先同步一份，避免剔除逻辑读到未初始化的世界球体
            mSphere = sphere;
        }

        /**
         * \brief 设置局部空间球心，保留当前半径
         * \param [in] center : 新的球心
         */
        TPROPERTY(RTTRFuncName="Center", RTTRFuncType="setter", "Description"="Sphere center in local space")
        void setCenter(const Vector3 &center)
        {
            setParams(center, mOriginalSphere.getRadius());
        }

        /**
         * \brief 设置局部空间半径，保留当前球心
         * \param [in] radius : 新的半径
         */
        TPROPERTY(RTTRFuncName="Radius", RTTRFuncType="setter", "Description"="Sphere radius in local space")
        void setRadius(Real radius)
        {
            setParams(mOriginalSphere.getCenter(), radius);
        }

    private:
        /// 局部空间原始球体，作为编辑与序列化数据源
        Sphere    mOriginalSphere {};
        /// 世界空间球体，由 update() 每帧重算，不参与序列化
        Sphere    mSphere {};
    };
}

#endif  /*__T3D_SPHERE_BOUND_H__*/
