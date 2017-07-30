/*******************************************************************************
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
 ******************************************************************************/

namespace Tiny3D
{
    inline void SGTransformNode::setPosition(const Vector3 &rkPos)
    {
        if (rkPos != mPosition)
        {
            mPosition = rkPos;
            setDirty(true, true);
        }
    }

    inline void SGTransformNode::setPosition(Real x, Real y, Real z)
    {
        Vector3 pos(x, y, z);
        setPosition(pos);
    }

    inline const Vector3 &SGTransformNode::getPosition() const
    {
        return mPosition;
    }

    inline void SGTransformNode::setOrientation(const Quaternion &rkQ)
    {
        if (rkQ != mOrientation)
        {
            mOrientation = rkQ;
            setDirty(true, true);
        }
    }

    inline void SGTransformNode::setOrientation(Real w, Real x, Real y, Real z)
    {
        Quaternion q(w, x, y, z);
        setOrientation(q);
    }

    inline void SGTransformNode::setOrientation(const Radian &radian, const Vector3 &axis)
    {
        Quaternion q(radian, axis);
        setOrientation(q);
    }

    inline const Quaternion &SGTransformNode::getOrientation() const
    {
        return mOrientation;
    }

    inline void SGTransformNode::setScale(const Vector3 &rkScale)
    {
        if (rkScale != mScale)
        {
            mScale = rkScale;
            setDirty(true, true);
        }
    }

    inline void SGTransformNode::setScale(Real x, Real y, Real z)
    {
        Vector3 s(x, y, z);
        setScale(s);
    }

    inline const Vector3 &SGTransformNode::getScale() const
    {
        return mScale;
    }

    inline void SGTransformNode::translate(const Vector3 &rkOffset)
    {
        if (rkOffset != Vector3::ZERO)
        {
            mPosition += rkOffset;
            setDirty(true, true);
        }
    }

    inline void SGTransformNode::translate(Real x, Real y, Real z)
    {
        Vector3 offset(x, y, z);
        translate(offset);
    }

    inline void SGTransformNode::translate(const Matrix3 &axis, const Vector3 &rkOffset)
    {
        Vector3 offset = axis * rkOffset;
        translate(offset);
    }

    inline void SGTransformNode::translate(const Matrix3 &axis, Real x, Real y, Real z)
    {
        Vector3 offset(x, y, z);
        translate(axis, offset);
    }

    inline void SGTransformNode::rotate(const Quaternion &rkQ)
    {
        if (rkQ != Quaternion::IDENTITY)
        {
            mOrientation *= rkQ;
            setDirty(true, true);
        }
    }

    inline void SGTransformNode::rotate(const Vector3 &axis, const Radian &radians)
    {
        Quaternion q;
        q.fromAngleAxis(radians, axis);
        rotate(q);
    }

    inline void SGTransformNode::rotate(const Vector3 &axis, const Degree &degrees)
    {
        rotate(axis, Radian(degrees));
    }

    inline void SGTransformNode::pitch(const Degree &degrees)
    {
        rotate(Vector3::UNIT_X, degrees);
    }

    inline void SGTransformNode::yaw(const Degree &degrees)
    {
        rotate(Vector3::UNIT_Y, degrees);
    }

    inline void SGTransformNode::roll(const Degree &degrees)
    {
        rotate(Vector3::UNIT_Z, degrees);
    }

    inline void SGTransformNode::pitch(const Radian &radians)
    {
        rotate(Vector3::UNIT_X, radians);
    }

    inline void SGTransformNode::yaw(const Radian &radians)
    {
        rotate(Vector3::UNIT_Y, radians);
    }

    inline void SGTransformNode::roll(const Radian &radians)
    {
        rotate(Vector3::UNIT_Z, radians);
    }

    inline void SGTransformNode::scale(const Vector3 &rkScale)
    {
        if (rkScale != Vector3::ZERO)
        {
            mScale *= rkScale;
            setDirty(true, true);
        }
    }

    inline void SGTransformNode::scale(Real x, Real y, Real z)
    {
        Vector3 s(x, y, z);
        scale(s);
    }

    inline Transform SGTransformNode::getLocalTransform() const
    {
        return Transform(mPosition, mScale, mOrientation);
    }
}
