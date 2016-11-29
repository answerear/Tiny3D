

#ifndef __T3D_SPHERE_H__
#define __T3D_SPHERE_H__


#include "T3DMathPrerequisites.h"
#include "T3DVector3.h"


namespace Tiny3D
{
    class T3D_MATH_API Sphere
    {
    public:
        Sphere();
        Sphere(const Vector3 &rkCenter, Real fRadius);
        Sphere(const Sphere &rkOther);

        Sphere &operator =(const Sphere &rkOther);

        const Vector3 &getCenter() const;
        Vector3 &getCenter();

        Real getRadius() const;
        Real &getRadius();

        void setCenter(const Vector3 &rkCenter);
        void setRadius(Real fRadius);

        bool contains(const Vector3 &rkPoint) const;

    private:
        Vector3 mCenter;
        Real    mRadius;
    };
}


#include "T3DSphere.inl"


#endif  /*__T3D_SPHERE_H__*/
