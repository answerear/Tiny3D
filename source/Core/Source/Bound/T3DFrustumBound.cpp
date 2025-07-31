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


#include "Bound/T3DFrustumBound.h"

#include "Component/T3DCamera.h"
#include "Component/T3DTransform3D.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    FrustumBoundPtr FrustumBound::create()
    {
        return new FrustumBound(UUID::generate());
    }

    //--------------------------------------------------------------------------

    FrustumBound::FrustumBound(const UUID &uuid)
        : Bound(uuid)
    {
        
    }
    
    //--------------------------------------------------------------------------

    FrustumBound::~FrustumBound()
    {

    }

    //--------------------------------------------------------------------------

    Bound::Type FrustumBound::getType() const
    {
        return Type::FRUSTUM;
    }

    //--------------------------------------------------------------------------

    ComponentPtr FrustumBound::clone() const
    {
        FrustumBoundPtr newObj = new FrustumBound();
        if (T3D_FAILED(newObj->cloneProperties(this)))
        {
            newObj = nullptr;
        }
        return newObj;
    }

    //--------------------------------------------------------------------------

    TResult FrustumBound::cloneProperties(const Component * const src)
    {
        TResult ret = Bound::cloneProperties(src);

        if (ret == T3D_OK)
        {
            const FrustumBound *const newBound = static_cast<const FrustumBound* const>(src);
            mFrustum = newBound->mFrustum;
            mOriginalFrustum = newBound->mOriginalFrustum;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void FrustumBound::setFrustumFace(Face face, const Plane &plane)
    {
        mFrustum.setFace(face, plane);
        mOriginalFrustum.setFace(face, plane);
    }

    //--------------------------------------------------------------------------

    void FrustumBound::setFrustumFaces(Plane *plane, size_t planeCount)
    {
        T3D_ASSERT(planeCount <= E_MAX_FACE);

        for (size_t i = 0; i < planeCount; ++i)
        {
            mFrustum.setFace((Face)i, plane[i]);
            mOriginalFrustum.setFace((Face)i, plane[i]);
        }
    }

    //--------------------------------------------------------------------------

    Renderable *FrustumBound::getRenderable()
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    bool FrustumBound::testSphere(const Sphere &sphere) const
    {
        IntrFrustumSphere intr(&mFrustum, &sphere);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool FrustumBound::testAabb(const Aabb &aabb) const
    {
        IntrFrustumAabb intr(&mFrustum, &aabb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool FrustumBound::testObb(const Obb &obb) const
    {
        IntrFrustumObb intr(&mFrustum, &obb);
        return intr.test();
    }

    //--------------------------------------------------------------------------

    bool FrustumBound::testFrustum(const Frustum &frustum) const
    {
        // 暂时不具备这种检测能力，而且实际应用中应该也没有这样的需求
        return false;
    }

    //--------------------------------------------------------------------------
    
    void FrustumBound::update()
    {
        // 快速计算视棱锥六个裁剪平面原理：
        //
        //  假设点V'是投影变换后的点，V是投影变换前相机坐标系的点，M是投影变换矩阵，则可得：
        //      V' = M * V
        //  其中
        //      V' = (x' y' z' w')
        //
        //      V = (x y z w), (w = 1)
        //
        //          | m00 m01 m02 m03 |
        //      M = | m10 m11 m12 m13 |
        //          | m20 m21 m22 m23 |
        //          | m30 m31 m32 m33 |
        //  则
        //      | m00 m01 m02 m03 |   | x |   | x*m00 + y*m01 + z*m02 + w*m03 |   | V * row0 |
        //      | m10 m11 m12 m13 |   | y |   | x*m10 + y*m11 + z*m12 + w*m13 |   | V * row1 |
        //      | m20 m21 m22 m23 | * | z | = | x*m20 + y*m21 + z*m22 + w*m23 | = | V * row2 |
        //      | m30 m31 m32 m33 |   | w |   | x*m30 + y*m31 + z*m32 + w*m33 |   | V * row3 |
        //
        //  这里的*代表点积，而rowi = (mi0 mi1 mi2 mi3)
        // 
        //  经过转换后，V'在齐次裁剪空间里。这个空间实际上现在已经是一个轴对齐的盒子。
        //  如果V'在这个盒子空间里，则变换前的V点也在这个没有经过变换的盒子空间里。
        //  这里要分一下DX和OpenGL的区别，因为投影空间不一样：
        //
        //      1、在DX里，V'在这个空间里，则符合下面的不等式规则
        //              -w' < x' < w'
        //              -w' < y' < w'
        //               0 < z' < w'
        //          用 -w' < x' 推导：
        //              -(V * row3) < (V * row0)
        //          移动项得：
        //              0 < (V * row3) + (V * row0)
        //          合并同类项，最终可得：
        //              0 < V * (row3 + row0)
        //
        //          如此类推可得：
        //              left    :   0 < V * (row3 + row0)   a=m30+m00, b=m31+m01, c=m32+m02, d=m33+m03
        //              right   :   0 < V * (row3 - row0)   a=m30-m00, b=m31-m01, c=m32-m02, d=m33-m03
        //              bottom  :   0 < V * (row3 + row1)   a=m30+m10, b=m31+m11, c=m32+m12, d=m33+m13
        //              top     :   0 < V * (row3 - row1)   a=m30-m10, b=m31-m11, c=m32-m12, d=m33-m13
        //              near    :   0 < V * row2            a=m20,     b=m21,     c=m22,     d=m23
        //              far     :   0 < V * (row3 - row2)   a=m30-m20, b=m31-m21, c=m32-m22, d=m33-m23
        //      2、在OpenGL里，V'在这个空间里，则符合下面的不等式规则
        //              -w' < x' < w'
        //              -w' < y' < w'
        //              -w' < z' < w'
        //              left    :   0 < V * (row3 + row0)   a=m30+m00, b=m31+m01, c=m32+m02, d=m33+m03
        //              right   :   0 < V * (row3 - row0)   a=m30-m00, b=m31-m01, c=m32-m02, d=m33-m03
        //              bottom  :   0 < V * (row3 + row1)   a=m30+m10, b=m31+m11, c=m32+m12, d=m33+m13
        //              top     :   0 < V * (row3 - row1)   a=m30-m10, b=m31-m11, c=m32-m12, d=m33-m13
        //              near    :   0 < V * (row3 + row2)   a=m30+m20, b=m31+m21, c=m32+m22, d=m33+m23
        //              far     :   0 < V * (row3 - row2)   a=m30-m20, b=m31-m21, c=m32-m22, d=m33-m23

        Plane plane[E_MAX_FACE];

        // const Matrix4 &m = mXformNode->getLocalToWorldTransform().getAffineMatrix();
        Camera *camera = getGameObject()->getComponent<Camera>();
        Matrix4 m = camera->getProjectionMatrix() * camera->getViewMatrix();

        // Left
        plane[E_FACE_LEFT][0] = m[3][0] + m[0][0];
        plane[E_FACE_LEFT][1] = m[3][1] + m[0][1];
        plane[E_FACE_LEFT][2] = m[3][2] + m[0][2];
        plane[E_FACE_LEFT][3] = m[3][3] + m[0][3];
        plane[E_FACE_LEFT].normalize();

        // Right
        plane[E_FACE_RIGHT][0] = m[3][0] - m[0][0];
        plane[E_FACE_RIGHT][1] = m[3][1] - m[0][1];
        plane[E_FACE_RIGHT][2] = m[3][2] - m[0][2];
        plane[E_FACE_RIGHT][3] = m[3][3] - m[0][3];
        plane[E_FACE_RIGHT].normalize();

        // Bottom
        plane[E_FACE_BOTTOM][0] = m[3][0] + m[1][0];
        plane[E_FACE_BOTTOM][1] = m[3][1] + m[1][1];
        plane[E_FACE_BOTTOM][2] = m[3][2] + m[1][2];
        plane[E_FACE_BOTTOM][3] = m[3][3] + m[1][3];
        plane[E_FACE_BOTTOM].normalize();

        // Top
        plane[E_FACE_TOP][0] = m[3][0] - m[1][0];
        plane[E_FACE_TOP][1] = m[3][1] - m[1][1];
        plane[E_FACE_TOP][2] = m[3][2] - m[1][2];
        plane[E_FACE_TOP][3] = m[3][3] - m[1][3];
        plane[E_FACE_TOP].normalize();

        // Near
        // plane[E_FACE_NEAR][0] = m[2][0];
        // plane[E_FACE_NEAR][1] = m[2][1];
        // plane[E_FACE_NEAR][2] = m[2][2];
        // plane[E_FACE_NEAR][3] = m[2][3];
        plane[E_FACE_NEAR][0] = m[3][0] + m[2][0];
        plane[E_FACE_NEAR][1] = m[3][1] + m[2][1];
        plane[E_FACE_NEAR][2] = m[3][2] + m[2][2];
        plane[E_FACE_NEAR][3] = m[3][3] + m[2][3];
        plane[E_FACE_NEAR].normalize();

        // Far
        plane[E_FACE_FAR][0] = m[3][0] - m[2][0];
        plane[E_FACE_FAR][1] = m[3][1] - m[2][1];
        plane[E_FACE_FAR][2] = m[3][2] - m[2][2];
        plane[E_FACE_FAR][3] = m[3][3] - m[2][3];
        plane[E_FACE_FAR].normalize();

        setFrustumFaces(plane, E_MAX_FACE);
    }

    //--------------------------------------------------------------------------

    void FrustumBound::onStart()
    {
        Bound::onStart();
        // mXformNode = getGameObject()->getComponent<Transform3D>();
        mXformNode = static_cast<Transform3D *>(getGameObject()->getTransformNode());
    }

    //--------------------------------------------------------------------------

    void FrustumBound::onUpdate()
    {
        Bound::onUpdate();
        update();
    }

    //--------------------------------------------------------------------------

    void FrustumBound::onDestroy()
    {
        mXformNode = nullptr;
        Bound::onDestroy();
    }

    //--------------------------------------------------------------------------
}

