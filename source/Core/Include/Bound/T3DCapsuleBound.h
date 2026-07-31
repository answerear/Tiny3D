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
    TCLASS()
    class T3D_ENGINE_API CapsuleBound : public Bound
    {
        TRTTI_ENABLE(Bound)
        TRTTI_FRIEND

    public:
        static CapsuleBoundPtr create();

        ~CapsuleBound() override;

        TFUNCTION()
        Type getType() const override;

        ComponentPtr clone() const override;

        TFUNCTION()
        void setParams(const Vector3 &p0, const Vector3 &p1, Real radius)
        {
            setOriginalCapsule(Capsule(p0, p1, radius));
        }

        TFUNCTION()
        void setParams(const Vector3 &center, Real cylinderHalfHeight,
            Real radius, const Vector3 &axis)
        {
            setOriginalCapsule(Capsule(center, cylinderHalfHeight, radius, axis));
        }

        /// 世界空间胶囊体，每帧由原始胶囊体与变换矩阵重算，不反射也不序列化
        const Capsule &getCapsule() const { return mCapsule; }

        /// 局部空间胶囊体
        const Capsule &getOriginalCapsule() const { return mOriginalCapsule; }

        // Capsule（TCapsule<Real>）没有生成 RTTR 注册，整体反射出去 inspector 画不出
        // 也存不下，因此拆成 Vector3 / Real 这些已注册的基础类型逐项暴露
        TPROPERTY(RTTRFuncName="Point0", RTTRFuncType="getter", "Description"="Capsule segment endpoint 0 in local space")
        const Vector3 &getPoint0() const { return mOriginalCapsule.getPoint0(); }

        TPROPERTY(RTTRFuncName="Point1", RTTRFuncType="getter", "Description"="Capsule segment endpoint 1 in local space")
        const Vector3 &getPoint1() const { return mOriginalCapsule.getPoint1(); }

        TPROPERTY(RTTRFuncName="Radius", RTTRFuncType="getter", "Description"="Capsule radius in local space")
        Real getRadius() const { return mOriginalCapsule.getRadius(); }

        Renderable *getRenderable() override;

    protected:
        CapsuleBound() = default;

        CapsuleBound(const UUID &uuid);

        bool testSphere(const Sphere &sphere) const override;

        bool testAabb(const Aabb &aabb) const override;

        bool testObb(const Obb &obb) const override;

        bool testCapsule(const Capsule &capsule) const override;

        bool testFrustum(const Frustum &frustum) const override;

        TResult cloneProperties(const Component * const src) override;

        void update();

        void onUpdate() override;

    private:
        void setOriginalCapsule(const Capsule &capsule)
        {
            mOriginalCapsule = capsule;
            // update() 要等到下一帧才跑，这里先同步一份，避免剔除逻辑读到未初始化的世界胶囊体
            mCapsule = capsule;
        }

        TPROPERTY(RTTRFuncName="Point0", RTTRFuncType="setter", "Description"="Capsule segment endpoint 0 in local space")
        void setPoint0(const Vector3 &p0)
        {
            setOriginalCapsule(Capsule(p0, mOriginalCapsule.getPoint1(),
                mOriginalCapsule.getRadius()));
        }

        TPROPERTY(RTTRFuncName="Point1", RTTRFuncType="setter", "Description"="Capsule segment endpoint 1 in local space")
        void setPoint1(const Vector3 &p1)
        {
            setOriginalCapsule(Capsule(mOriginalCapsule.getPoint0(), p1,
                mOriginalCapsule.getRadius()));
        }

        TPROPERTY(RTTRFuncName="Radius", RTTRFuncType="setter", "Description"="Capsule radius in local space")
        void setRadius(Real radius)
        {
            setOriginalCapsule(Capsule(mOriginalCapsule.getPoint0(),
                mOriginalCapsule.getPoint1(), radius));
        }

    private:
        /// 原始胶囊体（局部）
        Capsule mOriginalCapsule {};
        /// 用于快速检测相交性的胶囊体（世界），由 update() 每帧重算，不参与序列化
        Capsule mCapsule {};
    };
}

#endif  /*__T3D_CAPSULE_BOUND_H__*/
