

#include "Bound/T3DSphereBound.h"
#include "Bound/T3DAabbBound.h"
#include "Bound/T3DObbBound.h"
#include "Bound/T3DFrustumBound.h"


namespace Tiny3D
{
    SphereBound::SphereBound(uint32_t unID, SGVisual *node)
        : Bound(unID, node)
        , mOriginalSphere()
        , mRenderable(nullptr)
    {

    }

    SphereBound::~SphereBound()
    {

    }

    Bound::Type SphereBound::getType() const
    {
        return E_BT_SPHERE;
    }

    void SphereBound::setParams(const Vector3 &rkCenter, Real radius)
    {
        mOriginalSphere.setCenter(rkCenter);
        mOriginalSphere.setRadius(radius);
    }

    SGRenderable *SphereBound::getRenderable()
    {
        return nullptr;
    }

    bool SphereBound::testSphere(const SphereBound &bound) const
    {
        Real distance = mOriginalSphere.getCenter().squaredDistance(bound.mOriginalSphere.getCenter());
        Real radius = mOriginalSphere.getRadius() + bound.mOriginalSphere.getRadius();
        radius = radius * radius;
        return (distance <= radius);
    }

    bool SphereBound::testAabb(const AabbBound &bound) const
    {
        const Vector3 &center = mOriginalSphere.getCenter();
        Real radius = mOriginalSphere.getRadius();

        const Aabb &box = bound.getAlignAxisBox();
        Real minX = bound.getAlignAxisBox().getMinX();
        Real minY = bound.getAlignAxisBox().getMinY();
        Real minZ = bound.getAlignAxisBox().getMinZ();

        Real maxX = bound.getAlignAxisBox().getMaxX();
        Real maxY = bound.getAlignAxisBox().getMaxY();
        Real maxZ = bound.getAlignAxisBox().getMaxZ();

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

    bool SphereBound::testObb(const ObbBound &bound) const
    {
        Real distance = mOriginalSphere.getCenter().squaredDistance(bound.getSphere().getCenter());
        Real radius = mOriginalSphere.getRadius() + bound.getSphere().getRadius();
        radius = radius * radius;

        bool result = (distance <= radius);
        
        return result;
    }

    bool SphereBound::testFrustum(const FrustumBound &bound) const
    {
        return false;
    }

    void SphereBound::updateBound(const Transform &transform)
    {

    }
}
