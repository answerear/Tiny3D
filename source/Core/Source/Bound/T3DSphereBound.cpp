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


#include "Bound/T3DSphereBound.h"
#include "Component/T3DTransform3D.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SphereBoundPtr SphereBound::create()
    {
        return new SphereBound();
    }

    //--------------------------------------------------------------------------
    
    SphereBound::~SphereBound()
    {
    }

    //--------------------------------------------------------------------------

    Bound::Type SphereBound::getType() const
    {
        return Type::SPHERE;
    }

    //--------------------------------------------------------------------------

    ComponentPtr SphereBound::clone() const
    {
        SphereBoundPtr newObj = new SphereBound();
        if (T3D_FAILED(newObj->cloneProperties(this)))
        {
            newObj = nullptr;
        }
        return newObj;
    }

    //--------------------------------------------------------------------------

    TResult SphereBound::cloneProperties(const Component * const src)
    {
        TResult ret = Bound::cloneProperties(src);

        if (ret == T3D_OK)
        {
            const SphereBound * const sphereBound = static_cast<const SphereBound* const>(src);
            mOriginalSphere = sphereBound->mOriginalSphere;
            mSphere = sphereBound->mSphere;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    Renderable *SphereBound::getRenderable()
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    bool SphereBound::testSphere(const Sphere &sphere) const
    {
        IntrSphereSphere intr(&mSphere, &sphere);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool SphereBound::testAabb(const Aabb &aabb) const
    {
        IntrSphereAabb intr(&mSphere, &aabb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool SphereBound::testObb(const Obb &obb) const
    {
        IntrSphereObb intr(&mSphere, &obb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool SphereBound::testFrustum(const Frustum &frustum) const
    {
        IntrFrustumSphere intr(&frustum, &mSphere);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    void SphereBound::update()
    {
        GameObject *go = getGameObject();
        T3D_ASSERT(go != nullptr, "Game object must be not nullptr !");
        Transform3D *xform = go->getComponent<Transform3D>();
        if (xform != nullptr)
        {
            const Transform &transform = xform->getLocalToWorldTransform();

            // 更新变换后用于碰撞检测的球体
            const Matrix4 &M = transform.getAffineMatrix();
            Vector3 center = M * mOriginalSphere.getCenter();

            const Vector3 &S = transform.getScaling();
            Real factor = std::max(std::max(S.x(), S.y()), S.z());
            Real radius = factor * mOriginalSphere.getRadius();

            mSphere.setCenter(center);
            mSphere.setRadius(radius);

            // 原始球体，只更新球心位置，不更新半径大小
            center = M * mOriginalSphere.getCenter();
            mOriginalSphere.setCenter(center);
        }
    }
    
    //--------------------------------------------------------------------------

    void SphereBound::onUpdate()
    {
        update();
    }

    //--------------------------------------------------------------------------
}
