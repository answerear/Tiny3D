

namespace Tiny3D
{
    inline Aabb::Aabb()
        : mMinX(0.0)
        , mMaxX(0.0)
        , mMinY(0.0)
        , mMaxY(0.0)
        , mMinZ(0.0)
        , mMaxZ(0.0)
        , mSphere(Vector3::ZERO, 0.0)
    {

    }

    inline Aabb::Aabb(Real fXMin, Real fXMax, 
        Real fYMin, Real fYMax, 
        Real fZMin, Real fZMax)
        : mMinX(fXMin)
        , mMaxX(fXMax)
        , mMinY(fYMin)
        , mMaxY(fYMax)
        , mMinZ(fZMin)
        , mMaxZ(fZMax)
    {
        T3D_ASSERT(mMinX <= mMaxX && mMinY <= mMaxY && mMinZ <= mMaxZ);

        mSphere.getCenter().x() = (mMinX + mMaxX) * Real(0.5);
        mSphere.getCenter().y() = (mMinY + mMaxY) * Real(0.5);
        mSphere.getCenter().z() = (mMinZ + mMaxZ) * Real(0.5);

        Real fWidth = mMaxX - mMinX;
        Real fHeight = mMaxY - mMinY;
        Real fDepth = mMaxZ - mMinZ;
        mSphere.getRadius() = Math::Sqrt(fWidth * fWidth + fHeight * fHeight
            + fDepth * fDepth) * Real(0.5);
    }

    inline bool Aabb::hasXOverlap(const Aabb &rkBox) const
    {
        return (mMaxX >= rkBox.mMinX && mMinX <= rkBox.mMaxX);
    }

    inline bool Aabb::hasYOverlap(const Aabb &rkBox) const
    {
        return (mMaxY >= rkBox.mMinY && mMinY <= rkBox.mMaxY);
    }

    inline bool Aabb::hasZOverlap(const Aabb &rkBox) const
    {
        return (mMaxZ >= rkBox.mMinZ && mMinZ <= rkBox.mMaxZ);
    }

    inline bool Aabb::testIntersection(const Aabb &rkBox) const
    {
        return !(mMaxX < rkBox.mMinX || mMinX > rkBox.mMaxX
            || mMaxY < rkBox.mMinY || mMinY > rkBox.mMaxY
            || mMaxZ < rkBox.mMinZ || mMinZ > rkBox.mMaxZ);
    }

    inline bool Aabb::findIntersection(const Aabb &rkBox, Aabb &rIntr) const
    {
        bool bRet = testIntersection(rkBox);

        if (bRet)
        {
            // x-coordination
            if (mMaxX <= rkBox.mMaxX)
                rIntr.mMaxX = mMaxX;
            else
                rIntr.mMaxX = rkBox.mMaxX;

            if (mMinX <= rkBox.mMinX)
                rIntr.mMinX = rkBox.mMinX;
            else
                rIntr.mMinX = mMinX;

            // y-coordination
            if (mMaxY <= rkBox.mMaxY)
                rIntr.mMaxY = mMaxY;
            else
                rIntr.mMaxY = rkBox.mMaxY;

            if (mMinY <= rkBox.mMinY)
                rIntr.mMinY = rkBox.mMinY;
            else
                rIntr.mMinY = mMinY;

            // z-coordination
            if (mMaxZ <= rkBox.mMaxZ)
                rIntr.mMaxZ = mMaxZ;
            else
                rIntr.mMaxZ = rkBox.mMaxZ;

            if (mMinZ <= rkBox.mMinZ)
                rIntr.mMinZ = rkBox.mMinZ;
            else
                rIntr.mMinZ = mMinZ;
        }

        return bRet;
    }
}
