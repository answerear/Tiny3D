/***************************************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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
 **************************************************************************************************/


namespace Tiny3D
{
    inline void SGTransform2D::setPosition(const Vector2 &rkPos)
    {
        if (rkPos != mPosition)
        {
            mPosition = rkPos;
            setDirty(true, true);
        }
    }

    inline void SGTransform2D::setPosition(Real x, Real y)
    {
        Vector2 pos(x, y);
        setPosition(pos);
    }

    inline const Vector2 &SGTransform2D::getPosition() const
    {
        return mPosition;
    }

    inline void SGTransform2D::setOrientation(const Quaternion &rkQ)
    {
        if (rkQ != mOrientation)
        {
            mOrientation = rkQ;
            setDirty(true, true);
        }
    }

    inline void SGTransform2D::setOrientation(Real w, Real x, Real y, Real z)
    {
        Quaternion q(w, x, y, z);
        setOrientation(q);
    }

    inline void SGTransform2D::setOrientation(const Radian &radian, const Vector3 &axis)
    {
        Quaternion q(radian, axis);
        setOrientation(q);
    }

    inline const Quaternion &SGTransform2D::getOrientation() const
    {
        return mOrientation;
    }

    inline void SGTransform2D::setScale(const Vector2 &rkScale)
    {
        if (rkScale != mScale)
        {
            mScale = rkScale;
            setDirty(true, true);
        }
    }

    inline void SGTransform2D::setScale(Real x, Real y)
    {
        Vector2 s(x, y);
        setScale(s);
    }

    inline const Vector2 &SGTransform2D::getScale() const
    {
        return mScale;
    }

    inline void SGTransform2D::translate(const Vector2 &rkOffset)
    {
        if (rkOffset != Vector2::ZERO)
        {
            mPosition += rkOffset;
            setDirty(true, true);
        }
    }

    inline void SGTransform2D::translate(Real x, Real y)
    {
        translate(Vector2(x, y));
    }

    inline void SGTransform2D::translate(const Vector2 &rkOrientation, Real step)
    {
        Vector2 offset = rkOrientation * step;
        translate(offset);
    }

    inline void SGTransform2D::rotate(const Quaternion &rkQ)
    {
        if (rkQ != Quaternion::IDENTITY)
        {
            mOrientation *= rkQ;
            setDirty(true, true);
        }
    }

    inline void SGTransform2D::rotate(const Vector3 &axis, const Radian &radians)
    {
        Quaternion q;
        q.fromAngleAxis(radians, axis);
        rotate(q);
    }

    inline void SGTransform2D::rotate(const Vector3 &axis, const Degree &degrees)
    {
        rotate(axis, Radian(degrees));
    }

    inline void SGTransform2D::pitch(const Degree &degrees)
    {
        rotate(Vector3::UNIT_X, degrees);
    }

    inline void SGTransform2D::yaw(const Degree &degrees)
    {
        rotate(Vector3::UNIT_Y, degrees);
    }

    inline void SGTransform2D::roll(const Degree &degrees)
    {
        rotate(Vector3::UNIT_Z, degrees);
    }

    inline void SGTransform2D::pitch(const Radian &radians)
    {
        rotate(Vector3::UNIT_X, radians);
    }

    inline void SGTransform2D::yaw(const Radian &radians)
    {
        rotate(Vector3::UNIT_Y, radians);
    }

    inline void SGTransform2D::roll(const Radian &radians)
    {
        rotate(Vector3::UNIT_Z, radians);
    }

    inline void SGTransform2D::scale(const Vector2 &rkScale)
    {
        if (rkScale != Vector3::ZERO)
        {
            mScale *= rkScale;
            setDirty(true, true);
        }
    }

    inline void SGTransform2D::scale(Real x, Real y)
    {
        Vector2 s(x, y);
        scale(s);
    }

    inline Transform SGTransform2D::getLocalTransform() const
    {
        return Transform(Vector3(mPosition.x(), mPosition.y(), Real(0.0)), Vector3(mScale.x(), mScale.y(), Real(1.0)), mOrientation);
    }
}
