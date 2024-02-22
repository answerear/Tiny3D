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

#ifndef __T3D_TRANSFORM_H__
#define __T3D_TRANSFORM_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    TSTRUCT()
    struct T3D_ENGINE_API Transform
    {
    public:
        Transform();

        Transform(const Vector3 &translation, const Vector3 &scaling, const Quaternion &orientation);

        Transform(const Transform &other);

        Transform &operator =(const Transform &other);

        TPROPERTY(RTTRFuncName="Translation", RTTRFuncType="setter")
        void setTranslation(const Vector3 &translation);

        TPROPERTY(RTTRFuncName="Translation", RTTRFuncType="getter")
        const Vector3 &getTranslation() const;

        Vector3 &getTranslation();

        TPROPERTY(RTTRFuncName="Scaling", RTTRFuncType="setter")
        void setScaling(const Vector3 &scaling);

        TPROPERTY(RTTRFuncName="Scaling", RTTRFuncType="getter")
        const Vector3 &getScaling() const;

        Vector3 &getScaling();

        TPROPERTY(RTTRFuncName="Orientation", RTTRFuncType="setter")
        void setOrientation(const Quaternion &orientation);

        TPROPERTY(RTTRFuncName="Orientation", RTTRFuncType="getter")
        const Quaternion &getOrientation() const;

        Quaternion &getOrientation();

        void applyTransform(const Transform &xform1, const Transform &xform2);

        void applyTransform(const Transform &xform, const Vector3 &translation, const Quaternion &orientation, const Vector3 &scaling);

        const Matrix4 &getAffineMatrix() const;

        void update();

    private:
        void makeAffineMatrix();

    private:
        /// 平移
        Vector3     mTranslation {};
        /// 缩放
        Vector3     mScaling {};
        /// 朝向
        Quaternion  mOrientation {};

        /// 根据 TRS 构造的仿射变黄
        Matrix4     mAffineMatrix {};
    };
}


#include "T3DTransform.inl"


#endif    /*__T3D_TRANSFORM_H__*/
