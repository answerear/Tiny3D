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


#ifndef __T3D_OBB_BOUND_H__
#define __T3D_OBB_BOUND_H__


#include "Bound/T3DBound.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API ObbBound : public Bound
    {
        TRTTI_ENABLE(Bound)
        TRTTI_FRIEND

    public:
        static ObbBoundPtr create();

        ~ObbBound() override;

        TFUNCTION("Description"="Bound Type")
        Type getType() const override;

        ComponentPtr clone() const override;

        TPROPERTY(RTTRFuncName="OBB", RTTRFuncType="getter", "Description"="Oriented Bounding Box")
        const Obb &getObb() const { return mObb; }

        TPROPERTY(RTTRFuncName="originalOBB", RTTRFuncType="getter", "Description"="Original Oriented Bounding Box")
        const Obb& getOriginalObb() const { return mOriginalObb; }

        TFUNCTION()
        void setCenter(const Vector3 &center)
        {
            mObb.setCenter(center);
            mOriginalObb.setCenter(center);
        }

        TFUNCTION()
        void setAxis(const Vector3 &axis0, const Vector3 &axis1, const Vector3 &axis2)
        {
            mObb.setAxis(axis0, axis1, axis2);
            mOriginalObb.setAxis(axis0, axis1, axis2);
        }

        TFUNCTION()
        void setExtent(Real extent0, Real extent1, Real extent2)
        {
            mObb.setExtent(0, extent0);
            mObb.setExtent(1, extent1);
            mObb.setExtent(2, extent2);

            mOriginalObb.setExtent(0, extent0);
            mOriginalObb.setExtent(1, extent1);
            mOriginalObb.setExtent(2, extent2);
        }

        Renderable *getRenderable() override;

    protected:
        ObbBound() = default;

        bool testSphere(const Sphere &sphere) const override;

        bool testAabb(const Aabb &aabb) const override;

        bool testObb(const Obb &obb) const override;

        bool testFrustum(const Frustum &frustum) const override;

        TResult cloneProperties(const Component * const src) override;
        
        void update();

        void onUpdate() override;

    private:
        TPROPERTY(RTTRFuncName="OBB", RTTRFuncType="setter", "Description"="Oriented Bounding Box")
        void setObb(const Obb &obb) { mObb = obb; }

        TPROPERTY(RTTRFuncName="originalOBB", RTTRFuncType="setter", "Description"="Original Oriented Bounding Box")
        void setOriginalObb(const Obb &obb) { mOriginalObb = obb; }
        
    private:
        /// 实时变换的OBB
        Obb mObb {};
        /// 不参与变换的原始OBB
        Obb mOriginalObb {};
    };
}


#endif  /*__T3D_OBB_BOUND_H__*/
