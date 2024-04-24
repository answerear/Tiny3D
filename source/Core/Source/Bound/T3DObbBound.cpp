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


#include "Bound/T3DObbBound.h"
#include "Component/T3DTransform3D.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ObbBoundPtr ObbBound::create()
    {
        return new ObbBound();
    }

    //--------------------------------------------------------------------------

    ObbBound::~ObbBound()
    {

    }

    //--------------------------------------------------------------------------

    Bound::Type ObbBound::getType() const
    {
        return Type::OBB;
    }

    //--------------------------------------------------------------------------

    ComponentPtr ObbBound::clone() const
    {
        ObbBoundPtr newObj = new ObbBound();
        if (T3D_FAILED(newObj->cloneProperties(this)))
        {
            newObj = nullptr;
        }
        return newObj;
    }

    //--------------------------------------------------------------------------

    TResult ObbBound::cloneProperties(const Component * const src)
    {
        TResult ret = Bound::cloneProperties(src);

        if (ret == T3D_OK)
        {
            const ObbBound *const newBound = static_cast<const ObbBound* const>(src);
            mObb = newBound->mObb;
            mOriginalObb = newBound->mOriginalObb;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    Renderable *ObbBound::getRenderable()
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    bool ObbBound::testSphere(const Sphere &sphere) const
    {
        IntrSphereObb intr(&sphere, &mObb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool ObbBound::testAabb(const Aabb &aabb) const
    {
        IntrAabbObb intr(&aabb, &mObb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool ObbBound::testObb(const Obb &obb) const
    {
        IntrObbObb intr(&mObb, &obb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool ObbBound::testFrustum(const Frustum &frustum) const
    {
        IntrFrustumObb intr(&frustum, &mObb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    void ObbBound::update()
    {
        GameObject *go = getGameObject();
        T3D_ASSERT(go != nullptr, "Game object must be not nullptr !");
        Transform3D *xform = go->getComponent<Transform3D>();
        if (xform != nullptr)
        {
            const Transform &transform = xform->getLocalToWorldTransform();

            // 变换中点
            Vector3 center = transform.getAffineMatrix() * mOriginalObb.getCenter();

            // 变换3个轴
            Vector3 axis0 = transform.getAffineMatrix() * (mOriginalObb.getAxis(0) * mOriginalObb.getExtent(0));
            Vector3 axis1 = transform.getAffineMatrix() * (mOriginalObb.getAxis(1) * mOriginalObb.getExtent(1));
            Vector3 axis2 = transform.getAffineMatrix() * (mOriginalObb.getAxis(2) * mOriginalObb.getExtent(2));

            // 计算三个轴上的长度
            Real extent0 = axis0.length();
            Real extent1 = axis1.length();
            Real extent2 = axis2.length();

            axis0 /= extent0;
            axis1 /= extent1;
            axis2 /= extent2;

            mObb.setCenter(center);
            mObb.setAxis(axis0, axis1, axis2);
            mObb.setExtent(0, extent0);
            mObb.setExtent(1, extent1);
            mObb.setExtent(2, extent2);
        }
    }

    //--------------------------------------------------------------------------

    void ObbBound::onUpdate()
    {
        update();
    }

    //--------------------------------------------------------------------------
}


