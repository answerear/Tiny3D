

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
