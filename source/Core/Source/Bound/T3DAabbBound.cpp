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


#include "Bound/T3DAabbBound.h"
#include "Component/T3DGeometry.h"
#include "Component/T3DTransform3D.h"
#include "Resource/T3DMesh.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    AabbBoundPtr AabbBound::create()
    {
        return new AabbBound();
    }

    //--------------------------------------------------------------------------

    AabbBound::~AabbBound()
    {

    }

    //--------------------------------------------------------------------------

    Bound::Type AabbBound::getType() const
    {
        return Type::AABB;
    }

    //--------------------------------------------------------------------------

    ComponentPtr AabbBound::clone() const
    {
        AabbBoundPtr newObj = new AabbBound();
        if (T3D_FAILED(newObj->cloneProperties(this)))
        {
            newObj = nullptr;
        }
        return newObj;
    }

    //--------------------------------------------------------------------------

    TResult AabbBound::cloneProperties(const Component * const src) 
    {
        TResult ret = Bound::cloneProperties(src);

        if (ret == T3D_OK)
        {
            const AabbBound *const aabbBound = static_cast<const AabbBound* const>(src);
            mAabb = aabbBound->mAabb;
            mOriginalAabb = aabbBound->mOriginalAabb;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    Renderable *AabbBound::getRenderable()
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    bool AabbBound::testSphere(const Sphere &sphere) const
    {
        IntrSphereAabb intr(&sphere, &mAabb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool AabbBound::testAabb(const Aabb &aabb) const
    {
        IntrAabbAabb intr(&mAabb, &aabb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool AabbBound::testObb(const Obb &obb) const
    {
        IntrAabbObb intr(&mAabb, &obb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool AabbBound::testFrustum(const Frustum &frustum) const
    {
        IntrFrustumAabb intr(&frustum, &mAabb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    void AabbBound::update()
    {
        // 这里不用传统的变换8个顶点，然后逐个比较获取最大x,y,z来重新设置AABB
        // 这里使用快速变换的方法，减少矩阵变换带来的开销，原理如下：
        // 
        // 碰撞盒8个顶点的任意一点为P，变换后一点为P'，变换矩阵为M，则可得：
        //      P' = M * P
        // 其中
        //      P' = (x' y' z' 1)
        //
        //      P = (x y z 1)
        //
        //          | m00 m01 m02 tx|
        //      M = | m10 m11 m12 ty|
        //          | m20 m21 m22 tz|
        //          | 0   0   0   1 |
        // 则
        //      p'x' = px * m00 + py * m01 + pz * m02 + tx
        //      p'y' = px * m10 + py * m11 + pz * m12 + ty
        //      p'z' = px * m20 + py * m21 + pz * m22 + tz
        // 这里只要计算最大的p'x'、p'y'和p'z'就可以得出最大和最小值了。
        // 如此这样
        //      如 m00 > 0时，则当px = min_x时，则px * m00最小，
        //      如 m00 < 0时，则当px = max_x时，则px * m00最小
        // 同理，可以根据最小值计算方法计算最大值，并且如此类推，计算p'y'和p'z'

        GameObject *go = getGameObject();
        T3D_ASSERT(go != nullptr, "Game object must be not nullptr !");
        
        Transform3D *xform = go->getComponent<Transform3D>();
        if (xform != nullptr)
        {
            const Transform &transform = xform->getLocalToWorldTransform();
            const Vector3 &translate = transform.getTranslation();
            const Matrix4 &M = transform.getAffineMatrix();

            Vector3 vMin = translate;
            Vector3 vMax = translate;

            // Calculate min x & max x
            if (M[0][0] > Real(0.0))
            {
                vMin.x() += M[0][0] * mOriginalAabb.getMinX();
                vMax.x() += M[0][0] * mOriginalAabb.getMaxX();
            }
            else
            {
                vMin.x() += M[0][0] * mOriginalAabb.getMaxX();
                vMax.x() += M[0][0] * mOriginalAabb.getMinX();
            }

            if (M[0][1] > Real(0.0))
            {
                vMin.x() += M[0][1] * mOriginalAabb.getMinY();
                vMax.x() += M[0][1] * mOriginalAabb.getMaxY();
            }
            else
            {
                vMin.x() += M[0][1] * mOriginalAabb.getMaxY();
                vMax.x() += M[0][1] * mOriginalAabb.getMinY();
            }

            if (M[0][2] > Real(0.0))
            {
                vMin.x() += M[0][2] * mOriginalAabb.getMinZ();
                vMax.x() += M[0][2] * mOriginalAabb.getMaxZ();
            }
            else
            {
                vMin.x() += M[0][2] * mOriginalAabb.getMaxZ();
                vMax.x() += M[0][2] * mOriginalAabb.getMinZ();
            }

            // Calculate min y & max y
            if (M[1][0] > Real(0.0))
            {
                vMin.y() += M[1][0] * mOriginalAabb.getMinX();
                vMax.y() += M[1][0] * mOriginalAabb.getMaxX();
            }
            else
            {
                vMin.y() += M[1][0] * mOriginalAabb.getMaxX();
                vMax.y() += M[1][0] * mOriginalAabb.getMinX();
            }

            if (M[1][1] > Real(0.0))
            {
                vMin.y() += M[1][1] * mOriginalAabb.getMinY();
                vMax.y() += M[1][1] * mOriginalAabb.getMaxY();
            }
            else
            {
                vMin.y() += M[1][1] * mOriginalAabb.getMaxY();
                vMax.y() += M[1][1] * mOriginalAabb.getMinY();
            }

            if (M[1][2] > Real(0.0))
            {
                vMin.y() += M[1][2] * mOriginalAabb.getMinZ();
                vMax.y() += M[1][2] * mOriginalAabb.getMaxZ();
            }
            else
            {
                vMin.y() += M[1][2] * mOriginalAabb.getMaxZ();
                vMax.y() += M[1][2] * mOriginalAabb.getMinZ();
            }

            // Calculate min z & max z
            if (M[2][0] > Real(0.0))
            {
                vMin.z() += M[2][0] * mOriginalAabb.getMinX();
                vMax.z() += M[2][0] * mOriginalAabb.getMaxX();
            }
            else
            {
                vMin.z() += M[2][0] * mOriginalAabb.getMaxX();
                vMax.z() += M[2][0] * mOriginalAabb.getMinX();
            }

            if (M[2][1] > Real(0.0))
            {
                vMin.z() += M[2][1] * mOriginalAabb.getMinY();
                vMax.z() += M[2][1] * mOriginalAabb.getMaxY();
            }
            else
            {
                vMin.z() += M[2][1] * mOriginalAabb.getMaxY();
                vMax.z() += M[2][1] * mOriginalAabb.getMinY();
            }

            if (M[2][2] > Real(0.0))
            {
                vMin.z() += M[2][2] * mOriginalAabb.getMinZ();
                vMax.z() += M[2][2] * mOriginalAabb.getMaxZ();
            }
            else
            {
                vMin.z() += M[2][2] * mOriginalAabb.getMaxZ();
                vMax.z() += M[2][2] * mOriginalAabb.getMinZ();
            }

            mAabb.setParam(vMin, vMax);
        }
    }

    //--------------------------------------------------------------------------

    void AabbBound::onUpdate()
    {
        update();
    }

    //--------------------------------------------------------------------------
}


