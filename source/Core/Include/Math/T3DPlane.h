

#ifndef __T3D_PLANE_H__
#define __T3D_PLANE_H__


#include "T3DPrerequisites.h"
#include "Math/T3DVector3.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Plane
    {
    public:
        enum Side
        {
            E_SIDE_NONE,
            E_SIDE_POSITIVE,
            E_SIDE_NEGATIVE,
        };

        Plane();
        Plane(const Vector3 &rkNormal, Real fDistance);
        Plane(const Vector3 &rkNormal, const Vector3 &rkPos);
        Plane(const Vector3 &rkPos1, const Vector3 &rkPos2, const Vector3 &rkPos3);
        Plane(const Plane &rkOther);

        Plane &operator =(const Plane &rkOther);

        bool operator ==(const Plane &rkOther) const;
        bool operator !=(const Plane &rkOther) const;

        const Real operator [](int32_t idx) const;
        Real &operator [](int32_t idx);

        Real distanceToPoint(const Vector3 &rkPos) const;

        Side sideForPoint(const Vector3 &rkPos) const;

        Vector3 getNormal() const;
        Real getDistance() const;

    protected:
        Real    mCoeff[4];
    };
}


#include "T3DPlane.inl"


#endif  /*__T3D_PLANE_H__*/
