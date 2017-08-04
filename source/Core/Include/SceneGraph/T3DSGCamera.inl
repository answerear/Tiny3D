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
    inline void SGCamera::setProjectionType(ProjectionType eType)
    {
        mProjType = eType;
    }

    inline SGCamera::ProjectionType SGCamera::getProjectionType() const
    {
        return mProjType;
    }

    inline void SGCamera::setAspectRatio(Real ratio)
    {
        if (mAspectRatio != ratio)
        {
            mAspectRatio = ratio;
            mIsFrustumDirty = true;
        }
    }

    inline Real SGCamera::getAspectRatio() const
    {
        return mAspectRatio;
    }

    inline void SGCamera::setNearPlaneDistance(Real d)
    {
        if (mNearDistance != d)
        {
            mNearDistance = d;
            mIsFrustumDirty = true;
        }
    }

    inline Real SGCamera::getNearPlaneDistance() const
    {
        return mNearDistance;
    }

    inline void SGCamera::setFarPlaneDistance(Real d)
    {
        if (mFarDistance != d)
        {
            mFarDistance = d;
            mIsFrustumDirty = true;
        }
    }

    inline Real SGCamera::getFarPlaneDistance() const
    {
        return mFarDistance;
    }

    inline void SGCamera::setFovY(const Radian &radians)
    {
        if (radians != mFovY)
        {
            mFovY = radians;
            mIsFrustumDirty = true;
        }
    }

    inline const Radian &SGCamera::getFovY() const
    {
        return mFovY;
    }

    inline void SGCamera::setPerspective(const Radian &fovy, Real aspect, Real nearDist, Real farDist)
    {
        setFovY(fovy);
        setAspectRatio(aspect);
        setNearPlaneDistance(nearDist);
        setFarPlaneDistance(farDist);
    }
}
