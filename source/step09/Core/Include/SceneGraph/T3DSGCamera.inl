/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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
    //--------------------------------------------------------------------------

    inline void SGCamera::setObjectMask(uint32_t mask)
    {
        mObjectMask = mask;
    }

    //--------------------------------------------------------------------------

    inline uint32_t SGCamera::getObjectMask() const
    {
        return mObjectMask;
    }

    //--------------------------------------------------------------------------

    inline BoundPtr SGCamera::getBound() const
    {
        return mBound;
    }

    //--------------------------------------------------------------------------

    inline void SGCamera::setProjectionType(ProjectionType type)
    {
        if (mProjType != type)
        {
            mProjType = type;
            mIsFrustumDirty = true;
        }
    }

    //--------------------------------------------------------------------------

    inline SGCamera::ProjectionType SGCamera::getProjectionType() const
    {
        return mProjType;
    }

    //--------------------------------------------------------------------------

    inline void SGCamera::setProjectionParams(Real left, Real right,
        Real top, Real bottom, Real nearDist, Real farDist)
    {
        if (left != mLeft)
        {
            mLeft = left;
            mIsFrustumDirty = true;
        }

        if (right != mRight)
        {
            mRight = right;
            mIsFrustumDirty = true;
        }

        if (top != mTop)
        {
            mTop = top;
            mIsFrustumDirty = true;
        }

        if (bottom != mBottom)
        {
            mBottom = bottom;
            mIsFrustumDirty = true;
        }

        if (nearDist != mNear)
        {
            mNear = nearDist;
            mIsFrustumDirty = true;
        }

        if (farDist != mFar)
        {
            mFar = farDist;
            mIsFrustumDirty = true;
        }
    }

    //--------------------------------------------------------------------------

    inline Real SGCamera::getAspectRatio() const
    {
        return (mRight - mLeft) / (mTop - mBottom);
    }

    //--------------------------------------------------------------------------

    inline Radian SGCamera::getFovY() const
    {
        return Math::atan2(2 * mNear, (mTop - mBottom));
    }

    //--------------------------------------------------------------------------

    inline Real SGCamera::getNearPlaneDistance() const
    {
        return mNear;
    }

    //--------------------------------------------------------------------------

    inline Real SGCamera::getFarPlaneDistance() const
    {
        return mFar;
    }
}
