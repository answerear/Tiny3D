

#include "T3DMath.h"
#include "T3DSphere.h"
#include "T3DAabb.h"
#include "T3DObb.h"
#include "T3DPlane.h"
#include "T3DFrustum.h"


namespace Tiny3D
{
    const Real Math::POS_INFINITY = std::numeric_limits<Real>::infinity();
    const Real Math::NEG_INFINITY = -std::numeric_limits<Real>::infinity();

    const Real Math::PI = Real(4.0 * atan(1.0));
    const Real Math::TWO_PI = Real(2.0 * PI);
    const Real Math::HALF_PI = Real(0.5 * PI);

    const Real Math::DEGREES_TO_RADIANS = PI / Real(180.0);
    const Real Math::RADIANS_TO_DEGREES = Real(180.0) / PI;

    bool Math::intersects(const Sphere &sphere1, const Sphere &sphere2)
    {
        Real distance = sphere1.getCenter().squaredDistance(sphere2.getCenter());
        Real radius = sphere1.getRadius() + sphere2.getRadius();
        radius = radius * radius;
        return (distance <= radius);
    }

    bool Math::intersects(const Sphere &sphere, const Aabb &box)
    {
        const Vector3 &center = sphere.getCenter();
        Real radius = sphere.getRadius();

        Real minX = box.getMinX();
        Real minY = box.getMinY();
        Real minZ = box.getMinZ();

        Real maxX = box.getMaxX();
        Real maxY = box.getMaxY();
        Real maxZ = box.getMaxZ();

        bool result = true;

        if (center.x() < minX && minX - center.x() > radius)
        {
            result = false;
        }
        else if (center.x() > maxX && center.x() - maxX > radius)
        {
            result = false;
        }
        else if (center.y() < minY && minY - center.y() > radius)
        {
            result = false;
        }
        else if (center.y() > maxY && center.y() - maxY > radius)
        {
            result = false;
        }
        else if (center.z() < minZ && minZ - center.z() > radius)
        {
            result = false;
        }
        else if (center.z() > maxZ && center.z() - maxZ > radius)
        {
            result = false;
        }

        return result;
    }

    bool Math::intersects(const Sphere &sphere, const Obb &box)
    {
        return false;
    }

    bool Math::intersects(const Sphere &sphere, const Frustum &frustum)
    {
        const Vector3 &center = sphere.getCenter();
        const Real &radius = sphere.getRadius();

        Real distance;

        int32_t i = 0;
        for (i = 0; i < Frustum::E_MAX_FACE; ++i)
        {
            const Plane &plane = frustum.getFace(Frustum::Face(i));
            distance = plane[0] * center.x() + plane[1] * center.y() + plane[2] * center.z() + plane[3];
            if (distance <= -radius)
                return false;
        }

        return true;
    }

    bool Math::intersects(const Sphere &sphere, const Plane &plane)
    {
        return (Math::Abs(plane.getNormal().dot(plane.getNormal()) - plane.getDistance()) < sphere.getRadius());
    }

    bool Math::intersects(const Aabb &aabb1, const Aabb &aabb2)
    {
        return false;
    }

    bool Math::intersects(const Aabb &aabb, const Frustum &frustum)
    {
        const Real &minX = aabb.getMinX();
        const Real &maxX = aabb.getMaxX();
        const Real &minY = aabb.getMinY();
        const Real &maxY = aabb.getMaxY();
        const Real &minZ = aabb.getMinZ();
        const Real &maxZ = aabb.getMaxZ();

        int32_t i = 0;
        for (i = 0; i < Frustum::E_MAX_FACE; ++i)
        {
            const Plane &plane = frustum.getFace(Frustum::Face(i));
            
            if (plane[0] * minX + plane[1] * minY + plane[2] * minZ + plane[3] > 0)
                continue;

            if (plane[0] * maxX + plane[1] * minY + plane[2] * minZ + plane[3] > 0)
                continue;

            if (plane[0] * minX + plane[1] * maxY + plane[2] * minZ + plane[3] > 0)
                continue;

            if (plane[0] * maxX + plane[1] * maxY + plane[2] * minZ + plane[3] > 0)
                continue;

            if (plane[0] * minX + plane[1] * minY + plane[2] * maxZ + plane[3] > 0)
                continue;

            if (plane[0] * maxX + plane[1] * minY + plane[2] * maxZ + plane[3] > 0)
                continue;

            if (plane[0] * minX + plane[1] * maxY + plane[2] * maxZ + plane[3] > 0)
                continue;

            if (plane[0] * maxX + plane[1] * maxY + plane[2] * maxZ + plane[3] > 0)
                continue;

            return false;
        }

        return true;
    }

    bool Math::intersects(const Aabb &box, const Plane &plane)
    {
        return false;
    }

    bool Math::intersects(const Aabb &aabb, const Obb &obb)
    {
        return false;
    }

    bool Math::intersects(const Obb &box1, const Obb &box2)
    {
        return false;
    }

    bool Math::intersects(const Obb &box, const Frustum &frustum)
    {
        return false;
    }

    bool Math::intersects(const Obb &box, const Plane &plane)
    {
        return false;
    }
}
