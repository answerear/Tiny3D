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


#include "Bound/T3DCapsuleBound.h"
#include "Component/T3DTransform3D.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    CapsuleBoundPtr CapsuleBound::create()
    {
        return T3D_NEW CapsuleBound();
    }

    //--------------------------------------------------------------------------

    CapsuleBound::CapsuleBound(const UUID &uuid)
        : Bound(uuid)
    {
    }

    //--------------------------------------------------------------------------

    CapsuleBound::~CapsuleBound()
    {
    }

    //--------------------------------------------------------------------------

    Bound::Type CapsuleBound::getType() const
    {
        return Type::CAPSULE;
    }

    //--------------------------------------------------------------------------

    ComponentPtr CapsuleBound::clone() const
    {
        CapsuleBoundPtr newObj = T3D_NEW CapsuleBound();
        if (T3D_FAILED(newObj->cloneProperties(this)))
        {
            newObj = nullptr;
        }
        return newObj;
    }

    //--------------------------------------------------------------------------

    TResult CapsuleBound::cloneProperties(const Component * const src)
    {
        TResult ret = Bound::cloneProperties(src);

        if (ret == T3D_OK)
        {
            const CapsuleBound * const capsuleBound
                = static_cast<const CapsuleBound * const>(src);
            mOriginalCapsule = capsuleBound->mOriginalCapsule;
            mCapsule = capsuleBound->mCapsule;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    Renderable *CapsuleBound::getRenderable()
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    bool CapsuleBound::testSphere(const Sphere &sphere) const
    {
        IntrCapsuleSphere intr(&mCapsule, &sphere);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool CapsuleBound::testAabb(const Aabb &aabb) const
    {
        IntrCapsuleAabb intr(&mCapsule, &aabb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool CapsuleBound::testObb(const Obb &obb) const
    {
        IntrCapsuleObb intr(&mCapsule, &obb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool CapsuleBound::testCapsule(const Capsule &capsule) const
    {
        IntrCapsuleCapsule intr(&mCapsule, &capsule);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool CapsuleBound::testFrustum(const Frustum &frustum) const
    {
        IntrFrustumCapsule intr(&frustum, &mCapsule);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    void CapsuleBound::update()
    {
        GameObject *go = getGameObject();
        T3D_ASSERT(go != nullptr);
        Transform3D *xform = static_cast<Transform3D *>(go->getTransformNode());
        if (xform != nullptr)
        {
            const Transform &transform = xform->getLocalToWorldTransform();
            const Matrix4 &M = transform.getAffineMatrix();

            Vector3 p0 = M * mOriginalCapsule.getPoint0();
            Vector3 p1 = M * mOriginalCapsule.getPoint1();

            const Vector3 &S = transform.getScaling();
            Real factor = std::max(std::max(S.x(), S.y()), S.z());
            Real radius = factor * mOriginalCapsule.getRadius();

            mCapsule.setPoint0(p0);
            mCapsule.setPoint1(p1);
            mCapsule.setRadius(radius);
        }
    }

    //--------------------------------------------------------------------------

    void CapsuleBound::onUpdate()
    {
        update();
    }

    //--------------------------------------------------------------------------
}
