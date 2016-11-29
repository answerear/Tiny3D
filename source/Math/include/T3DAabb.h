

#ifndef __T3D_AABB_H__
#define __T3D_AABB_H__


#include "T3DMathPrerequisites.h"
#include "T3DSphere.h"


namespace Tiny3D
{
    class T3D_MATH_API Aabb
    {
    public:
        Aabb();
        Aabb(Real fXMin, Real fXMax, 
            Real fYMin, Real fYMax, 
            Real fZMin, Real fZMax);

        bool hasXOverlap(const Aabb &rkBox) const;
        bool hasYOverlap(const Aabb &rkBox) const;
        bool hasZOverlap(const Aabb &rkBox) const;

        bool testIntersection(const Aabb &rkBox) const;

        bool findIntersection(const Aabb &rkBox, Aabb &rIntr) const;

        Real getWidth() const;
        Real getHeight() const;
        Real getDepth() const;

        Real getMinX() const;
        Real getMaxX() const;

        Real getMinY() const;
        Real getMaxY() const;

        Real getMinZ() const;
        Real getMaxZ() const;

        const Vector3 &getCenter() const;
        Real getRadius() const;

        const Sphere &getSphere() const;

        void setWidth(Real fWidth);
        void setHeight(Real fHeight);
        void setDepth(Real fDepth);

        void setMinX(Real x);
        void setMaxX(Real x);

        void setMinY(Real y);
        void setMaxY(Real y);

        void setMinZ(Real z);
        void setMaxZ(Real z);

        void setParam(const Vector3 &vMin, const Vector3 &vMax);

    private:
        Real    mMinX;
        Real    mMaxX;

        Real    mMinY;
        Real    mMaxY;

        Real    mMinZ;
        Real    mMaxZ;

        Sphere  mSphere;
    };
}


#include "T3DAabb.inl"


#endif  /*__T3D_AABB_H__*/
