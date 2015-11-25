

#include "Bound/T3DBound.h"
#include "SG/T3DSGVisual.h"
#include "Math/T3DTransform.h"


namespace Tiny3D
{
    Bound::Bound(uint32_t unID, SGVisual *node)
        : mID(unID)
        , mNode(node)
    {
        T3D_SAFE_ACQUIRE(mNode);
    }

    Bound::~Bound()
    {
        T3D_SAFE_RELEASE(mNode);
    }

    void Bound::setTransform(const Transform &transform)
    {
        const Vector3 &S = transform.getScale();
        const Vector3 &T = transform.getTranslate();
        const Quaternion &R = transform.getOrientation();

        Vector3 center = mSphere.getCenter();
        center = R * center;
        center = S * center;
        center = T + center;
        
        Real factor = std::max(std::max(S.x(), S.y()), S.z());
        Real radius = factor * mSphere.getRadius();

        mSphere.setCenter(center);
        mSphere.setRadius(radius);

        updateBound(transform);
    }

    bool Bound::test(const Bound &bound) const
    {
        bool result = false;

        switch (bound.getType())
        {
        case E_BT_SPHERE:
            {
                result = testSphere((const SphereBound &)bound);
            }
            break;
        case E_BT_AABB:
            {
                result = testAabb((const AabbBound &)bound);
            }
            break;
        case E_BT_OBB:
            {
                result = testObb((const ObbBound &)bound);
            }
            break;
        case E_BT_FRUSTUM:
            {
                result = testFrustum((const FrustumBound &)bound);
            }
            break;
        default:
            break;
        }

        return result;
    }
}
