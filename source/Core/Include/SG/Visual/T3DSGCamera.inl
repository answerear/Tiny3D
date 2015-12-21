

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
        mAspectRatio = ratio;
    }

    inline Real SGCamera::getAspectRatio() const
    {
        return mAspectRatio;
    }

    inline void SGCamera::setNearPlaneDistance(Real d)
    {
        mNearDistance = d;
    }

    inline Real SGCamera::getNearPlaneDistance() const
    {
        return mNearDistance;
    }

    inline void SGCamera::setFarPlaneDistance(Real d)
    {
        mFarDistance = d;
    }

    inline Real SGCamera::getFarPlaneDistance() const
    {
        return mFarDistance;
    }

    inline void SGCamera::setFovX(Real x)
    {
        mFovX = x;
    }

    inline Real SGCamera::getFovX() const
    {
        return mFovX;
    }

    inline void SGCamera::setFovY(Real y)
    {
        mFovY = y;
    }

    inline Real SGCamera::getFovY() const
    {
        return mFovY;
    }

    inline void SGCamera::setPerspective(Real fovx, Real ratio, Real near, Real far)
    {
        mFovX = fovx;
    }
}
