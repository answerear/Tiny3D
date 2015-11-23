

#ifndef __T3D_PLANE_H__
#define __T3D_PLANE_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Plane
    {
    public:
        Plane();
        Plane(const Vector3 &rkNormal, Real fDistance);
        Plane(const Vector3 &rkNormal, const Vector3 &rkPos);
        Plane(const Vector3 &rkPos1, const Vector3 &rkPos2, const Vector3 &rkPos3);
        Plane(const Plane &rkOther);
        virtual ~Plane();

        const Real operator [](int32_t idx) const;
        Real &operator [](int32_t idx);

        Real distanceToPoint(const Vector3 &rkPos) const;

        Vector3 getNormal() const;
        Real getDistance() const;

    protected:
        Real    mCoeff[4];
    };
}


#endif  /*__T3D_PLANE_H__*/
