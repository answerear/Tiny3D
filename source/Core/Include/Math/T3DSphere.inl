

namespace Tiny3D
{
    inline Sphere::Sphere()
        : mCenter(0.0, 0.0, 0.0)
        , mRadius(0.0)
    {

    }

    inline Sphere::Sphere(const Vector3 &rkCenter, Real fRadius)
        : mCenter(rkCenter)
        , mRadius(fRadius)
    {

    }

    inline Sphere::Sphere(const Sphere &rkOther)
    {
        mCenter = rkOther.mCenter;
        mRadius = rkOther.mRadius;
    }

    inline Sphere &Sphere::operator =(const Sphere &rkOther)
    {
        mCenter = rkOther.mCenter;
        mRadius = rkOther.mRadius;
        return *this;
    }

    inline const Vector3 &Sphere::getCenter() const
    {
        return mCenter;
    }

    inline Vector3 &Sphere::getCenter()
    {
        return mCenter;
    }

    inline Real Sphere::getRadius() const
    {
        return mRadius;
    }

    inline Real &Sphere::getRadius()
    {
        return mRadius;
    }

    inline void Sphere::setCenter(const Vector3 &rkCenter)
    {
        mCenter = rkCenter;
    }

    inline void Sphere::setRadius(Real fRadius)
    {
        mRadius = fRadius;
    }

    inline bool Sphere::contains(const Vector3 &rkPoint) const
    {
        return ((rkPoint - mCenter).length() <= mRadius);
    }
}