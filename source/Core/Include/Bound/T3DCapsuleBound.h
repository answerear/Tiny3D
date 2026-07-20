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
            mOriginalCapsule = Capsule(p0, p1, radius);
            mCapsule = mOriginalCapsule;
        }

        TFUNCTION()
        void setParams(const Vector3 &center, Real cylinderHalfHeight,
            Real radius, const Vector3 &axis)
        {
            mOriginalCapsule = Capsule(center, cylinderHalfHeight, radius, axis);
            mCapsule = mOriginalCapsule;
        }

        TPROPERTY(RTTRFuncName="capsule", RTTRFuncType="getter", "Description"="Capsule")
        const Capsule &getCapsule() const { return mCapsule; }

        TPROPERTY(RTTRFuncName="originalCapsule", RTTRFuncType="getter", "Description"="OriginalCapsule")
        const Capsule &getOriginalCapsule() const { return mOriginalCapsule; }

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
        TPROPERTY(RTTRFuncName="capsule", RTTRFuncType="setter", "Description"="Capsule")
        void setCapsule(const Capsule &capsule) { mCapsule = capsule; }

        TPROPERTY(RTTRFuncName="originalCapsule", RTTRFuncType="setter", "Description"="OriginalCapsule")
        void setOriginalCapsule(const Capsule &capsule) { mOriginalCapsule = capsule; }

    private:
        /// 原始胶囊体（局部）
        Capsule mOriginalCapsule {};
        /// 用于快速检测相交性的胶囊体（世界）
        Capsule mCapsule {};
    };
}

#endif  /*__T3D_CAPSULE_BOUND_H__*/
