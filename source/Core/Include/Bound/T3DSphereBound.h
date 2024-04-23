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
            mOriginalSphere.setCenter(center);
            mOriginalSphere.setRadius(radius);

            mSphere.setCenter(center);
            mSphere.setRadius(radius);
        }

        TPROPERTY(RTTRFuncName="sphere", RTTRFuncType="getter", "Description"="Sphere")
        const Sphere &getSphere() const { return mSphere; }

        TPROPERTY(RTTRFuncName="originalSphere", RTTRFuncType="getter", "Description"="OriginalSphere")
        const Sphere& getOriginalSphere() const { return mOriginalSphere; }

        Renderable *getRenderable() override;

    protected:
        SphereBound() = default;

        bool testSphere(const Sphere &sphere) const override;

        bool testAabb(const Aabb &aabb) const override; 

        bool testObb(const Obb &obb) const override;

        bool testFrustum(const Frustum &frustum) const override;

        TResult cloneProperties(const Component * const src) override;

        void onPositionChanged(const Vector3 &oldPos, const Vector3 &newPos) override;

        void onOrientationChanged(const Quaternion &oldOri, const Quaternion &newOri) override;

        void onScalingChanged(const Vector3 &oldScaling, const Vector3 &newScaling) override;

        void update();

    private:
        TPROPERTY(RTTRFuncName="sphere", RTTRFuncType="setter", "Description"="Sphere")
        void setSphere(const Sphere &sphere) { mSphere = sphere; }

        TPROPERTY(RTTRFuncName="originalSphere", RTTRFuncType="setter", "Description"="OriginalSphere")
        void setOriginalSphere(const Sphere &sphere) { mOriginalSphere = sphere; }

    private:
        /// 原始球体
        Sphere    mOriginalSphere {};
        /// 用于快速检测相交性的球体
        Sphere    mSphere {};
    };
}

#endif  /*__T3D_SPHERE_BOUND_H__*/
