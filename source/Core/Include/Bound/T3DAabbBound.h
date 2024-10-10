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


#ifndef __T3D_AABB_BOUND_H__
#define __T3D_AABB_BOUND_H__


#include "Bound/T3DBound.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API AabbBound : public Bound
    {
         TRTTI_ENABLE(Bound)
         TRTTI_FRIEND

    public:
        static AabbBoundPtr create();

        ~AabbBound() override;

        TFUNCTION("Description"="Bound Type")
        Type getType() const override;

        ComponentPtr clone() const override;

        TPROPERTY(RTTRFuncName="AABB", RTTRFuncType="getter", "Description"="Aligned Axis Bounding Box")
        const Aabb &getAabb() const { return mAabb; }

        TPROPERTY(RTTRFuncName="originalAABB", RTTRFuncType="getter", "Description"="Original Aligned Axis Bounding Box")
        const Aabb& getOriginalAabb() const { return mOriginalAabb; }

        TFUNCTION()
        void setParams(Real minX, Real maxX, Real minY, Real maxY, Real minZ, Real maxZ)
        {
            mOriginalAabb.setParam(Vector3(minX, minY, minZ), Vector3(maxX, maxY, maxZ));
        }

        Renderable *getRenderable() override;
        
    protected:
        AabbBound() = default;

        AabbBound(const UUID &uuid);

        bool testSphere(const Sphere &sphere) const override;

        bool testAabb(const Aabb &aabb) const override;

        bool testObb(const Obb &obb) const override;

        bool testFrustum(const Frustum &frustum) const override;

        TResult cloneProperties(const Component * const src) override;
        
        void update();

        void onUpdate() override;

    private:
        TPROPERTY(RTTRFuncName="AABB", RTTRFuncType="setter", "Description"="Aligned Axis Bounding Box")
        void setAabb(const Aabb &aabb) { mAabb = aabb; }

        TPROPERTY(RTTRFuncName="originalAABB", RTTRFuncType="setter", "Description"="Original Aligned Axis Bounding Box")
        void setOriginalAabb(const Aabb &aabb) { mOriginalAabb = aabb; }

    private:
        /// 实时变换的AABB
        Aabb    mAabb {};
        /// 不参与变换的原始AABB
        Aabb    mOriginalAabb {};
    };
}

#endif  /*__T3D_AABB_BOUND_H__*/
