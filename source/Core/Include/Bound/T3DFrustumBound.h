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


#ifndef __T3D_FRUSTUM_BOUND_H__
#define __T3D_FRUSTUM_BOUND_H__


#include "Bound/T3DBound.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API FrustumBound : public Bound
    {
        TRTTI_ENABLE(Bound)
        TRTTI_FRIEND

    public:
        static FrustumBoundPtr create();

        ~FrustumBound() override;

        TFUNCTION("Description"="Bound Type")
        Type getType() const override;

        ComponentPtr clone() const override;

        TPROPERTY(RTTRFuncName="frustum", RTTRFuncType="getter", "Description"="Frustum")
        const Frustum &getFrustum() const { return mFrustum; }

        TPROPERTY(RTTRFuncName="originalFrustum", RTTRFuncType="getter", "Description"="Original Frustum")
        const Frustum& getOriginalFrustum() const { return mOriginalFrustum; }

        TFUNCTION()
        void setFrustumFace(Face face, const Plane &plane);

        TFUNCTION()
        void setFrustumFaces(Plane *plane, size_t planeCount);

        Renderable *getRenderable() override;

    protected:
        FrustumBound() = default;

        bool testSphere(const Sphere &sphere) const override;

        bool testAabb(const Aabb &aabb) const override;

        bool testObb(const Obb &obb) const override;

        bool testFrustum(const Frustum &frustum) const override;

        TResult cloneProperties(const Component * const src) override;

        void update();

        void onStart() override;

        void onUpdate() override;

        void onDestroy() override;

    private:
        TPROPERTY(RTTRFuncName="frustum", RTTRFuncType="setter", "Description"="Frustum")
        void setFrustum(const Frustum &frustum) { mFrustum = frustum; }

        TPROPERTY(RTTRFuncName="originalFrustum", RTTRFuncType="setter", "Description"="Original Frustum")
        void setOriginalFrustum(const Frustum &frustum) { mOriginalFrustum = frustum; }

    private:
        /// 可变换的视锥体对象
        Frustum mFrustum {};
        /// 不参与变换的原始视锥体
        Frustum mOriginalFrustum {};

        Transform3DPtr  mXformNode {nullptr};
    };
}


#endif  /*__T3D_FRUSTUM_BOUND_H__*/
