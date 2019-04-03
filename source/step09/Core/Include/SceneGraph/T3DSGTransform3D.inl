/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    inline void SGTransform3D::setPosition(const Vector3 &pos)
    {
        if (pos != mPosition)
        {
            mPosition = pos;
            setDirty(true, true);
        }
    }

    //--------------------------------------------------------------------------

    inline void SGTransform3D::setPosition(Real x, Real y, Real z)
    {
        Vector3 pos(x, y, z);
        setPosition(pos);
    }

    //--------------------------------------------------------------------------

    inline const Vector3 &SGTransform3D::getPosition() const
    {
        return mPosition;
    }

    //--------------------------------------------------------------------------

    inline void SGTransform3D::setOrientation(const Quaternion &orientation)
    {
        if (orientation != mOrientation)
        {
            mOrientation = orientation;
            setDirty(true, true);
        }
    }

    //--------------------------------------------------------------------------

    inline void SGTransform3D::setOrientation(Real w, Real x, Real y, Real z)
    {
        Quaternion q(w, x, y, z);
        setOrientation(q);
    }

    //--------------------------------------------------------------------------

    inline void SGTransform3D::setOrientation(const Radian &radian,
        const Vector3 &axis)
    {
        Quaternion q(radian, axis);
        setOrientation(q);
    }

    //--------------------------------------------------------------------------

    inline const Quaternion &SGTransform3D::getOrientation() const
    {
        return mOrientation;
    }

    //--------------------------------------------------------------------------

    inline void SGTransform3D::setScaling(const Vector3 &scaling)
    {
        if (scaling != mScaling)
        {
            mScaling = scaling;
            setDirty(true, true);
        }
    }

    //--------------------------------------------------------------------------

    inline void SGTransform3D::setScaling(Real x, Real y, Real z)
    {
        Vector3 scaling(x, y, z);
        setScaling(scaling);
    }

    //--------------------------------------------------------------------------

    inline const Vector3 &SGTransform3D::getScaling() const
    {
        return mScaling;
    }

    //--------------------------------------------------------------------------

    inline void SGTransform3D::translate(const Vector3 &offset)
    {
        if (offset != Vector3::ZERO)
        {
            mPosition += offset;
            setDirty(true, true);
        }
    }

    //--------------------------------------------------------------------------

    inline void SGTransform3D::translate(Real x, Real y, Real z)
    {
        translate(Vector3(x, y, z));
    }

    //--------------------------------------------------------------------------

    inline void SGTransform3D::translate(const Vector3 &dir, Real step)
    {
        Vector3 offset = dir * step;
        translate(offset);
    }

    //--------------------------------------------------------------------------

    inline void SGTransform3D::rotate(const Quaternion &orientation)
    {
        if (orientation != Quaternion::IDENTITY)
        {
            mOrientation *= orientation;
            setDirty(true, true);
        }
    }

    //--------------------------------------------------------------------------

    inline void SGTransform3D::rotate(const Vector3 &axis, 
        const Radian &radians)
    {
        Quaternion q;
        q.fromAngleAxis(radians, axis);
        rotate(q);
    }

    //--------------------------------------------------------------------------

    inline void SGTransform3D::rotate(const Vector3 &axis,
        const Degree &degrees)
    {
        rotate(axis, Radian(degrees));
    }

    //--------------------------------------------------------------------------

    inline void SGTransform3D::pitch(const Degree &degrees)
    {
        rotate(Vector3::UNIT_X, degrees);
    }

    //--------------------------------------------------------------------------

    inline void SGTransform3D::yaw(const Degree &degrees)
    {
        rotate(Vector3::UNIT_Y, degrees);
    }

    //--------------------------------------------------------------------------

    inline void SGTransform3D::roll(const Degree &degrees)
    {
        rotate(Vector3::UNIT_Z, degrees);
    }

    //--------------------------------------------------------------------------

    inline void SGTransform3D::pitch(const Radian &radians)
    {
        rotate(Vector3::UNIT_X, radians);
    }

    //--------------------------------------------------------------------------

    inline void SGTransform3D::yaw(const Radian &radians)
    {
        rotate(Vector3::UNIT_Y, radians);
    }

    //--------------------------------------------------------------------------

    inline void SGTransform3D::roll(const Radian &radians)
    {
        rotate(Vector3::UNIT_Z, radians);
    }

    //--------------------------------------------------------------------------

    inline void SGTransform3D::scale(const Vector3 &scaling)
    {
        if (scaling != mScaling)
        {
            mScaling = scaling;
            setDirty(true, true);
        }
    }

    //--------------------------------------------------------------------------

    inline void SGTransform3D::scale(Real x, Real y, Real z)
    {
        scale(Vector3(x, y, z));
    }

    //--------------------------------------------------------------------------

    inline Transform SGTransform3D::getLocalTransform() const
    {
        return Transform(mPosition, mScaling, mOrientation);
    }

    //--------------------------------------------------------------------------

    inline bool SGTransform3D::isDirty() const
    {
        return mIsDirty;
    }

}
