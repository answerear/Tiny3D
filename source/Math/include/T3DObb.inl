

namespace Tiny3D
{
    inline Obb::Obb()
        : mCenter(Vector3::ZERO)
    {
        mAxis[0] = mAxis[1] = mAxis[2] = Vector3::ZERO;
        mExtent[0] = mExtent[1] = mExtent[2] = 0.0;
    }

    inline Obb::Obb(const Vector3 &rkCenter, const Vector3 *akAxis, 
        const Real *afExtent)
        : mCenter(rkCenter)
    {
        mAxis[0] = akAxis[0];
        mAxis[1] = akAxis[1];
        mAxis[2] = akAxis[2];
        mExtent[0] = afExtent[0];
        mExtent[1] = afExtent[1];
        mExtent[2] = afExtent[2];
    }

    inline Obb::Obb(const Vector3 &rkCenter, const Vector3 &rkAxis0, 
        const Vector3 &rkAxis1, const Vector3 &rkAxis2, 
        Real fExtent0, Real fExtent1, Real fExtent2)
        : mCenter(rkCenter)
    {
        mAxis[0] = rkAxis0;
        mAxis[1] = rkAxis1;
        mAxis[2] = rkAxis2;
        mExtent[0] = fExtent0;
        mExtent[1] = fExtent1;
        mExtent[2] = fExtent2;
    }

    inline void Obb::computeVertices(Vector3 *akVertex) const
    {
        Vector3 akEAxis[3] =
        {
            mExtent[0] * mAxis[0], mExtent[1] * mAxis[1], mExtent[2] * mAxis[2]
        };

        akVertex[0] = mCenter - akEAxis[0] - akEAxis[1] - akEAxis[2];
        akVertex[1] = mCenter + akEAxis[0] - akEAxis[1] - akEAxis[2];
        akVertex[2] = mCenter + akEAxis[0] + akEAxis[1] - akEAxis[2];
        akVertex[3] = mCenter - akEAxis[0] + akEAxis[1] - akEAxis[2];
        akVertex[4] = mCenter - akEAxis[0] - akEAxis[1] + akEAxis[2];
        akVertex[5] = mCenter + akEAxis[0] - akEAxis[1] + akEAxis[2];
        akVertex[6] = mCenter + akEAxis[0] + akEAxis[1] + akEAxis[2];
        akVertex[7] = mCenter - akEAxis[0] + akEAxis[1] + akEAxis[2];
    }

    inline const Vector3 &Obb::getCenter() const
    {
        return mCenter;
    }

    inline const Vector3 &Obb::getAxis(int32_t idx) const
    {
        T3D_ASSERT(idx >= 0 && idx < 3);
        return mAxis[idx];
    }

    inline const Vector3 *Obb::getAxis() const
    {
        return mAxis;
    }

    inline Real Obb::getExtent(int32_t idx) const
    {
        return mExtent[idx];
    }

    inline const Real *Obb::getExtent() const
    {
        return mExtent;
    }

    inline void Obb::setCenter(const Vector3 &rkCenter)
    {
        mCenter = rkCenter;
    }

    inline void Obb::setAxis(const Vector3 &rkAxis0, const Vector3 &rkAxis1, 
        const Vector3 &rkAxis2)
    {
        mAxis[0] = rkAxis0;
        mAxis[1] = rkAxis1;
        mAxis[2] = rkAxis2;
    }

    inline void Obb::setExtent(int32_t idx, Real fExtent)
    {
        T3D_ASSERT(idx >= 0 && idx < 3);
        mExtent[idx] = fExtent;
    }
}
