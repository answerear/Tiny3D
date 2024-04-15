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

    inline void Transform3D::setPosition(const Vector3 &pos)
    {
        if (pos != mPosition)
        {
            Vector3 oldPos = mPosition;
            mPosition = pos;
            onPositionChanged(oldPos, mPosition);
            setDirty(true, true);
        }
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::setPosition(Real x, Real y, Real z)
    {
        Vector3 pos(x, y, z);
        setPosition(pos);
    }

    //--------------------------------------------------------------------------

    inline const Vector3 &Transform3D::getPosition() const
    {
        return mPosition;
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::setOrientation(const Quaternion &orientation)
    {
        if (orientation != mOrientation)
        {
            Quaternion oldOrient = mOrientation;
            mOrientation = orientation;
            onOrientationChanged(oldOrient, mOrientation);
            setDirty(true, true);
        }
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::setOrientation(Real w, Real x, Real y, Real z)
    {
        Quaternion q(w, x, y, z);
        setOrientation(q);
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::setOrientation(const Radian &radian,
        const Vector3 &axis)
    {
        Quaternion q(radian, axis);
        setOrientation(q);
    }

    //--------------------------------------------------------------------------

    inline const Quaternion &Transform3D::getOrientation() const
    {
        return mOrientation;
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::setScaling(const Vector3 &scaling)
    {
        if (scaling != mScaling)
        {
            Vector3 oldScaling = mScaling;
            mScaling = scaling;
            onScalingChanged(oldScaling, mScaling);
            setDirty(true, true);
        }
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::setScaling(Real x, Real y, Real z)
    {
        Vector3 scaling(x, y, z);
        setScaling(scaling);
    }

    //--------------------------------------------------------------------------

    inline const Vector3 &Transform3D::getScaling() const
    {
        return mScaling;
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::translate(const Vector3 &offset)
    {
        if (offset != Vector3::ZERO)
        {
            Vector3 pos = mPosition;
            mPosition += offset;
            onPositionChanged(pos, mPosition);
            setDirty(true, true);
        }
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::translate(Real x, Real y, Real z)
    {
        translate(Vector3(x, y, z));
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::translate(const Vector3 &dir, Real step)
    {
        Vector3 offset = dir * step;
        translate(offset);
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::rotate(const Quaternion &orientation)
    {
        if (orientation != Quaternion::IDENTITY)
        {
            Quaternion oldOrient = mOrientation;
            mOrientation *= orientation;
            onOrientationChanged(oldOrient, mOrientation);
            setDirty(true, true);
        }
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::rotate(const Vector3 &axis, 
        const Radian &radians)
    {
        Quaternion q;
        q.fromAngleAxis(radians, axis);
        rotate(q);
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::rotate(const Vector3 &axis,
        const Degree &degrees)
    {
        rotate(axis, Radian(degrees));
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::pitch(const Degree &degrees)
    {
        rotate(Vector3::UNIT_X, degrees);
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::yaw(const Degree &degrees)
    {
        rotate(Vector3::UNIT_Y, degrees);
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::roll(const Degree &degrees)
    {
        rotate(Vector3::UNIT_Z, degrees);
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::pitch(const Radian &radians)
    {
        rotate(Vector3::UNIT_X, radians);
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::yaw(const Radian &radians)
    {
        rotate(Vector3::UNIT_Y, radians);
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::roll(const Radian &radians)
    {
        rotate(Vector3::UNIT_Z, radians);
    }

    //--------------------------------------------------------------------------

    inline Matrix3 Transform3D::fromEulerAnglesXYZ(const Radian &xAngle, const Radian &yAngle, const Radian &zAngle)
    {
        Matrix3 mat;
        mat.fromEulerAnglesXYZ(xAngle, yAngle, zAngle);
        mOrientation = mat;
        return mat;
    }
    
    //--------------------------------------------------------------------------

    inline Matrix3 Transform3D::fromEulerAnglesXZY(const Radian &xAngle, const Radian &zAngle, const Radian &yAngle)
    {
        Matrix3 mat;
        mat.fromEulerAnglesXZY(xAngle, zAngle, yAngle);
        mOrientation = mat;
        return mat;
    }

    //--------------------------------------------------------------------------

    inline Matrix3 Transform3D::fromEulerAnglesYXZ(const Radian &yAngle, const Radian &xAngle, const Radian &zAngle)
    {
        Matrix3 mat;
        mat.fromEulerAnglesYXZ(yAngle, xAngle, zAngle);
        mOrientation = mat;
        return mat;
    }

    //--------------------------------------------------------------------------

    inline Matrix3 Transform3D::fromEulerAnglesYZX(const Radian &yAngle, const Radian &zAngle, const Radian &xAngle)
    {
        Matrix3 mat;
        mat.fromEulerAnglesYZX(yAngle, zAngle, xAngle);
        mOrientation = mat;
        return mat;
    }

    //--------------------------------------------------------------------------

    inline Matrix3 Transform3D::fromEulerAnglesZXY(const Radian &zAngle, const Radian &xAngle, const Radian &yAngle)
    {
        Matrix3 mat;
        mat.fromEulerAnglesZXY(zAngle, xAngle, yAngle);
        mOrientation = mat;
        return mat;
    }

    //--------------------------------------------------------------------------

    inline Matrix3 Transform3D::fromEulerAnglesZYX(const Radian &zAngle, const Radian &yAngle, const Radian &xAngle)
    {
        Matrix3 mat;
        mat.fromEulerAnglesZYX(zAngle, yAngle, xAngle);
        mOrientation = mat;
        return mat;
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::toEulerAnglesXYZ(Radian &xAngle, Radian &yAngle, Radian &zAngle)
    {
        Matrix3 mat;
        mOrientation.toRotationMatrix(mat);
        mat.toEulerAnglesXYZ(xAngle, yAngle, zAngle);
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::toEulerAnglesXZY(Radian &xAngle, Radian &zAngle, Radian &yAngle)
    {
        Matrix3 mat;
        mOrientation.toRotationMatrix(mat);
        mat.toEulerAnglesXZY(xAngle, zAngle, yAngle);
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::toEulerAnglesYXZ(Radian &yAngle, Radian &xAngle, Radian &zAngle)
    {
        Matrix3 mat;
        mOrientation.toRotationMatrix(mat);
        mat.toEulerAnglesYXZ(yAngle, xAngle, zAngle);
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::toEulerAnglesYZX(Radian &yAngle, Radian &zAngle, Radian &xAngle)
    {
        Matrix3 mat;
        mOrientation.toRotationMatrix(mat);
        mat.toEulerAnglesYZX(yAngle, zAngle, xAngle);
    }
    
    //--------------------------------------------------------------------------

    inline void Transform3D::toEulerAnglesZXY(Radian &zAngle, Radian &xAngle, Radian &yAngle)
    {
        Matrix3 mat;
        mOrientation.toRotationMatrix(mat);
        mat.toEulerAnglesZXY(zAngle, xAngle, yAngle);
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::toEulerAnglesZYX(Radian &zAngle, Radian &yAngle, Radian &xAngle)
    {
        Matrix3 mat;
        mOrientation.toRotationMatrix(mat);
        mat.toEulerAnglesZYX(zAngle, yAngle, xAngle);
    }
    
    //--------------------------------------------------------------------------

    inline void Transform3D::scale(const Vector3 &scaling)
    {
        if (scaling != mScaling)
        {
            Vector3 oldScaling = scaling;
            mScaling += scaling;
            onScalingChanged(oldScaling, scaling);
            setDirty(true, true);
        }
    }

    //--------------------------------------------------------------------------

    inline void Transform3D::scale(Real x, Real y, Real z)
    {
        scale(Vector3(x, y, z));
    }

    //--------------------------------------------------------------------------

    inline Transform Transform3D::getLocalTransform() const
    {
        return Transform(mPosition, mScaling, mOrientation);
    }

    //--------------------------------------------------------------------------

    inline bool Transform3D::isDirty() const
    {
        return mIsDirty;
    }

    //--------------------------------------------------------------------------
}
