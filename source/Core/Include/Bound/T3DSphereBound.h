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
    TCLASS()
    class T3D_ENGINE_API SphereBound : public Bound
    {
        TRTTI_ENABLE(Bound)
        TRTTI_FRIEND

    public:
        static SphereBoundPtr create();

        ~SphereBound() override;

        TFUNCTION()
        Type getType() const override;

        ComponentPtr clone() const override;

        TFUNCTION()
        void setParams(const Vector3 &center, Real radius)
        {
            Sphere sphere;
            sphere.setCenter(center);
            sphere.setRadius(radius);
            setOriginalSphere(sphere);
        }

        /// 世界空间球体，每帧由原始球体与变换矩阵重算，不反射也不序列化
        const Sphere &getSphere() const { return mSphere; }

        /// 局部空间球体
        const Sphere& getOriginalSphere() const { return mOriginalSphere; }

        // 整体反射 Sphere 会在 inspector 里多出一层可折叠子结构，与其余 bound 呈现不一致，
        // 因此拆成 Center / Radius 直接平铺在组件面板上
        TPROPERTY(RTTRFuncName="Center", RTTRFuncType="getter", "Description"="Sphere center in local space")
        const Vector3 &getCenter() const { return mOriginalSphere.getCenter(); }

        TPROPERTY(RTTRFuncName="Radius", RTTRFuncType="getter", "Description"="Sphere radius in local space")
        Real getRadius() const { return mOriginalSphere.getRadius(); }

        Renderable *getRenderable() override;

    protected:
        SphereBound() = default;

        SphereBound(const UUID &uuid);

        bool testSphere(const Sphere &sphere) const override;

        bool testAabb(const Aabb &aabb) const override; 

        bool testObb(const Obb &obb) const override;

        bool testCapsule(const Capsule &capsule) const override;

        bool testFrustum(const Frustum &frustum) const override;

        TResult cloneProperties(const Component * const src) override;
        
        void update();

        void onUpdate() override;

    private:
        void setOriginalSphere(const Sphere &sphere)
        {
            mOriginalSphere = sphere;
            // update() 要等到下一帧才跑，这里先同步一份，避免剔除逻辑读到未初始化的世界球体
            mSphere = sphere;
        }

        TPROPERTY(RTTRFuncName="Center", RTTRFuncType="setter", "Description"="Sphere center in local space")
        void setCenter(const Vector3 &center)
        {
            setParams(center, mOriginalSphere.getRadius());
        }

        TPROPERTY(RTTRFuncName="Radius", RTTRFuncType="setter", "Description"="Sphere radius in local space")
        void setRadius(Real radius)
        {
            setParams(mOriginalSphere.getCenter(), radius);
        }

    private:
        /// 原始球体（局部空间）
        Sphere    mOriginalSphere {};
        /// 用于快速检测相交性的球体（世界空间），由 update() 每帧重算，不参与序列化
        Sphere    mSphere {};
    };
}

#endif  /*__T3D_SPHERE_BOUND_H__*/
