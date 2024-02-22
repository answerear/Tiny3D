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


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    inline void Transform::makeAffineMatrix()
    {
        Matrix3 R;
        mOrientation.toRotationMatrix(R);
        Matrix3 S(mScaling.x(), mScaling.y(), mScaling.z());
        mAffineMatrix = R * S;
        mAffineMatrix.setTranslate(mTranslation);
    }

    //--------------------------------------------------------------------------

    inline Transform::Transform()
        : mTranslation(Vector3::ZERO)
        , mScaling(Vector3::UNIT_SCALE)
        , mOrientation(Quaternion::IDENTITY)
        , mAffineMatrix(false)
    {

    }

    //--------------------------------------------------------------------------

    inline Transform::Transform(const Vector3 &translation, 
        const Vector3 &scaling, const Quaternion &orientation)
        : mTranslation(translation)
        , mScaling(scaling)
        , mOrientation(orientation)
    {
        makeAffineMatrix();
    }

    //--------------------------------------------------------------------------

    inline Transform::Transform(const Transform &other)
    {
        mTranslation = other.mTranslation;
        mOrientation = other.mOrientation;
        mScaling = other.mScaling;
        mAffineMatrix = other.mAffineMatrix;
    }

    //--------------------------------------------------------------------------

    inline Transform &Transform::operator =(const Transform &other)
    {
        mTranslation = other.mTranslation;
        mOrientation = other.mOrientation;
        mScaling = other.mScaling;
        mAffineMatrix = other.mAffineMatrix;
        return *this;
    }

    //--------------------------------------------------------------------------

    inline void Transform::setTranslation(const Vector3 &translation)
    {
        mTranslation = translation;
    }

    //--------------------------------------------------------------------------

    inline const Vector3 &Transform::getTranslation() const
    {
        return mTranslation;
    }

    //--------------------------------------------------------------------------

    inline Vector3 &Transform::getTranslation()
    {
        return mTranslation;
    }

    //--------------------------------------------------------------------------

    inline void Transform::setScaling(const Vector3 &scaling)
    {
        mScaling = scaling;
    }

    //--------------------------------------------------------------------------

    inline const Vector3 &Transform::getScaling() const
    {
        return mScaling;
    }

    //--------------------------------------------------------------------------

    inline Vector3 &Transform::getScaling()
    {
        return mScaling;
    }

    //--------------------------------------------------------------------------

    inline void Transform::setOrientation(const Quaternion &orientation)
    {
        mOrientation = orientation;
    }

    //--------------------------------------------------------------------------

    inline const Quaternion &Transform::getOrientation() const
    {
        return mOrientation;
    }

    //--------------------------------------------------------------------------

    inline Quaternion &Transform::getOrientation()
    {
        return mOrientation;
    }

    //--------------------------------------------------------------------------

    inline const Matrix4 &Transform::getAffineMatrix() const
    {
        return mAffineMatrix;
    }

    //--------------------------------------------------------------------------

    inline void Transform::update()
    {
        makeAffineMatrix();
    }
}
